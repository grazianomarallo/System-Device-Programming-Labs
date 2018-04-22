#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#define BUFFER_SIZE 16
#define OVER (-1)

int bu=0,bn=0;
typedef struct Buffer{
  sem_t *empty, *full, *me_p, *me_c;
  int in, out;
  int *buffer;
  int size;
}Buffer;

// function to retrieve the current time stamp
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

Buffer *buffer_init_n(int size);
Buffer *buffer_init_u(int size);
Buffer *n,*u;
int p;


Buffer *buffer_init_n(int size){
  n = (Buffer *) malloc(sizeof(Buffer));
  n->size = size;
  n->buffer = (int *) malloc(n->size * sizeof(int));
  n->in = 0;
  n->out = 0;
  n->empty = (sem_t *) malloc(sizeof(sem_t)); sem_init (n->empty, 0, n->size);
  n->full = (sem_t *) malloc(sizeof(sem_t));  sem_init (n->full, 0, 0);

  return n;
}
Buffer *buffer_init_u(int size){
  u = (Buffer *) malloc(sizeof(Buffer));
  u->size = size;
  u->buffer = (int *) malloc(u->size * sizeof(int));
  u->in = 0;
  u->out = 0;
  u->empty = (sem_t *) malloc(sizeof(sem_t)); sem_init (u->empty, 0, u->size);
  u->full = (sem_t *) malloc(sizeof(sem_t));  sem_init (u->full, 0, 0);

  return n;
}

/* Store an integer in the buffer */
void put_n (int data){
  sem_wait(n->empty);
  n->buffer[n->in] = data;
  n->in = (n->in + 1) % n->size;
  sem_post(n->full);
}

/* Store an integer in the buffer */
void put_u (int data){
  sem_wait(u->empty);
  u->buffer[u->in] = data;
  u->in = (u->in + 1) % u->size;
  sem_post(u->full);
}

/* Read and remove an integer from the buffer */
int get_n (void){
  int data;
  sem_wait(n->full);
  data = n->buffer[n->out];
  n->out = (n->out + 1) % n->size;
  sem_post(n->empty);
  return data;
}

/* Read and remove an integer from the buffer */
int get_u (void){
  int data;
  sem_wait(u->full);
  data = u->buffer[u->out];
  u->out = (u->out + 1) % u->size;
  sem_post(u->empty);
  return data;
}


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
     	  put_n (0);
		  bn++;
          printf("Dim: %d\n", bn);
		}
		else{
		 printf("Milliseconds: %ld - Selected buffer: urgent - ", ms);
     	  put_u (1);
		  bu++;
		  printf("Dim: %d\n", bu);
		}
    }
  return NULL;
}

static void *consumer (void *data){
  int d;
  int i;
 for(i=0;i<100;i++){
	usleep(10*1000); // Sleep for 10 milliseconds
     
	  if(bu >0){
		d = get_u();
	  	printf("->Consuming from urgent buffer - ");
		bu--;
		printf("DIM: %d\n", bu);
	  }
	 if(bu ==0 && bn >0) {
		d= get_n();
		printf("->Consuming from nomal buffer - ");
		bn--;
		printf("DIM: %d\n", bn);
    	}
    }
  return NULL;
}

int main (int argc, char **argv){

 if (argc < 2){
    printf("usage %s: <probability>.\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  pthread_t th_a, th_b;
  void *retval;
  int rc;
  //Initilize the two buffer
  buffer_init_n(BUFFER_SIZE);
  buffer_init_u(BUFFER_SIZE);

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


