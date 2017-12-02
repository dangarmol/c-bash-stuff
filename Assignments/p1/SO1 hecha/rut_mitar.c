#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mitar.h"

extern char *uso;

int loadstr( FILE *file, char** buf);

/** Copia el nBytes del fichero origen al fichero destino.
 *
 * origen: puntero al descriptor FILE del fichero origen
 * destino: puntero al descriptor FILE del fichero destino
 * nBytes: numero de bytes a copiar
 *
 * Devuelve el numero de bytes copiados o -1 en caso de error. 
 */
int copynFile(FILE *origen, FILE *destino, int nBytes){
 	int nCopy = 0;
 	char c;

 	while (!feof(origen) && nCopy < nBytes - 1) {
 		c = getc(origen);
 		putc(c, destino);
 		nCopy++;
 	}

 	c = getc(origen);

 	return(nCopy);
}

/** Carga en memoria la cabecera del tarball.
 *
 * tarFile: puntero al descriptor FILE del tarball
 * header: direcci�n de un puntero que se inicializar� con la direcci�n de un
 * buffer que contenga los descriptores de los ficheros cargados.
 * nFiles: direcci�n de un entero (puntero a entero) que se inicializar� con el
 * n�mero de ficheros contenido en el tarball (primeros bytes de la cabecera).
 *
 * Devuelve EXIT_SUCCESS en caso de exito o EXIT_FAILURE en caso de error
 * (macros definidas en stdlib.h).
 */
int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles){
	stHeaderEntry* array = NULL;
	int nr_files = 0;
	int n = 0, err = 0;
	char *buf = NULL, aux;

	fread(&nr_files, sizeof(int), 1, tarFile);

	array = malloc(sizeof(stHeaderEntry)*nr_files);

	if (array == NULL) {
		fprintf(stderr,"Error de memoria.");
		fclose(tarFile);
		return (EXIT_FAILURE);
	}

	while (n < nr_files && err != 1) {
		err = loadstr(tarFile, &buf);
		if (err != 1) {
			array[n].name = buf;
			fread(&(array[n].size), sizeof(int), 1, tarFile);
			n++;
		}
	}

	n = 0;
	while (n == 0 || n < nr_files - 1) {
		fread(&aux, 1, 1, tarFile);
		n++;
	}
	(*nFiles) = nr_files;
	(*header) = array;

	return (EXIT_SUCCESS);
}

/** Carga una cadena de caracteres de un fichero.
 *
 * file: direcci�n de la estructura FILE del fichero
 * buf: direcci�n del puntero en que se inicializar� con la direcci�n donde se
 * copiar� la cadena. Ser� una direcci�n del heap obtenida con malloc.
 *
 * Devuelve: 0 si tuvo exito, -1 en caso de error.
 */
int loadstr( FILE *file, char **buf){
	int ch;
	size_t len;
	char strBuf[PATH_MAX];
	const int STRLEN = 256;

	ch = getc(file);
	if (ch == ' ')
		return (1);

	len = 0;
	while (ch != '\0') {
		if (len >= STRLEN || ch == EOF || ch == '\n')
			return (1);

		if (ch == '\\') ch = getc(file);
		strBuf[len++] = (char)ch;
		ch = getc(file);
	}
	strBuf[len] = '\0';
	if ((*buf = (char*)malloc(len+1)) == NULL)
		return (1);
	strcpy (*buf, strBuf);

	return (0);
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
 * los ficheros, rellenando a la vez una representaci�n de la cabecera del
 * tarball en memoria. al final copiar la cabecera al comienzo del tarball.
 * recordar que  que no vale un simple sizeof de stheaderentry para saber el
 * tama�o necesario en disco para el descriptor de un fichero puesto que sizeof
 * no calcula el tama�o de la cadena apuntada por name, sino el tama�o del
 * propio puntero. para calcular el tama�o de una cadena c puede usarse la
 * funci�n strlen.
 */
int createTar(int nFiles, char *fileNames[], char tarName[]) {
	int i, espacio = 0;
	FILE *tarFile, *inputFile;
	stHeaderEntry *header;

	if (nFiles <= 0) {
		fprintf(stderr, uso);
		return (EXIT_FAILURE);
	}

	tarFile = fopen(tarName, "wb");
	if (tarFile == NULL) {
		fprintf(stderr, "Error al abrir el archivo %s:\n", tarName);
		perror(NULL);
			return (EXIT_FAILURE);
	}

	for (espacio = 0; espacio < nFiles; espacio++) {
		espacio = espacio + sizeof(fileNames[espacio]);
	}
	espacio = espacio + sizeof(stHeaderEntry) + 1;
	header = malloc(sizeof(stHeaderEntry) * nFiles);
	fseek(tarFile, nFiles * espacio + sizeof(int), SEEK_SET);

	for (i = 0; i < nFiles; i++) {
		inputFile = fopen(fileNames[i], "rb");
		if (inputFile == NULL) {
			fprintqf(stderr, "Error al abrir el arhivo %s:\n", fileNames[i]);
			perror(NULL);
			free(header);
			fclose(tarFile);
			return (EXIT_FAILURE);
		}

		header[i].name = malloc(sizeof(fileNames[i]));
		strcpy(header[i].name, fileNames[i]);
		header[i].size = copynFile(inputFile, tarFile, INT_MAX);
		fclose(inputFile);
	}

	rewind(tarFile);
	fwrite(&nFiles, sizeof(int), 1, tarFile);
	for (i = 0; i < nFiles; i++) {
		fwrite(header[i].name, sizeof(char), strlen(header[i].name)+ 1, tarFile);
		fwrite(&(header[i].size), sizeof (int), 1, tarFile);
	}
	printf("El archivo tar fue creado con exito.\n");
	free(header);
	fclose(tarFile);

	return (EXIT_SUCCESS);
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
	stHeaderEntry *header;
	int nFiles, i;
	FILE *tarFile, *outputFile;

	tarFile = fopen(tarName, "a+b");
	if (tarFile == NULL) {
		fprintf(stderr, "Error al abrir el archivo %s\n", tarName);
		perror(NULL);
		return (EXIT_FAILURE);
	}
	if (readHeader(tarFile, &header, &nFiles) == EXIT_FAILURE) {
		fclose(tarFile);
		return (EXIT_FAILURE);
	}

	for (i = 0; i < nFiles; i++) {
		outputFile = fopen(header[i].name, "wb");
		if (outputFile == NULL) {
			fprintf(stderr, "Error al abrir el archivo %s:\n", header[i].name);
			free(header);
			fclose(tarFile);
			return (EXIT_FAILURE);
		}

		copynFile(tarFile, outputFile, header[i].size);
		fclose(outputFile);
	}
	printf("Archivo tar creado con exito. \n");
	free(header);
	fclose(tarFile);
	return (EXIT_SUCCESS);
}
