#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */

int main(void)
{
   pid_t  pid;
   pid_t soy, papa;
   
   /* El programa hace un fork, se crearán dos procesos independientes
    */
   pid = fork();
   if (pid == -1)
   {   
      /* Error:
       * Cuando fork() devuelve -1,
       * (Por ejemplo, hemos llegado al numero maximo de procesos).
       * La salida se envia por el estandar de eroor stderr
       */
      fprintf(stderr, "can't fork, error %d\n", errno);
      /* Indicamos que no se ha ejecutado con normalidad
      */
      exit(EXIT_FAILURE);
   }

   soy = getpid();

   papa = getppid();

   if (pid == 0)
   {
      /* Proceso hijo:
       * El fork() devuelve un 0, en proceso hijo
       * Como hemos hecho un fork pid son dos variables independientes 
       * Una para el hijo y otra para el padre
       */
      printf("\nComprueba que mi padre %d es un proceso asociado a este programa\n\n",papa);
      sleep(1);
      int j;
      for (j = 0; j < 4; j++)
      {
         printf("Soy hijo con PID %d y estoy en la iteracion %d\n", soy, j);
         sleep(1);	/*Duerme 1 segundo*/
      }
      _exit(0);  
   }
   else
   { 
      /* Proceso padre:
       * fork() devuelve un numero positivo al padre
       * le devuelve la identificación de su hijo
       */
      printf("\nComprueba que mi padre %d es una ventana de Bash\n\n", papa);
      sleep(1);
      int j;
      for (j = 0; j < 4; j++)
      {
         printf("Soy el padre con PID %d y estoy en la iteracion %d\n", soy, j);
         sleep(1);
      }
      exit(0);
   }
   return 0;
}
