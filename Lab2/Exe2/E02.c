#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>

int arr,n;
pthread_mutex_t sem;

struct tS {
pthread_t *vector;
int N;
};


void *treeThread (void *parameters) {
	int j, rc;
	pthread_t tid[2];
	struct tS *par;
	par = (struct tS*) parameters; 
	
	struct tS parS;
	int actualLevel = n-par->N; 
	pthread_t* newVector = malloc((n+1)*sizeof(pthread_t));
	for (j=0; j<actualLevel; j++) {
		newVector[j] = par->vector[j];
	}
	newVector[actualLevel] = pthread_self();
	parS.vector = newVector;
	
	if (par->N == 0) {
		pthread_mutex_lock(&sem);
		fprintf(stdout,"Thread tree:");
		for (j=0; j<n+1; j++) 
			fprintf(stdout," %ld",(long int)parS.vector[j]);
		fprintf(stdout,"\n");
		pthread_mutex_unlock(&sem);
		pthread_exit(NULL); 
	}
	
	parS.N = par->N-1;
	// generating the two children threads
	for (j=0; j<2; j++) {
		rc = pthread_create (&tid[j], NULL, treeThread, (void*)&parS);
		
		if (rc) {
			fprintf(stderr,"ERROR! Generating the thread!");
			exit (-1);
		}
	}
	
	// wait for the children
	for (j=0; j<2; j++)
		pthread_join(tid[j],NULL);
	pthread_exit(NULL);
}

int main (int argc, char **argv) {
  if(argc < 2){
        printf ("usage: %s <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	n = atoi(argv[1]);
	arr = n+1;
	setbuf(stdout,NULL);
	pthread_mutex_init(&sem,NULL); // this is the mutual exclusion semaphore, to avoid overlaps during the final printing phase
	pthread_t *vet = malloc(arr*sizeof(pthread_t));
	struct tS params = {vet,n};
	pthread_t tid;
	int rc;
	rc = pthread_create (&tid, NULL, treeThread, (void*)&params);
	if (rc) {
		fprintf(stderr,"ERROR! Generating the thread!");
		exit (-1);
	}
	pthread_join(tid,NULL);
	pthread_exit(NULL);
}

