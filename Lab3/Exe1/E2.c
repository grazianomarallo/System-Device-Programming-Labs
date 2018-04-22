#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 16
int p;
int bu=0,bn=0;
// function to retrieve the current time stamp
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

/* Circular buffer of integers. */

struct prodcons
{
  int buffer[BUFFER_SIZE];	/* the actual data */
  pthread_mutex_t lock;		/* mutex ensuring exclusive access to buffer */
  int readpos, writepos, count;	/* positions for reading and writing */
  pthread_cond_t notempty;	/* signaled when buffer is not empty */
  pthread_cond_t notfull;	/* signaled when buffer is not full */
};

/* Initialize a buffer */
static void init_normal (struct prodcons *n)
{
  pthread_mutex_init (&n->lock, NULL);
  pthread_cond_init (&n->notempty, NULL);
  pthread_cond_init (&n->notfull, NULL);
  n->readpos = 0;
  n->writepos = 0;
  n->count = 0;
}

/* Initialize a buffer */
static void init_urgent (struct prodcons *u)
{
  pthread_mutex_init (&u->lock, NULL);
  pthread_cond_init (&u->notempty, NULL);
  pthread_cond_init (&u->notfull, NULL);
  u->readpos = 0;
  u->writepos = 0;
  u->count = 0;
}

/* Store an integer in the buffer */
static void
put_n (struct prodcons *n, int data){
  pthread_mutex_lock (&n->lock);
  /* Wait until buffer is not full */
  while (n->count == BUFFER_SIZE)
    {
      pthread_cond_wait (&n->notfull, &n->lock);
      /* pthread_cond_wait reacquired b->lock before returning */
    }
  /* Write the data and advance write pointer */
  n->buffer[n->writepos] = data;
  n->writepos++;
  if (n->writepos >= BUFFER_SIZE)
    n->writepos = 0;
  n->count++;
  /* Signal that the buffer is now not empty */
  pthread_cond_signal (&n->notempty);
  pthread_mutex_unlock (&n->lock);
}

/* Store an integer in the buffer */
static void
put_u (struct prodcons *u, int data){
  pthread_mutex_lock (&u->lock);
  /* Wait until buffer is not full */
  while (u->count == BUFFER_SIZE)
    {
      pthread_cond_wait (&u->notfull, &u->lock);
      /* pthread_cond_wait reacquired b->lock before returning */
    }
  /* Write the data and advance write pointer */
  u->buffer[u->writepos] = data;
  u->writepos++;
  if (u->writepos >= BUFFER_SIZE)
    u->writepos = 0;
  u->count++;
  /* Signal that the buffer is now not empty */
  pthread_cond_signal (&u->notempty);
  pthread_mutex_unlock (&u->lock);
}

/* Read and remove an integer from the buffer */
static int
get_n (struct prodcons *b)
{
  int data;
  pthread_mutex_lock (&b->lock);
  /* Wait until buffer is not empty */
  while (b->count == 0)
    {
      pthread_cond_wait (&b->notempty, &b->lock);
    }
  /* Read the data and advance read pointer */
  data = b->buffer[b->readpos];
  b->readpos++;
  if (b->readpos >= BUFFER_SIZE)
    b->readpos = 0;
  b->count--;
  /* Signal that the buffer is now not full */
  pthread_cond_signal (&b->notfull);
  pthread_mutex_unlock (&b->lock);
  return data;
}

static int
get_u (struct prodcons *b)
{
  int data;
  pthread_mutex_lock (&b->lock);
  /* Wait until buffer is not empty */
  while (b->count == 0)
    {
      pthread_cond_wait (&b->notempty, &b->lock);
    }
  /* Read the data and advance read pointer */
  data = b->buffer[b->readpos];
  b->readpos++;
  if (b->readpos >= BUFFER_SIZE)
    b->readpos = 0;
  b->count--;
  /* Signal that the buffer is now not full */
  pthread_cond_signal (&b->notfull);
  pthread_mutex_unlock (&b->lock);
  return data;
}


/* A test program: one thread inserts integers from 1 to 10000,
   the other reads them and prints them. */

#define OVER (-1)

struct prodcons normal,urgent;


static void *producer (void *data){
   int n,random;
  long ms;
  int t;
  for (n = 0; n < 100; n++) {
	 t= 1+(rand() % 10);
     usleep(t*1000);// Sleep randomly between 1 and 10 milliseconds
     ms = current_timestamp();	//get current time in milliseconds
	 random = rand() % p + (1-p);
	// printf("Random number is: %d\n",random);
     	if(random % 2 ==0){
  		  printf("Milliseconds: %ld - Selected buffer: normal - ", ms);
     	  put_n (&normal,0);
		  bn++;
          printf("Dim: %d\n", bn);
		}
		else{
		 printf("Milliseconds: %ld - Selected buffer: urgent - ", ms);
     	  put_n (&urgent,1);
		  bu++;
		  printf("Dim: %d\n", bu);
		}
 printf("n: %d\n",n);
    }
  return NULL;
}

static void *consumer (void *data){
  int d;
  int i;
 for(i=0;i<100;i++){
	usleep(10*1000); // Sleep for 10 milliseconds
      d = get_u(&normal);
	  if(bu >0){
	  	printf("->Consuming from urgent buffer - ");
		bu--;
		printf("DIM: %d\n", bu);
	  }
	 if(bu ==0 && bn >0) {
		d= get_n(&urgent);
		printf("->Consuming from nomal buffer - ");
		bn--;
		printf("DIM: %d\n", bn);
    	}
	 printf("i: %d\n",i);
    }
  return NULL;
}

int
main (int argc, char **argv){

 if (argc < 2){
    printf("usage %s: <probability>.\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  pthread_t th_a, th_b;
  void *retval;
  int rc;
  //Initilize the two buffer
  init_normal(&normal);
  init_urgent(&urgent);

  p = atoi(argv[1]);	//get probability from command line
  printf("probability-> %d\n",p);
  srand(time(NULL));

  
  /* Create the threads */
  rc = pthread_create (&th_a, NULL, producer, 0);
  if (rc) {
		fprintf(stderr,"ERROR! Generating the thread!");
		exit (-1);
	}
  rc = pthread_create (&th_b, NULL, consumer, 0);
  if (rc) {
		fprintf(stderr,"ERROR! Generating the thread!");
		exit (-1);
	}
  /* Wait until producer and consumer finish. */
  pthread_join (th_a, &retval);
  pthread_join (th_b, &retval);
  return 0;
}

