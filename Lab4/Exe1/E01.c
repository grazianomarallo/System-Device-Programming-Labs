/**
 * Lab_4.1 Graziano Marallo 238159
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <limits.h>
#define NUM_THREADS 2



//Function prototype
void swap(int *i, int *j);
int cmpfunc(const void*, const void*);

//global variables
int size;
int *v=NULL;
long int len;

struct qs {
    int left;
    int right;
};

void *quicksort (void *par) {
    pthread_t thread1,thread2;
    pthread_t *thread = (pthread_t *) malloc (2*sizeof(pthread_t));

    struct qs *params= (struct qs*) par;
    struct qs *th1= (struct qs*) par;
    struct qs *th2= (struct qs*) par;
    int i, j, x, tmp;
    int left,right;
    left=params->left;
    right=params->right;



        if (left >= right)
            pthread_exit(NULL);
    if((right-left)<size) {
        qsort(v,len, sizeof(int),cmpfunc);
    }
    else{

        x = v[left];
        i = left - 1;
        j = right + 1;
        while (i < j) {
            while (v[--j] > x);
            while (v[++i] < x);
            if (i < j)
                swap(&v[i], &v[j]);
        }

        th1->left=left;
        th1->right=j;
        pthread_create(&thread1,NULL,quicksort,(void *) th1);

        th2->left=j+1;
        th2->right=right;
        pthread_create(&thread2,NULL,quicksort,(void *) th2);

    }

    pthread_exit(NULL);

}

int main (int argc, char *argv[]){
    pthread_t thread;
    long int rc;
    int fdo,i; // fdo=file descriptor
    struct qs *param;
    struct stat buffer;
    int pg;



    // read size from input command line
    size = atoi(argv[1]);

    //read integer from binary file
    if ((fdo = open ("fv1.b", O_RDWR)) == -1)
        perror ("open");

    if (fstat (fdo, &buffer) == -1)
        perror ("fstat");
    len = buffer.st_size;



    v = mmap ((caddr_t) 0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fdo, 0);
    if (v == MAP_FAILED)
        perror ("mmap");

    close (fdo);
    len /=  sizeof(int);



    printf("Array read from input file:\n");
    for(i=0;i <len; i++){
        printf("%d ",v[i]);
    }


    param = (struct qs*) malloc(sizeof(struct qs));
    param->left=0;
    param->right=len-1;
    rc = pthread_create (&thread, NULL, quicksort, (void *) param);
    if (rc) {
        fprintf(stderr, "ERROR: pthread_create() code %ld\n", rc);
        exit(-1);
    }
        printf("\n");

    pthread_join(thread,NULL);

    printf("Ordered array:\n");
    for(i=0;i <len; i++){
        printf("%d ",v[i]);
    }

    pthread_exit (NULL);
}

int cmpfunc (const void * a, const void * b) {
    return (*(int *) a - *(int *) b);
}

void swap(int *i, int *j){
    int temp;
    temp = *i;
    *i = *j;
    *j = temp;
}

