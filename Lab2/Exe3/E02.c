#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#define NUM_THREADS 2



int g;                 //global variable
static sem_t*srv;      //semaphore used for synchronization
static sem_t *clt;
static sem_t *proc;
int counter=0;         //counter used to end the loop
int servedrequest;

//Create structure with thread id and file pointer
struct tS {
    int tid;
    char *file;
};

//function to handle threads behaviour
void *thfunc (void *threadid) {
    int tid;
    int fdo; 
    int buffer;
    char *filename;
    struct tS *tD;
    tD = (struct tS *) threadid;
    tid = tD->tid;
    filename = tD->file;

   
    if ((fdo = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "error open %s\n", filename);
    }
    while (read(fdo, &buffer, sizeof(int))) {
        sem_wait(clt);
        g=buffer;
        printf("Thread %d -> %d\n", tid, g);
        sem_post(srv);
        sem_wait(proc);
        printf("Thread %d -> processed %d\n",tid,g);
        sem_post(clt);

    }

    counter++;
    if (counter ==2)
        sem_post(srv);

    pthread_exit(NULL);
}

//main function where semaphore are initialized and server behaviour is managed
int main (int argc, char *argv[]){
    pthread_t threads[NUM_THREADS];
    struct tS v[NUM_THREADS];
    long int rc, t;

    v[0].file="fv1.b";
    v[1].file="fv2.b";

    srv = (sem_t*) malloc(sizeof(sem_t));
    clt = (sem_t*) malloc(sizeof(sem_t));
    proc = (sem_t*) malloc(sizeof(sem_t));

    sem_init(srv, 0, 0);
    sem_init(clt, 0, 1);
    sem_init(proc, 0, 0);

//for cycle to create thread
    for (t=0; t < NUM_THREADS; t++) {
        fprintf (stdout, "In main: creating thread %ld\n", t);
        rc = pthread_create (&threads[t], NULL, thfunc, (void *) &v[t]);
        v[t].tid =t;
        if (rc) {
            fprintf (stderr, "ERROR: pthread_create() code %ld\n", rc);
            exit(-1);
        }
    }

    while(counter <2){
        sem_wait(srv);
        g *=3;
        sem_post(proc);
        if(counter <2)
            servedrequest++;

    }


    printf("Totale served request: %d\n",servedrequest);
    sem_destroy(srv);
    sem_destroy(clt);
    sem_destroy(proc);

    pthread_exit (NULL);
}
