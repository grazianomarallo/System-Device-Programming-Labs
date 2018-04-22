#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <time.h>
#include <signal.h>
#define NUM_THREADS 2

void *thA(void *);
void *thB(void *);
static sem_t *b1,*b2;
int n;

//define struct and variable associated
typedef struct{
	int counta, countb;
	pthread_mutex_t mutex;
}Counter;

Counter *c;


void *thA(void *threadid){
	long int tid;
	tid = (long int ) threadid;
	//wait on the barrier one that is set to 1 to allow just on A to be in the 
	//critical region
	sem_wait(b1);
	pthread_mutex_lock(&c->mutex);
	c->counta++;
	printf("A%ld ",tid);
	if((c->counta + c->countb) % 3 == 0){
		printf("\n");	//if 3 has been reached can print the new line
		c->counta=c->countb=0;	//and reset the two counters
	}
	// in both cases now we allow the two Bs to enter 
	//so post the barrier2 and unlock mutex before terminat
	sem_post(b2);
	sem_post(b2);
	pthread_mutex_unlock(&c->mutex);
	
	pthread_exit(NULL);
}

void *thB(void *threadid){
	long int tid;
	tid= (long int) threadid;
	//wait on the barrier 2 that is set to 2 to allow 2 Bs to be in the 
	//critical region	pthread_mutex_lock(&c->mutex);
	sem_wait(b2);
	pthread_mutex_lock(&c->mutex);
	c->countb++;
	printf("B%ld ",tid);
	//if 3 has been reached can print the new line
	if((c->counta + c->countb) % 3 == 0){
		printf("\n");	//print new line if 3 is reached
		c->counta=c->countb=0;//reset the two counters
		}
	//then check if A is still missing, if so unlock barrier 1, then release lock and exit
	if(c->countb %2 ==0)
		sem_post(b1);
	pthread_mutex_unlock(&c->mutex);

	
    pthread_exit(NULL);
}



int main (int argc, char **argv){
    long int rc,rc1;
	long int i,j;
	
	if(argc < 2){
	printf("Usage of %d: <N>\n", argc);
	exit(EXIT_FAILURE);	
	}

    //read the number of n from input
    n = atoi(argv[1]);
    //allocate memory for the threads. This number is basically n threads for A and 2*n for B
	// so at the end we need 3 * n threads
   pthread_t *threads= (pthread_t *) malloc(3*n*sizeof(pthread_t));

	// allocate memory for semaphore
	b1 = (sem_t*) malloc(sizeof(sem_t));
	b2 = (sem_t*) malloc(sizeof(sem_t));
	sem_init(b1,0,1);//barrier for A
	sem_init(b2,0,2);//barrier for B
	c = (Counter *) malloc(sizeof(Counter));
	c->counta=0;
	c->countb=0;
	pthread_mutex_init(&c->mutex,NULL);
    //create the threads
	for(i=0; i<n;i++){
    rc = pthread_create (&threads[i], NULL, thA, (void *) i);
    if (rc) {
        fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);

    	}
	}

	for(j=0; j<(n*2); j++){
   	rc1=pthread_create (&threads[j], NULL, thB, (void *) j);
    	if (rc1) {
        fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);

    	}
	}

	//exit from main thread after completion of job
    pthread_exit (NULL);
}







