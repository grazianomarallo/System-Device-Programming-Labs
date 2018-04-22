#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <time.h>
#define NUM_THREADS 2

void *th1();
void *th2();
sem_t *b1,*b2;
int n;

//define struct and variable associated
typedef struct{
	int count;	// shared counter to keep track of the print routine
	pthread_mutex_t mutex;	// mutex to allow the mutual exclusione while updating counter
}Counter;

Counter *c;


void *th1(){
		int i;
	for(i=0;i<n ;i++){
		printf("A1");
		pthread_mutex_lock(&c->mutex);		//acquire lock
		c->count++;			//increment counter and check if two As have been printed out
		if(c->count%2 ==0){	//if so reset counter and unlock two barrier for B
			c->count==0;
			sem_post(b1);
			sem_post(b1);
		}
		pthread_mutex_unlock(&c->mutex);	// realease lock and wait on barrier1
		sem_wait(b1);			
		printf("B1");			// same strategy as before
		pthread_mutex_lock(&c->mutex);
		c->count++;
		
		if(c->count %2 ==0){
			c->count==0;
			printf("\n");
			sem_post(b2);
			sem_post(b2);
			
			
		}
		pthread_mutex_unlock(&c->mutex);			
		sem_wait(b2);		// wait on barrier two before loop again
		
	}

	

	pthread_exit(NULL);
}

void *th2(){
		int i;
		int v;
	for(i=0;i<n ;i++){
		printf("A2");
		pthread_mutex_lock(&c->mutex);
		c->count++;
		if(c->count%2 ==0){
			c->count==0;
			sem_post(b1);
			sem_post(b1);
			
		}
		pthread_mutex_unlock(&c->mutex);
		
		sem_wait(b1);
		printf("B2");
		pthread_mutex_lock(&c->mutex);
		c->count++;
		
		if(c->count%2 ==0){	
			c->count==0;
			printf("\n");			
			sem_post(b2);
			sem_post(b2);
		}
		pthread_mutex_unlock(&c->mutex);
		sem_wait(b2);
	}
	

	pthread_exit(NULL);
}



int main (int argc, char **argv){
    long int rc,rc1;
	long int i,j;
	pthread_t threads[NUM_THREADS]; 
	
	if(argc < 2){
	printf("Usage of %d: <N>\n", argc);
	exit(EXIT_FAILURE);	
	}
	
    //read the number of n from input
    n = atoi(argv[1]);

  

	// allocate memory for semaphore
	b1 = (sem_t*) malloc(sizeof(sem_t));
	b2 = (sem_t*) malloc(sizeof(sem_t));
	sem_init(b1,0,0);
	sem_init(b2,0,0);
	c = (Counter *) malloc(sizeof(Counter));
	c->count=0;
	pthread_mutex_init(&c->mutex,NULL);
    //create the threads

    rc = pthread_create (&threads[0], NULL, th1, NULL);
    if (rc) {
        fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);

    	}
	
	
   	rc1=pthread_create (&threads[1], NULL, th2, NULL);
    	if (rc1) {
        fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);
    	}
	
	//awaits for threads completion
	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);
	//destroy semaphores and exit from main thread after completion of job
	sem_destroy(b1);
	sem_destroy(b2);
	
    pthread_exit (NULL);
}







