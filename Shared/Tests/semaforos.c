#define MAX_BUF             1024    /*tamanio del buffer */
#define PROD              100000    /*datos a producir */
sem_t elementos;                /*elementos en el buffer */
sem_t huecos;                      /* huecos en el buffer */
int buffer[MAX_BUF];                /* buffer comun */
Int cons,prod = 0;          /*posicion dentro del buffer*/

void main(void){
   pthread_t th1, th2;  /* identificadores de threads */
   /* inicializar los semaforos */
   sem_init(&elementos, 0, 0);   sem_init(&huecos, 0, MAX_BUFFER);

   /*crear los procesos ligeros */
   pthread_create(&th1, NULL, Productor, NULL);
   pthread_create(&th2, NULL, Consumidor, NULL);  
   /*esperar su finalizacion */
   pthread_join(th1, NULL);   
   pthread_join(th2, NULL);

   sem_destroy(&huecos);   
   sem_destroy(&elementos);
   exit(0);
}

void *Productor(void)
{
/*dato a producir*/
   int dato;               
   int i;

   for(i=0; i < PROD; i++ ){
      /*producir dato*/
      dato = producir_dato();
      /*un hueco menos*/
      sem_wait(&huecos);
      buffer[prod] = dato;
      prod = (prod + 1) % MAX_BUF;
      /*un elemento mas*/
      sem_post(&elementos);
  }
  pthread_exit(0);
}
 
void Consumidor(void)
{
/*dato a consumir*/
   int dato;
   int i;

   for(i=0; i<PROD; i++ ){
      /*un elemento menos*/
      sem_wait(&elementos);          
      dato = buffer[con];
      cons= (cons+ 1) % MAX_BUF;
      /*un hueco mas*/
      sem_post(&huecos);
      cosumir_dato(dato);
   }
   pthread_exit(0);
}

