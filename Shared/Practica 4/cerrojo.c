#define MAX_BUFFER         1024      /* tamanio del buffer */
#define DATOS_A_PRODUCIR   100000    /* datos a producir */
pthread_mutex_t mutex;     /*mutex para buffer compartido*/
pthread_cond_t lleno;      /*controla el llenado del buffer*/
pthread_cond_t vacio;      /*controla el vaciado del buffer*/
int n_elementos;           /*numero de elementos en el buffer*/
int buffer[MAX_BUFFER];    /*buffer comun*/

main(int argc, char *argv[]){
    pthread_t th1, th2;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&lleno, NULL);
    pthread_cond_init(&vacio, NULL);
    pthread_create(&th1, NULL, Productor, NULL);
    pthread_create(&th2, NULL, Consumidor, NULL);
    pthread_join(th1, NULL);    
    pthread_join(th2, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&lleno);
    pthread_cond_destroy(&vacio);
    exit(0);
}
void *Productor(void)  
{  
   /* codigo del productor */
    int dato, i ,pos = 0;

    for(i=0; i < DATOS_A_PRODUCIR; i++ )  {
        dato = producir_dato();           /*producir dato*/
        pthread_mutex_lock(&mutex);       /*acceder al buffer*/
        while (n_elementos == MAX_BUFFER) /*si buffer lleno*/
            pthread_cond_wait(&lleno, &mutex); /*se bloquea*/
        /*Compartir dato*/    
        buffer[pos] = dato;
        pos = (pos + 1) % MAX_BUFFER;
        n_elementos ++;
        pthread_cond_signal(&vacio);   /*buffer no vacio*/
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void *Consumidor(void) {   /* codigo del consumidor */
    int dato, i ,pos = 0;

    for(i=0; i < DATOS_A_PRODUCIR; i++ ) {
        pthread_mutex_lock(&mutex);    /* acceder al buffer */
        while (n_elementos == 0)       /* si buffer vacio */
            pthread_cond_wait(&vacio, &mutex); /* se bloquea */
        /*Retirar dato del buffer*/
        dato = buffer[pos];
        pos = (pos + 1) % MAX_BUFFER;
        n_elementos --;
        pthread_cond_signal(&lleno);   /* buffer no lleno */
        pthread_mutex_unlock(&mutex);
        printf("Consume %d \n", dato);     /* consume dato */
    }
    pthread_exit(0);
}
