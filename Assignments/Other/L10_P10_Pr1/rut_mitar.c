#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mitar.h"

extern char *uso;


int copynFile(FILE *origen, FILE *destimo, int nBytes);
int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles);


int createTar(int nFiles, char *fileNames[], char tarName[]) {
  
  int i;
  FILE *tarFile, *inputFile;
  stHeaderEntry *header;
  
  //fichero[s]
  if(nFiles<=0) {
    fprintf(stderr, uso);
    return(EXIT_FAILURE);
  }

  //Abrimos el fichero destino
  if((tarFile=fopen(tarName,"w+")) == NULL)
  {
  	perror("Error al crear el ficher");
	return (EXIT_FAILURE);
  }

  //Reservamos memoria para la estructura cabecera del mtar

  //Casteo para pasar de void a struct
  header= (stHeaderEntry*)malloc(sizeof(stHeaderEntry)*nFiles);  


  //Nos posicionamos enla zona de datos del fichero mtar

  //Tamaño de la cabecera:
  int posFile = sizeof(unsigned int) + sizeof(stHeaderEntry)*nFiles;
  //Desplazar desde el comienzo del fichero
  fseek(tarFile, posFile, SEEK_SET);

  //Relleno la cabecera en RAM y copio los datos de los ficheros en el tar
  for(i=0; i<nFiles; i++) {
    //Abrimos fichero fuente
    if((inputFile=fopen(fileNames[i], "r")) == NULL)
    {
    	perror("Error al abrir el archivo");
    	return(EXIT_FAILURE);
    }
    //Completamos la cabecera en memoria (NO en el fichero)
    //Copiar el nombre del fichero a una cabecera
    strcpy(header[i].name, fileNames[i]);
    //Nos colocamos al final del fichero
    fseek(inputFile, 0, SEEK_END);
    //Cogemos esa posicion (en bytes) con ftell
    header[i].size = ftell(inputFile);
    //Volvemos al inicio del archivo
    fseek(inputFile, 0, SEEK_SET);

    //Copiamos el fichero origen al fichero TAR (si da error cerramos los archivos)
    if(copynFile(inputFile, tarFile, header[i].size)!=header[i].size)
    {
    	fclose(tarFile);
    	fclose(inputFile);
    	perror("Error al copiar los archivos");
    	return(EXIT_FAILURE);
    } 
    fclose(inputFile);
   }

  // Escribimos al fichero la cabecera (num. ficheros y lo que hemos ido creando en memoria)
  fseek(tarFile, 0, SEEK_SET);
  fwrite(&nFiles, sizeof(unsigned int), 1, tarFile);
  fwrite(header, posFile - sizeof(unsigned int), 1, tarFile);
  
  fclose(tarFile);

  return(EXIT_SUCCESS);
}

int extractTar(char tarName[]) {
  stHeaderEntry *header;
  int nFiles,i;
  FILE *tarFile,*outputFile;
  
  //Abrimos el tarball
  if((tarFile=fopen(tarName, "r")) == NULL)
  {
  	perror("Error al abrir el archivo");
	return(EXIT_FAILURE);
  }  

  //Leemos la cabecera del mtar, utilizando la función readHeader
  readHeader(tarFile, &header, &nFiles);

  //Creamos los ficheros contenidos en el mtar. Para ello recorremos secuencialmente la cabecera,
  // creando (abriendo en escritura) los ficheros indicados en cada uno de los descriptores y copiando
  // en ellos su contenido, que se leerá del tarball usando la función copynFile.
  for(i=0; i< nFiles; i++)
  {
  	if((outputFile=fopen(header[i].name, "w+")) == NULL)
	{
		perror("Error al abrir el archivo \n");
		return(EXIT_FAILURE);
	}
	//Copiamos el contenido
	copynFile(tarFile, outputFile, header[i].size);
	
  	fclose(outputFile);
  }

  //Liberamos memoria de la cabecera y cerramos el tarball
  free(header);
  fclose(tarFile);
 
  return(EXIT_SUCCESS);
}
  
int copynFile(FILE *origen, FILE *destimo, int nBytes){
  int nCopy=0;
  char c;

  // Copiar todo el contenido del fichero origen, byte a byte, al destino.
  for(nCopy=0; nCopy<nBytes; nCopy++)
  {
  	fread(&c, sizeof(char), 1, origen);
	fwrite(&c, sizeof(char), 1, destimo);
  }
  // Devolvemos en nCopy los bytes copiados.
  
  return(nCopy);
}

int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles){

  //Primero leemos el número de ficheros contenidos en el tarball
  fread(nFiles, sizeof(unsigned int), 1, tarFile);  

  //Reservamos memoria del heap para la cabecera del tarball
  *header = (stHeaderEntry*)malloc(sizeof(stHeaderEntry)*(*nFiles));  

  //Leemos toda la cabecera
  fread(*header, sizeof(stHeaderEntry)*(*nFiles), 1, tarFile);

  return(EXIT_SUCCESS);
}
