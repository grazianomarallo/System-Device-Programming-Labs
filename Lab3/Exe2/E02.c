#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pthread.h"
#include "semaphore.h"
#include <time.h>
#include <sys/time.h>
sem_t *me, *me1, *w;
int nr = 0;

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

static void *writer (void *a){
  int *i = (int *) a;
  int t;
  long time;
  t= 0+(rand() % 500);
  usleep(t*1000);
  time = current_timestamp();
  printf("Writer %d trying to write at time: %ld\n", *i,time);
  sem_wait (me1);
  sem_wait (w);
  time = current_timestamp();
  printf ("->Thread n. %d writing at time: %ld\n", *i,time);
  usleep (500*1000); //simulate writing sleeping for 500 ms
  sem_post (w);
  sem_post (me1);

  return NULL;
}


static void *reader (void *a){
  int *i = (int *) a;
  int t;
  long time;
  t=0+(rand() % 500);
  usleep(t*1000);
  time = current_timestamp();
  printf("Reader thread %d trying to read at time: %ld\n", *i,time);
  sem_wait (me);
  nr++;
  if (nr == 1)
    sem_wait (w);
  sem_post (me);
  time = current_timestamp();
  printf ("->Thread n. %d reading at time: %ld\n", *i,time);
  usleep (500*1000); //simulate reading sleeping for 500 ms
 
  sem_wait (me);
  nr--;
  if (nr == 0)
    sem_post (w);
  sem_post (me);

  return NULL;
}

int
main (int argc, char **argv){
  pthread_t th_a, th_b;
  int i, *v, N;

  if (argc < 2){
    printf("usage %s: <N>.\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  N = atoi(argv[1]);	//get number of reader and writer
  printf("N-> %d\n",N);

  w = (sem_t *) malloc (sizeof (sem_t));
  me = (sem_t *) malloc (sizeof (sem_t));
  me1 = (sem_t *) malloc (sizeof (sem_t));
  sem_init (w, 0, 1);	//semaphore to ensure priority to readers
  sem_init (me, 0, 1); //critical section for readers
  sem_init (me1, 0, 1); //critical section for writers

  setbuf(stdout,0);

  /* Create N threads for readers and writer */

  for (i = 0; i < N; i++) {
    v = (int *) malloc (sizeof (int));
    *v = i;
    pthread_create (&th_a, NULL, reader, v);
  }

  for (i = 0; i < N; i++) {
    v = (int *) malloc (sizeof (int));
    *v = i;
    pthread_create (&th_b, NULL, writer, v);
  }
	
  pthread_join (th_a, NULL);
  pthread_join (th_b, NULL);
  return 0;

}

