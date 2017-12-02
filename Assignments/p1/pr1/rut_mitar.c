#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mitar.h"

extern char *uso;

/** Copia el nBytes del fichero origen al fichero destino.
 *
 * origen: puntero al descriptor FILE del fichero origen
 * destino: puntero al descriptor FILE del fichero destino
 * nBytes: numero de bytes a copiar
 *
 * Devuelve el numero de bytes copiados o -1 en caso de error. 
 */
int copynFile(FILE *origen, FILE *destimo, int nBytes){
 // Completar la funci—n
    int nCopy=0;
    char c;
    // Copiar todo el contenido del fichero origen, byte a byte, al destino.
    fopen(origen);
    fopen(destino);
    
    while(!feof(origen)) {
        lector = fread(c, 1, sizeof(c), origen);  //Leo datos: cada uno de 1 byte, todos los que me caben
        fwrite(c, 1, lector, *destino); //
    }
    fclose(origen);     // Cerramos los ficheros
    fclose(destino);
    // Devolvemos en nCopy los bytes copiados.
    while(lector<nBytes && (c=getc(origen)!=EOF))
    {
        putc(c,destino);
        lector++;
    }
    return(lector);
}

/** Carga en memoria la cabecera del tarball.
 *
 * tarFile: puntero al descriptor FILE del tarball
 * header: dirección de un puntero que se inicializará con la dirección de un
 * buffer que contenga los descriptores de los ficheros cargados.
 * nFiles: dirección de un entero (puntero a entero) que se inicializará con el
 * número de ficheros contenido en el tarball (primeros bytes de la cabecera).
 *
 * Devuelve EXIT_SUCCESS en caso de exito o EXIT_FAILURE en caso de error
 * (macros definidas en stdlib.h).
 */
int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles){
 // Completar la función
    //Primero leemos el numero de ficheros contenidos en el tarball
    stHeaderEntry* array=NULL;
    int nr_files=0;
    //Reservamos memoria del heap para la cabecera del tarball
    array=(stHeaderEntry*)malloc(sizeof(stHeaderEntry)*(*nFiles));
    //Leemos toda la cabecera
    (*nFiles)=nr_files;
    (*header)=array;
    return(EXIT_SUCCESS);
}

/** Carga una cadena de caracteres de un fichero.
 *
 * file: dirección de la estructura FILE del fichero
 * buf: dirección del puntero en que se inicializará con la dirección donde se
 * copiará la cadena. Será una dirección del heap obtenida con malloc.
 *
 * Devuelve: 0 si tuvo exito, -1 en caso de error.
 */
int loadstr( FILE *file, char** buf ){
 // Completar la función
    
    
}

/** crea un tarball a partir de unos ficheros de entrada.
 *
 * nfiles:    numero de ficheros de entrada
 * filenames: array con los nombres de los ficheros de entrada (rutas)
 * tarname:   nombre del tarball
 * 
 * devuelve exit_success en caso de exito y exit_failure en caso de error
 * (macros definidas en stdlib.h).
 *
 * ayuda: calcular primero el espacio necesario para la cabecera del tarball,
 * saltar dicho espacio desde el comienzo del archivo e ir copiando uno a uno
 * los ficheros, rellenando a la vez una representación de la cabecera del
 * tarball en memoria. al final copiar la cabecera al comienzo del tarball.
 * recordar que  que no vale un simple sizeof de stheaderentry para saber el
 * tamaño necesario en disco para el descriptor de un fichero puesto que sizeof
 * no calcula el tamaño de la cadena apuntada por name, sino el tamaño del
 * propio puntero. para calcular el tamaño de una cadena c puede usarse la
 * función strlen.
 */
int createTar(int nFiles, char *fileNames[], char tarName[]) {
 // Completar la función
    int i;
    FILE *tarFile, *inputFile;
    stHeaderEntry *header;
    stHeaderEntry cabeceras[nFiles]; // Para guardar las cabeceras en memoria y luego volcarlas directamente en el archivo
    
    //fichero[s]
    if(nFiles<=0) {
        fprintf(stderr, uso);
        return(EXIT_FAILURE);
    }
    
    //Abrimos el fichero destino
    //tarFile = fopen(tarName, "w");
    //por si da error ponemos un if
    if((tarFile = fopen(tarName, "w")==NULL)
       {
           fprintf("Error en la apertura del fichero.\n");
           return(EXIT_FAILURE);
       }
       //Reservamos memoria para la estructura cabecera del mtar
       header = malloc(sizeof(stHeaderEntry)*nFiles);
       //CREO QUE ES ASI header = (stHeaderEntry*)malloc(sizeof(stHeaderEntry)*nFiles)
       //Nos posicionamos enla zona de datos del fichero mtar
       fseek(tarFile, 0L, nFiles*sizeof(stHeaderEntry)+sizeof(int));
       //fseek(nombre_del_fichero, Desplazamiento (""CREO""), posicion)
       
       
       //Relleno la cabecera en RAM y copio los datos de los ficheros en el tar
       for(i=0; i<nFiles; i++) {
           //Abrimos fichero fuente
           inputFile = fopen(fileNames[i], "r");
           //Completamos la cabecera en memoria (NO en el fichero)
           //Copiamos el fichero origen al fichero TAR
           cabeceras[i].name = fileNames[i];
           cabeceras[i].size = copynFile(inputFile, tarFile, INT_MAX);
           fclose(inputFile);
       }
       // Escribimos al fichero la cabecera (num. ficheros y lo que hemos ido creando en memoria)
       fseek(inputFile, 0L, SEEK_SET); //ARGUMENTOS: NOMBRE DEL FICHERO, Despl, Y SEEK_SET PARA EL INICIO DEL FICHERO
       // Funcion write?
       fclose(tarFile);
       
       return(EXIT_SUCCESS);
}

/** Extrae todos los ficheros de un tarball.
 *
 * tarName: cadena C con el nombre del tarball
 *
 * Devuelve EXIT_SUCCES en caso de exito y EXIT_FAILURE en caso de error (macros
 * definidas en stdlib.h).
 *
 * AYUDA: debemos cargar primero la cabecera, con lo que nos situaremos al
 * comienzo del primer fichero almacenado. Entonces recorriendo la cabecera
 * sabremos el nombre que debemos ir dando a los ficheros y la cantidad de bytes
 * que debemos leer del fichero para cargar su contenido.
 */
int extractTar(char tarName[]) {
 // Completar la función
    stHeaderEntry *header;
    int nFiles,i;
    FILE *tarFile,*outputFile;
    
    //Abrimos el tarball
    tarFile=fopen(tarName,"r");
    //Leemos la cabecera del mtar, utilizando la funcionn readHeader
    readHeader(tarFile, &header, &nFiles);
    //Creamos los ficheros contenidos en el mtar. Para ello recorremos secuencialmente la cabecera,
    // creando (abriendo en escritura) los ficheros indicados en cada uno de los descriptores y copiando
    // en ellos su contenido, que se leera del tarball usando la funcion copynFile.
    
    //Liberamos memoria de la cabecera y cerramos el tarball
    free(header);
    fclose(tarFile);
    
    return(EXIT_SUCCESS);
}

