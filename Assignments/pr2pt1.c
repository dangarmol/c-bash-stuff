#include <stdio.h>
#include <stdlib.h>

main(){ 
    FILE *archivoEntrada, *archivoSalida;
    char c, *nombreArchivoEntrada;
    int bytes, numeroArchivos;
	
	printf ("Número del archivos: ");
	scanf("%d", &numeroArchivos);

    archivoSalida = fopen("info.txt","w"); //Abrimos el archivo de salida con el nombre info.txt
    
	while(numeroArchivos > 0){
		bytes = 0;
		printf ("Nombre del archivo de entrada: "); //Pedimos nombre de archivo
		scanf ("%ms", &nombreArchivoEntrada); //Capturamos nombre de archivo y lo almacenamos en nombreArchivoEntrada con malloc: "%ms"
		archivoEntrada = fopen(nombreArchivoEntrada,"r"); //Abrimos el archivo de entrada con el nombre introducido
		
		if ((archivoEntrada == NULL) || (archivoSalida == NULL))
			printf ("Error al abrir ficheros");
		else{
			while ((c = getc(archivoEntrada)) != EOF){ //Copiamos el archivo de entrada al archivo de salida
				bytes++; //Contamos cada char copiado (Cada char ocupa un byte)
			}
			
			//Escribimos el nombre del archivo y los bytes que ocupa en el archivo de salida
			//No muestra el contenido, ya que no se ha capturado con un putc (c, archivoSalida)
			fprintf (archivoSalida, "%s %s %d \n", nombreArchivoEntrada, "\nBytes: ", bytes);
			
			free(nombreArchivoEntrada); //Liberamos nombreArchivoEntrada de malloc
			
			if (fclose(archivoEntrada)!= 0)
				printf ("%s %s \n", "Error al cerrar el fichero", nombreArchivoEntrada);
		}
		numeroArchivos--;
	}
	if (fclose(archivoSalida)!= 0)
		printf ("Error al cerrar el fichero info.txt");
}