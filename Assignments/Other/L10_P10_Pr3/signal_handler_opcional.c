#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "clients.h"
#include "server.h"



unsigned char _pause = 1; // Shared variable to pause/resume execution
/// Semaphores to pause the signal_handler thread when waiting for signals
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static sys_mbox_t* gbl_mbox=NULL;	//Initialized upon thread creation

// Flag to finish the "endless" signal waiting loop
unsigned char finish_thread = 0;

/* Signal handler descriptor */
pthread_t signal_handler;

/** Funciones para parar/continuar este hilo (gestor de señales)
 **/
void my_pause() {
	_pause = 1;
	pthread_mutex_lock(&lock);
    while(_pause==1) { /* We're paused */
        pthread_cond_wait(&cond, &lock); /* Wait for play signal */
    }
    pthread_mutex_unlock(&lock);
	
}

void my_resume() {
	pthread_mutex_lock(&lock);
	_pause=0;
	pthread_cond_signal(&cond); // cond to reevaluate pause
	pthread_mutex_unlock(&lock);
}

void finish_signal_handler(void){
	//Antes haciamos pthread_join() para esperar a la terminacion del hilo, pero al usar sigwait() no es necesario	
	finish_thread=1;
	my_resume();
}



/// PROPUESTA EJERCICIO: por cada USR1 -> crear un cliente nuevo
void USR1_handler(int signo) {
    
	pthread_t id;

	id = pthread_self();
	fprintf(logfile,"thread %u: caught signal %d (USR1)\n", (unsigned int) id, signo);
	
	if(create_client(gbl_mbox, default_client_type, nr_msgs, nr_secs)!=0)
 	{
		printf("Error al crear cliente");	
	}

	my_resume();

}


void SIGINT_handler(int signo) {
    
	pthread_t id;
	
    id = pthread_self();
    fprintf(logfile,"thread %u: caught signal %d (SIGINT)\n", (unsigned int) id, signo);
}


void* signal_handler_thread(void * c)
{
	sigset_t set;
	int sig;

	//Inicializamos el set
	sigfillset(&set);
	/*Mientras no terminemos (las pausas estan incluidas en este bucle igual que en la implementacion
	anterior, lo unico que ya no usamos my_pause() sino sigwait()) Dejo comentado donde iba antes my_pause()*/
	while(!finish_thread){
		if(sigwait(&set, &sig) == 0) {
			//Distinguimos entre dos tipos
			if(sig == SIGUSR1)
				USR1_handler(sig);			
			else if(sig == SIGINT)
				SIGINT_handler(sig);
		}
		//Mostramos los errores
		else {
			//Distinguimos entre los dos tipos de nuevo
			if(sig == SIGUSR1) {
				printf("ERROR: can't install SIGUSR1 signal handler");
				return NULL;
			}
			else if(sig == SIGINT) {
				printf("ERROR: can't install SIGINT signal handler");
				return NULL;
			}
		}
		//my_pause();
	}
	printf("Exiting signal_handler thread\n");
	return NULL;
}


int create_signal_handler(sys_mbox_t* mbox) {

	int r;	

	//Initialize global reference to mailbox
	gbl_mbox=mbox;

	if ( ( r= pthread_create(&signal_handler,NULL, signal_handler_thread,NULL ) ) == 0)
		printf("Signal handler thread id: %u \n",(unsigned int)signal_handler);
	return r;
}
