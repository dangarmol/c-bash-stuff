#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int suma_total = 0;
void * suma_parcial(void * arg) {
  int j = 0;
  int ni=((int*)arg)[0];
  int nf=((int*)arg)[1];

  for (j = ni; j <= nf; j++)
    suma_total = suma_total + j;

  pthread_exit(0);
}

int main(int argc, char** argv) {
  if(argc != 3){
	printf("Uso: %s num_threads max.\n", argv[0]);
	return -1;  
  }

  int num_threads = atoi(argv[1]);
  int max = atoi(argv[2]);

  if(num_threads == 0){
	printf("num_threads debe ser mayor que 0.\n");
  	return -1;
  }

  pthread_t* th = (pthread_t*)malloc(num_threads);
  
  int inc = max/num_threads;
  int i;

  int* num = (int*)malloc(num_threads*2);
  for(i = 0; i < num_threads*2; i+=2){
	num[i] = (i*inc);
	num[i+1] = ((i+1)*inc)-1;
  }

  /* se crean dos threads con atributos por defecto */
  
  for(i = 0; i < num_threads; i++){
	int aux[2];
	aux[0] = num[2*i];
	aux[1] = num[2*i+1];
	pthread_create(&th[i], NULL, suma_parcial, (void*)aux);
  }

  printf("El thread principal continua ejecutando\n");

  /* se espera su terminacion */

  for(i = 0; i < num_threads; i++){
	pthread_join(th[i], NULL);
  }

  printf("La suma total es: %d y debería ser (50005000)\n", suma_total);

  free(th);
  free(num);

  return 0;
}