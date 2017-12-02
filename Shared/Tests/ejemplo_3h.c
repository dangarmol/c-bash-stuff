#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */

int main(void)
{
   pid_t  pid;
   pid_t soy, papa, padre;
   int i=0;

   padre=getpid(); /* lee el PID del proceso padre */
   printf(" Soy%d\n", padre);
   printf(" y voy a ser padre en 10 segundos, así que prepara el ps -ef para ver a mis hijos\n");
   sleep(10);
   
   /* El programa hace un fork, se crearán dos procesos independientes
    */
   
   for(i=0; i<3; i++)
   	pid = fork();
   
   soy = getpid();
	  
 
   if (pid == -1)
   {   
      /* Error:
       * Cuando fork() devuelve -1,
       * (Por ejemplo, hemos llegado al numero maximo de procesos).
       * La salida se envia por el estandar de error stderr
       */
      fprintf(stderr, "can't fork, error %d\n", errno);
      /* Indicamos que no se ha ejecutado con normalidad
      */
      exit(EXIT_FAILURE);
   }

    papa = getppid();

   if (pid == 0)
   {
      /* Proceso hijo:
       * El fork() devuelve un 0, en proceso hijo
       * Como hemos hecho un fork pid son dos variables independientes 
       * Una para el hijo y otra para el padre
       */
      printf("\nSoy %d y acabo de nacer",soy);
      printf("\nComprueba que mi padre %d es mi padre\n\n",papa);
      sleep(1);
      int j;
      for (j = 0; j < 4; j++)
      {
         printf("Soy %d y estoy en la iteracion %d\n", soy, j);
         sleep(5);	/*Duerme 1 segundo*/
      }
      _exit(0);  
   }
   else
   { 
      /* Proceso padre:
       * fork() devuelve un numero positivo al padre
       * le devuelve la identificación de su hijo
       */
      printf("\nSoy  %d",soy);
      printf("\nComprueba que mi padre %d es mi padre\n\n", papa);
      sleep(1);
      int j;
      for (j = 0; j < 4; j++)
      {
         printf("Soy %d y estoy en la iteracion %d\n", soy, j);
         sleep(5);
      }
      exit(0);
   }
   return 0;
}
