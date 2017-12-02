#define MAX_BUF	3
#define PROD	1000
sem_t elementos;
sem_t huecos;
int buffer[MAX_BUF];
int cons, prod = 0;

void main(void){
	pthread_t th1, th2, th3;
	sem_init(&elementos, 0, 0); 
	sem_init(&huecos, 0, MAX_buffer);
	sem_init(&elementos, 0, 0); 
}

