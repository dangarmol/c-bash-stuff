#include <stdio.h>
#include <stdlib.h>

main(){ 
    FILE *archivoEntrada, *archivoSalida;
    char c, *nombreArchivoEntrada;
    int numeroArchivos;
	
	printf ("Número del archivos: ");
	scanf("%d", &numeroArchivos);

    archivoSalida = fopen("copia.txt","w"); //Abrimos el archivo de salida con el nombre info.txt
    
	while(numeroArchivos > 0){
		printf ("Nombre del archivo de entrada: "); //Pedimos nombre de archivo
		scanf ("%ms", &nombreArchivoEntrada); //Capturamos nombre de archivo y lo almacenamos en nombreArchivoEntrada con malloc: "%ms"
		archivoEntrada = fopen(nombreArchivoEntrada,"r"); //Abrimos el archivo de entrada con el nombre introducido
		
		if ((archivoEntrada == NULL) || (archivoSalida == NULL))
			printf ("Error al abrir ficheros");
		else{
			while ((c = getc(archivoEntrada)) != EOF){ //Copiamos el archivo de entrada al archivo de salida
				putc (c, archivoSalida); //Capturamos el contenido de archivoSalida
			}
			
			//Escribimos en el archivo de salida la concatenacion de los archivos de entrada
			fprintf (archivoSalida, "\n");
			
			free(nombreArchivoEntrada); //Liberamos nombreArchivoEntrada de malloc
			
			if (fclose(archivoEntrada)!= 0)
				printf ("%s %s \n", "Error al cerrar el fichero", nombreArchivoEntrada);
		}
		numeroArchivos--;
	}
	if (fclose(archivoSalida)!= 0)
		printf ("Error al cerrar el fichero copia.txt");
}