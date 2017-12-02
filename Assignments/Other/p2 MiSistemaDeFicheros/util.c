#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Formatea el disco virtual. Guarda el mapa de bits del super bloque 
// y el directorio único.

int myMkfs(MiSistemaDeFicheros* miSistemaDeFicheros, int tamDisco, char* nombreArchivo) {
	// Creamos el disco virtual:
	miSistemaDeFicheros->discoVirtual = open(nombreArchivo, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	// Algunas comprobaciones mínimas:
	assert(sizeof (EstructuraSuperBloque) <= TAM_BLOQUE_BYTES);
	assert(sizeof (EstructuraDirectorio) <= TAM_BLOQUE_BYTES);
	int numBloques = tamDisco / TAM_BLOQUE_BYTES;
	int minNumBloques = 3 + MAX_BLOQUES_CON_NODOSI + 1;
	int maxNumBloques = NUM_BITS;
	if (numBloques < minNumBloques) {
		return 1;
	}
	if (numBloques >= maxNumBloques) {
		return 2;
	}

	/// MAPA DE BITS
	// Inicializamos el mapa de bits
	int i;
	for (i = 0; i < NUM_BITS; i++) {
		miSistemaDeFicheros->mapaDeBits[i] = 0;
	}

	// Los primeros tres bloques tendrán el superbloque, mapa de bits y directorio
	miSistemaDeFicheros->mapaDeBits[MAPA_BITS_IDX] = 1;
	miSistemaDeFicheros->mapaDeBits[SUPERBLOQUE_IDX] = 1;
	miSistemaDeFicheros->mapaDeBits[DIRECTORIO_IDX] = 1;
	// Los siguientes NUM_INODE_BLOCKS contendrán nodos-i
	for (i = 3; i < 3 + MAX_BLOQUES_CON_NODOSI; i++) {
		miSistemaDeFicheros->mapaDeBits[i] = 1;
	}
	escribeMapaDeBits(miSistemaDeFicheros);

	/// DIRECTORIO
	// Inicializamos el directorio (numArchivos, archivos[i].libre) y lo escribimos en disco

	miSistemaDeFicheros->directorio.numArchivos = 0;
	for(i = 0; i < MAX_ARCHIVOS_POR_DIRECTORIO; i++)
	{
		miSistemaDeFicheros->directorio.archivos[i].libre = 1;
	}
	escribeDirectorio(miSistemaDeFicheros);
	 	

	/// NODOS-I
	EstructuraNodoI nodoActual;
	nodoActual.libre = 1;
	// Escribimos nodoActual MAX_NODOSI veces en disco
	int j;
	for(j = 0; j < MAX_NODOSI; j++)
	{
		escribeNodoI(miSistemaDeFicheros, j, &nodoActual);		
	}

	/// SUPERBLOQUE
	// Inicializamos el superbloque (ver common.c) y lo escribimos en disco
	// ...
	initSuperBloque(miSistemaDeFicheros, tamDisco);
	escribeSuperBloque(miSistemaDeFicheros);
	sync();

	// Al finalizar tenemos al menos un bloque
	assert(myQuota(miSistemaDeFicheros) >= 1);

	printf("SF: %s, %d B (%d B/bloque), %d bloques\n", nombreArchivo, tamDisco, TAM_BLOQUE_BYTES, numBloques);
	printf("1 bloque para SUPERBLOQUE (%lu B)\n", sizeof(EstructuraSuperBloque));
	printf("1 bloque para MAPA DE BITS, que cubre %lu bloques, %lu B\n", NUM_BITS, NUM_BITS * TAM_BLOQUE_BYTES);
	printf("1 bloque para DIRECTORIO (%lu B)\n", sizeof(EstructuraDirectorio));
	printf("%d bloques para nodos-i (a %lu B/nodo-i, %lu nodos-i)\n",MAX_BLOQUES_CON_NODOSI,sizeof(EstructuraNodoI),MAX_NODOSI);
	printf("%d bloques para datos (%d B)\n",miSistemaDeFicheros->superBloque.numBloquesLibres,TAM_BLOQUE_BYTES*miSistemaDeFicheros->superBloque.numBloquesLibres);
	printf("¡Formato completado!\n");
	return 0;
}

int myImport(char* nombreArchivoExterno, MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno) {
	int handle = open(nombreArchivoExterno, O_RDONLY);
	if (handle == -1) {
		return 1;
	}

	/// Comprobamos que podemos abrir el archivo a importar
	struct stat stStat;
	if (stat(nombreArchivoExterno, &stStat) != false) {
		return 2;
	}

	/// Comprobamos que hay suficiente espacio
	// stStat.st_size > ...
	// ...
	if(((stStat.st_size + TAM_BLOQUE_BYTES - 1)/TAM_BLOQUE_BYTES) > miSistemaDeFicheros->superBloque.numBloquesLibres)
	{
		fprintf(stderr, "No hay suficiente espacio");
		return 3;
	}
	

	/// Comprobamos que el tamaño total es suficientemente pequeño para ser almacenado en MAX_BLOCKS_PER_FILE
	// stStat.st_size > ...
	if(((stStat.st_size + TAM_BLOQUE_BYTES - 1)/TAM_BLOQUE_BYTES) > MAX_BLOQUES_POR_ARCHIVO)
	{
		fprintf(stderr, "El archivo es demasiado grande");
		return 4;
	}
	

	/// Comprobamos que la longitud del nombre del archivo es adecuada
	if(strlen(nombreArchivoInterno) > MAX_TAM_NOMBRE_ARCHIVO)
	{
		fprintf(stderr, "Tamanio del nombre demasiado grande");
		return 5;
	}

	/// Comprobamos que el fichero no exista, y localizamos el primer archivo libre disponible
	int i = 0;
	BOOLEAN archivoLibreEncontrado = false;
	int idxArchivoLibre = buscaPosDirectorio(miSistemaDeFicheros, nombreArchivoInterno);
	if(idxArchivoLibre != -1){
		fprintf(stderr, "El fichero ya existe \n");
		return 6;
	}
	else
		archivoLibreEncontrado = true;
	

	/// Comprobamos que existe un nodo-i libre
	if (miSistemaDeFicheros->numNodosLibres <= 0) {
		return 7;
	}

	/// Comprobamos que todavía cabe un archivo en el directorio (MAX_ARCHIVOS_POR_DIRECTORIO)
	if (miSistemaDeFicheros->directorio.numArchivos >= MAX_ARCHIVOS_POR_DIRECTORIO) {
		return 8;
	}

	/// Actualizamos toda la información:
	/// directorio, mapa de bits, nodo-i, bloques de datos, superbloque ...
	int entradaLibre;
	BOOLEAN encontrado = false;
	for(i = 0; i < MAX_ARCHIVOS_POR_DIRECTORIO && !encontrado;i++){
		if(miSistemaDeFicheros->directorio.archivos[i].libre){
		  entradaLibre = i;
		  encontrado = true;
		}
		else
		  entradaLibre = -1;
	}
	
	miSistemaDeFicheros->directorio.numArchivos++;
	miSistemaDeFicheros->directorio.archivos[entradaLibre].idxNodoI = entradaLibre;
	strcpy(miSistemaDeFicheros->directorio.archivos[entradaLibre].nombreArchivo,nombreArchivoInterno);
	miSistemaDeFicheros->directorio.archivos[entradaLibre].libre = false;

	escribeDirectorio(miSistemaDeFicheros);


	int numNodoI;
	if ((numNodoI = buscaNodoLibre(miSistemaDeFicheros)) == -1) {
		return 9;
	}

	/* Calcula division  (stStat.st_size/TAM_BLOQUE_BYTES) redondeada por arriba */

	int numeroBloques = ((stStat.st_size + TAM_BLOQUE_BYTES -1) / TAM_BLOQUE_BYTES); 
	
	//Creamos el nodo, le inicializamos y hacemos que nuestro SF apunte a el.
	miSistemaDeFicheros->nodosI[numNodoI] = malloc(sizeof(EstructuraNodoI));	
	miSistemaDeFicheros->nodosI[numNodoI]->tamArchivo = stStat.st_size;
	miSistemaDeFicheros->nodosI[numNodoI]->numBloques = numeroBloques;
	miSistemaDeFicheros->nodosI[numNodoI]->libre = false;
	miSistemaDeFicheros->nodosI[numNodoI]->tiempoModificado = time(NULL);

	reservaBloquesNodosI(miSistemaDeFicheros, miSistemaDeFicheros->nodosI[numNodoI]->idxBloques, numeroBloques);

	//Guardamos los datos del fichero en la ruta del disco
	miSistemaDeFicheros->superBloque.numBloquesLibres -= numeroBloques;	
	escribeDatos(miSistemaDeFicheros, handle, numNodoI);
	escribeMapaDeBits(miSistemaDeFicheros);
	escribeSuperBloque(miSistemaDeFicheros);
	escribeNodoI(miSistemaDeFicheros, numNodoI, miSistemaDeFicheros->nodosI[numNodoI]);

	sync();
	close(handle);
	return 0;
}

int myExport(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno, char* nombreArchivoExterno) {
	// Buscamos el archivo "nombreArchivoInterno"
	int posDirectorio = buscaPosDirectorio(miSistemaDeFicheros, nombreArchivoInterno);
	if(posDirectorio == -1) {
		perror("El archivo no existe");
		return 1;
	} 

	int numNodoActual = miSistemaDeFicheros->directorio.archivos[posDirectorio].idxNodoI;	

	// Preguntamos si sobreescribir:
	int handle;
	int archivoExiste;
	char sobreEscribir;
	if ((handle = open(nombreArchivoExterno, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
		archivoExiste = errno;
		if (archivoExiste == EEXIST) {
			printf("El archivo \"%s\" ya existe, ¿sobreescribo? (S/N): ", nombreArchivoExterno);
			sobreEscribir = tolower(getchar());
			if (sobreEscribir == 'n')
				return 0;
		}
		errno = archivoExiste;
		perror("open(...,O_CREAT|O_WRONLY,...)");
	}

	// Copiamos bloque a bloque del archivo interno al externo:
	EstructuraNodoI* temp = miSistemaDeFicheros->nodosI[numNodoActual];
	char buffer[TAM_BLOQUE_BYTES];
	
	int i;
	for (i = 0; i < temp->numBloques - 1; i++) {	
		lseek(miSistemaDeFicheros->discoVirtual, temp->idxBloques[i] * TAM_BLOQUE_BYTES, SEEK_SET);
		read(miSistemaDeFicheros->discoVirtual, &buffer, TAM_BLOQUE_BYTES);
		write(handle, &buffer, TAM_BLOQUE_BYTES);	
    	}
	// Calculamos los bytes restantes y los escribimos:
	int bytesRestantes = miSistemaDeFicheros->superBloque.tamBloque - (temp->numBloques* miSistemaDeFicheros->superBloque.tamBloque - temp->tamArchivo);
	lseek(miSistemaDeFicheros->discoVirtual, temp->idxBloques[i] * TAM_BLOQUE_BYTES, SEEK_SET);
	read(miSistemaDeFicheros->discoVirtual, &buffer, bytesRestantes);
	write(handle, &buffer, bytesRestantes);
	if (close(handle) == -1) {
		return 2;
	}
	return 0;
}

int myRm(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivo) {
	/// Completar:
	// Busca el archivo con nombre "nombreArchivo"
	int posDirectorio = buscaPosDirectorio(miSistemaDeFicheros, nombreArchivo);
	if(posDirectorio == -1) {
		fprintf(stderr, "El archivo a borrar no existe \n");
		return 1;
	}
	// Obtiene el nodo-i asociado y lo actualiza
	int posNodoI = miSistemaDeFicheros->directorio.archivos[posDirectorio].idxNodoI;
	miSistemaDeFicheros->nodosI[posNodoI]->libre = 1;

	// Actualiza el superbloque (numBloquesLibres) y el mapa de bits
	miSistemaDeFicheros->superBloque.numBloquesLibres += miSistemaDeFicheros->nodosI[posNodoI]->numBloques;
	int i;
	for (i = 0; i < miSistemaDeFicheros->nodosI[posNodoI]->numBloques; i++) {
		miSistemaDeFicheros->mapaDeBits[miSistemaDeFicheros->nodosI[posNodoI]->idxBloques[i]] = 0;
	}		
	
	// Actualiza el archivo
	miSistemaDeFicheros->directorio.archivos[posDirectorio].libre = 1;
	miSistemaDeFicheros->directorio.numArchivos--;

	// Finalmente, actualiza en disco el directorio, nodoi, mapa de bits y superbloque
	escribeDirectorio(miSistemaDeFicheros);	
	escribeNodoI(miSistemaDeFicheros, posNodoI, miSistemaDeFicheros->nodosI[posNodoI]);	
	escribeSuperBloque(miSistemaDeFicheros);
	escribeMapaDeBits(miSistemaDeFicheros);

	// Libera el puntero y lo hace NULL
	free(miSistemaDeFicheros->nodosI[posNodoI]);
	miSistemaDeFicheros->nodosI[posNodoI] = NULL;	
	return 0;
}

void myLs(MiSistemaDeFicheros* miSistemaDeFicheros) {
	int numArchivosEncontrados = 0;
	//EstructuraNodoI nodoActual;
	int i;
	// Recorre el sistema de ficheros, listando los archivos encontrados
	for (i = 0; i < MAX_ARCHIVOS_POR_DIRECTORIO; i++) {
		if(miSistemaDeFicheros->directorio.archivos[i].libre == 0) {	
			printf("%s\t", miSistemaDeFicheros->directorio.archivos[i].nombreArchivo);
			printf("%d\t", miSistemaDeFicheros->nodosI[miSistemaDeFicheros->directorio.archivos[i].idxNodoI]->tamArchivo);
			struct tm *tlocal = localtime(&miSistemaDeFicheros->nodosI[i]->tiempoModificado);
			char output[129];
			strftime(output, 100, "%d/%m/%y %H:%M:%S", tlocal);
			printf("%s\n", output);
			numArchivosEncontrados++;
		}
	}

	if (numArchivosEncontrados == 0) {
		printf("Directorio vacío\n");
	}
}

void myExit(MiSistemaDeFicheros* miSistemaDeFicheros) {
	int i;
    close(miSistemaDeFicheros->discoVirtual);
    for(i=0; i<MAX_NODOSI; i++) {
    	free(miSistemaDeFicheros->nodosI[i]);
    	miSistemaDeFicheros->nodosI[i] = NULL;
    }
    exit(1);
}
