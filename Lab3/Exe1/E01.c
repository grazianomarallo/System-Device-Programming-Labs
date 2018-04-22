#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "buffer.h"
#include <sys/time.h>




// Global variables
buffer_item normal[BUFFER_SIZE];
buffer_item urgent[BUFFER_SIZE];

pthread_mutex_t mutex, mutex1;
sem_t full, empty,full1,empty1;
int count, in, out;
int count_u, in_u,out_u;
int p;
int s_buf;

// Function prototypes
int insert_item(buffer_item item);
int insert_item_urgent(buffer_item item);
int remove_item(buffer_item *item);
void *consumer(void *param);
void *producer(void *param);

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}


int main(int argc, char **argv){
  if (argc < 2){
    printf("usage %s: <probability>.\n",argv[0]);
    exit(EXIT_FAILURE);
  }



  // Initialize
  p = atoi(argv[1]);	//get probability from command line
  printf("P-> %d\n",p);
  srand(time(NULL));
  pthread_mutex_init(&mutex, NULL);
  sem_init(&empty, 0, BUFFER_SIZE); // All of buffer is empty
  sem_init(&full, 0, 0);
  pthread_mutex_init(&mutex1, NULL);
  sem_init(&empty1, 0, BUFFER_SIZE); // All of buffer is empty
  sem_init(&full1, 0, 0);
  count = in = out = 0;
 count_u = in_u = out_u = 0;


  // Create the producer and consumer threads
  pthread_t prod,cons;
 
 
	int rc,rc1;
	rc =  pthread_create(&prod, NULL, producer, NULL);
	if (rc) {
		fprintf(stderr,"ERROR! Generating the thread!");
		exit (-1);
	}
	rc1 =   pthread_create(&cons, NULL, consumer, NULL);
	if (rc1) {
		fprintf(stderr,"ERROR! Generating the thread!");
		exit (-1);
	}

  
	pthread_join(prod, NULL);
	pthread_join(cons, NULL);
    sleep(10);
  return 0;
}

// Insert item into buffer.
//Returns 0 if successful, -1 indicating error
int insert_item(buffer_item item){
  int success;
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);

  // Add item to buffer
  if( count != BUFFER_SIZE){
    normal[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;
    success = 0;
  }
  else
    success = -1;

  pthread_mutex_unlock(&mutex);
  sem_post(&full);
  
  return success;
}

// Insert item into buffer.
//Returns 0 if successful, -1 indicating error
int insert_item_urgent(buffer_item item){
  int success;
  sem_wait(&empty1);
  pthread_mutex_lock(&mutex1);

  // Add item to buffer
  if( count_u != BUFFER_SIZE){
    urgent[in_u] = item;
    in_u = (in_u + 1) % BUFFER_SIZE;
    count_u++;
    success = 0;
  }
  else
    success = -1;

  pthread_mutex_unlock(&mutex1);
  sem_post(&full1);
  
  return success;
}

// Remove an object from the buffer, placing it in item.
// Returns 0 if successful, -1 indicating error
int remove_item(buffer_item *item){
  int success;
  
  sem_wait(&full);
  pthread_mutex_lock(&mutex);
  
  // Remove item from buffer to item
  if( count != 0){
    *item = normal[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;
    success = 0;
  }
  else
    success = -1;

  pthread_mutex_unlock(&mutex);
  sem_post(&empty);
  
  return success;
}

int remove_item_urgent(buffer_item *item){
  int success;
  
  sem_wait(&full1);
  pthread_mutex_lock(&mutex1);
  
  // Remove item from buffer to item
  if( count_u != 0){
    *item = urgent[out_u];
    out_u = (out_u + 1) % BUFFER_SIZE;
    count_u--;
    success = 0;
  }
  else
    success = -1;

  pthread_mutex_unlock(&mutex1);
  sem_post(&empty1);
  
  return success;
}

void *producer(void *param){
  buffer_item ms;
int random,i;
  printf("Producer ok");
  for(i=0;i<100;i++){
    sleep(rand() % 10000 + 1000); // Sleep randomly between 1 and 10 milliseconds
    ms = current_timestamp();	//get current time in milliseconds
	random = rand() % p + (1-p);
	if(random % 2 ==0){
		// get normal buffer
		s_buf=0;
		printf("Milliseconds: %ld - Selected buffer: %d", ms, s_buf);
		if(insert_item(ms))
     	 printf("Error occured\n");
    	else
      		printf("Producer produced-> %ld\n", ms);
	}
	else{
		//get urgent buffer
		s_buf=1;
		printf("Milliseconds: %ld - Selected buffer: %d", ms, s_buf);
		if(insert_item_urgent(ms))
     	 printf("Error occured\n");
    	else
      	 printf("Producer produced-> %ld\n", ms);
	}
    
  }
}


void *consumer(void *param){
  buffer_item item;
  int i;
  for(i=0;i<100;i++){
    sleep(10000); // Sleep randomly between 10 millisecons
	if(s_buf==0){
    if(remove_item(&item))
      printf("Error occured\n");
    else
      printf("Consumer consumed %ld\n", item);
	}
	else{
	if(remove_item_urgent(&item))
      printf("Error occured\n");
    else
      printf("Consumer consumed %ld\n", item);
	}
	
  }



}
