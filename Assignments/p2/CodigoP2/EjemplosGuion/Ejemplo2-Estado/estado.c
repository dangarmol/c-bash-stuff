#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

char permisos[] = {'x', 'w', 'r'};

int estado(char *);

int main(int argc, char *argv[]) {
	int i;

	// Comprobaci�n de que el n�mero de argumentos de la l�nea de orden es correcto
	if (argc < 2)
		fprintf(stderr, "Uso: %s ficheros...\n", argv[0]), exit(-1);

	// Para cada fichero mostrar su estado
	for (i=1; i<argc; i++)
		estado(argv[i]);

	exit(0);
}

int estado(char *fichero) {
	struct stat buf;
	struct passwd *pw;
	struct group *gr;
	int i;

	// Rellena en buf la estructura stat con los atributos del fichero
	if (stat(fichero, &buf) == -1)
		perror(fichero), exit(-1);
	printf("Fichero: %s\n", fichero);

	// Separa los n�mero 'major' y 'minor' que identifican al dispositivo
	//	en el que est� alojado este inodo; es decir, el volumen
	//	que contiene el sistema de ficheros al que pertenece el fichero
	printf("Reside en el dispositivo: %d, %d\n",
		(int)(buf.st_dev & 0xff00) >> 8, (int)buf.st_dev & 0x00ff);

	// Muestra el n�mero de inodo; junto con el valor anterior
	//	permite identificar totalmente al fichero
	printf(" Nro. de i-nodo: %ld\n", buf.st_ino);

	// Procesa el campo 'mode' que informa sobre
	//	el tipo de fichero
	//	los permisos que le son aplicables
	//	y alg�n otro modo de operaci�n adicional
	//	(S_ISxxx son m�scaras de bits)
	printf(" Tipo: ");
	switch(buf.st_mode & S_IFMT) {
        case S_IFREG:
            printf("regular.\n"); break;
        case S_IFDIR:
            printf("directorio.\n"); break;
        case S_IFCHR:
            printf("especial modo caracter.\n"); break;
        case S_IFBLK:
            printf("especial modo bloque.\n"); break;
        case S_IFIFO:
            printf("FIFO.\n"); break;
        case S_IFLNK:
            printf("Enlace simbolico.\n"); break;
        default:
            printf("DESCONOCIDO.\n"); break;
	}
	if (buf.st_mode & S_ISUID)
		printf(" Cambia el ID del propietario en ejecucion.\n");
	if (buf.st_mode & S_ISGID)
		printf(" Cambia el ID del grupo en ejecucion.\n");
	if (buf.st_mode & S_ISVTX)
		printf(" Bit sticky activo.\n");

	// Muestra los permisos en octal
	printf(" Permisos: 0%o ", buf.st_mode & 0777);

	// Muestra los permisos de la forma 'rwxrwxrwx'
	for (i=0; i<9; i++)
		if (buf.st_mode & (0400 >> i))
			printf("%c", permisos[(8-i)%3]);
		else
			printf("-");

	// Muestra el n�mero de enlaces r�gidos de este inodo
	printf("\n");
	printf(" Enlaces: %ld\n", (long int)buf.st_nlink);

	// Muestra las identificaciones de usuario y de grupo,
	// n�merica y simb�licamente
	//	La funci�n getpwuid() consulta el fichero /etc/passwd
	//	y la funci�n getgrgid() consulta el fichero /etc/group
	// 	donde se guardan tales relaciones entre identificaciones
	printf(" ID de usuario: %d; Nombre: ", buf.st_uid);
	if ((pw = getpwuid(buf.st_uid)) == NULL)
		printf("???\n");
	else
		printf("%s\n", pw->pw_name);
	printf(" ID de grupo: %d; Nombre: ", buf.st_gid);
	if ((gr = getgrgid(buf.st_gid)) == NULL)
		printf("???\n");
	else
		printf("%s\n", gr->gr_name);

	// Si el fichero es de tipo especial muestra los n�emros
	//	'major' y 'minor' que le corresponden
	switch (buf.st_mode & S_IFMT) {
        case S_IFCHR:
        case S_IFBLK:
            printf(" NUmeros de dispositivo: %d, %d\n",
                (int)(buf.st_rdev & 0xff00) >> 8, (int)buf.st_rdev & 0x00ff);
	}

	// Muestra el tama�o del fichero
	printf(" Tama�o: %ld bytes.\n", buf.st_size);

	// Muestra las fechas asociadas al fichero en formato cadena de caracteres.
	//	stat() poroporciona los tiempos como n�meros de tipo time_t
	//	La funci�n localtime() transforma un tiempo de tipo time_t
	//	 en una struct tm
	//	La funci�n asctime() transforma un tipo de tipo struct tm
	//	 en cadena de caracteres
	printf(" Ultimo acceso: %s", asctime(localtime(&buf.st_atime)));
	printf(" Ultima modificacion: %s", asctime(localtime(&buf.st_mtime)));
	printf(" Ultimo cambio: %s", asctime(localtime(&buf.st_ctime)));

	return 0;
}
