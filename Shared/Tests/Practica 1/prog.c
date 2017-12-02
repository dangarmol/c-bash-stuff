#include <stdio.h>

main()
{

FILE *archivo1, *archivo2;
char c;

archivo1=fopen("prueba.txt","r");
archivo2=fopen("copia.txt","w");

if ((archivo1==NULL)||(archivo2==NULL))

printf("Error al abrir ficheros");

else

{

	while((c=getc(archivo1))!=EOF)
	{

		putc(c,archivo2);  
	}


if (fclose(archivo1)!=0)

printf("Error al cerrar fichero prueba");

if (fclose(archivo2)!=0)

printf("Error al cerrar fichero copia");

} 
exit(0);

}
