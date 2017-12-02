#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_OF_PHILOSOPHERS 5

pthread_t philosophers[NO_OF_PHILOSOPHERS];
pthread_mutex_t tenedores[NO_OF_PHILOSOPHERS];

/*
static inline void double_lock_rq(runqueue_t* this_rq,runqueue_t* remote_rq){
 
/* If it's a smaller CPU ID, simply adquire the lock
 * Otherwise, unlock this_rq first and lock both RQs following
 * the locking protocol
 */
/*
 if (this_rq > remote_rq) {
    lock_rq(remote_rq);	
 }else{
    unlock_rq(this_rq);
    lock_rq(remote_rq);
    lock_rq(this_rq);    
 }
}

*/

void init()
{
    int i;
    for(i=0; i<NO_OF_PHILOSOPHERS; i++)
        pthread_mutex_init(&tenedores[i],NULL);

    
}

void think(int i) {
    printf("Filósofo %d pensando... \n" , i);
        sleep(random() % 10);
    printf("Filósofo %d teminó de pensar!!! \n" , i);

}

void eat(int i) {
    printf("Filósofo %d comiendo... \n" , i);
     sleep(random() % 5);
    printf("Filósofo %d teminó de comer!!! \n" , i);

}

void toSleep(int i) {
    printf("Filósofo %d durmiendo... \n" , i);
    sleep(random() % 10);
    printf("Filósofo %d teminó de dormir!!! \n" , i);
    
}

void* philosopher(void* i)
{
    int nFilosofo = (int)i;
    int derecha = nFilosofo;
    int izquierda = (nFilosofo - 1 == -1) ? NO_OF_PHILOSOPHERS - 1 : (nFilosofo - 1);
    while(1)
    {
        
        think(nFilosofo);
        
        /// INTENTAR COGER AMBOS TENEDORES (derecha e izquierda)
		//this		//remote
	    if(philosophers[i] > philosophers[i+1]) {
			pthread_mutex_lock(&tenedores[i+1]); 	//bloqueo el remote
	    }
	    else {
			pthread_mutex_unlock(&tenedores[i]); 	//desbloqueo el this
			pthread_mutex_lock(&tenedores[i+1]); 	//bloqueo el remote
			pthread_mutex_lock(&tenedores[i]);	//bloqueo el this
	    }      
		//
 
        eat(nFilosofo);
                
		// DEJAR AMBOS TENEDORES
		pthread_mutex_unlock(&tenedores[i]);
		pthread_mutex_unlock(&tenedores[i+1]); 
		//       

        toSleep(nFilosofo);
   }

}

int main()
{
    init();
    unsigned long i;

    for(i=0; i<NO_OF_PHILOSOPHERS; i++)
        pthread_create( &philosophers[i], NULL, philosopher, (void*)i);

    for(i=0; i<NO_OF_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);

    for(i=0; i<NO_OF_PHILOSOPHERS; i++)
        pthread_mutex_destroy(&tenedores[i]);

    return 0;
} 
