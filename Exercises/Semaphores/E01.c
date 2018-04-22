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

void alarm_handler(int);
int wait_with_timeout(sem_t* ,int);
void *thfunc(void *);
void *thfunc1(void *);
static sem_t *s;
int tmax;

typedef struct{
	int tid;
	int tm;
}tS;


void *thfunc(void *threadid){
	int tid;
	int tm;
	int t;
	tS *tH;
	tH = (tS *) threadid;
	tid = tH->tid;
	tm = tH->tm;
	srand((unsigned)pthread_self());

	//a.
	t= 1+(rand()%4);
	usleep(1000*t);
	//b.
	printf("waiting on semaphore after %d milliseconds",t);
	//c. & d.
	if(wait_with_timeout(s,tm))
		printf("Wait on semaphore s returned for timeout\n");
	else
	printf("Wait returned normally\n");
	//e.
	pthread_exit(NULL);
}

void *thfunc1(void *threadid){
	int t;
	int tid;

	tS *tH;
	tH = (tS *) threadid;
	tid = tH->tid;
	srand((unsigned)pthread_self());

	//a.
	t= 100+(rand()%900);
	usleep(1000*t);
	//b.
	printf("Performing signal on semaphore s after %d milliseconds\n", t);
    sem_post(s);

   	//c.
    pthread_exit(NULL);
}



int main (int argc, char **argv){
    pthread_t threads[NUM_THREADS];
    tS v[NUM_THREADS];
    long int rc,rc1, t;

	if(argc < 2){
	printf("Usage of %d: <N>\n", argc);
	exit(EXIT_FAILURE);	
	}

    //read the number of tmax from input
    tmax = atoi(argv[1]);
    //store the value of tmax
    v[0].tm=tmax;

    signal(SIGALRM,alarm_handler);
    //allocate and initialize semaphore
    s = (sem_t*) malloc(sizeof(sem_t));
    sem_init(s, 0, 0); //init semaphore to 0 and shared beetween thread (2nd param is 0)

    //create the threads
    rc = pthread_create (&threads[0], NULL, thfunc, (void *) &v[0]);
    v[0].tid =0;
    if (rc) {
        fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);

    }
    rc1=pthread_create (&threads[1], NULL, thfunc1, (void *) &v[1]);
    v[1].tid=1;
    if (rc1) {
        fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);

    }

    /* Wait for threads completition */
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    //destroy the semaphore
    sem_destroy(s);
	//exit from main thread after completion of job
    pthread_exit (NULL);
}

void alarm_handler(int sig){
	sem_post(s);
	return;
}


int wait_with_timeout(sem_t* S,int tmx){
    ualarm(tmx*1000,10000);
    sem_wait(S);
    if(alarm(0)>0)
        return 0;
    else
    return 1;
}




