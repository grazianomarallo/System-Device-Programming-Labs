/**
 * Lab_1.1 Graziano Marallo 238159
 */


#include <printf.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

void fillArr(int*,int, int*,int );
void print(int*,int);
int cmpfunc(const void*, const void*);
void writeFileBin(char *,const void *,int len);
void writeFileTxt(char *,int *,int len);

int main(int argc,const char *argv[]) {
    char *ptr;
    int n1,n2;
    int *v1,*v2;

    //read from input the numbers to use for the program
    n1 = (int)strtol(argv[1], &ptr,10);
    n2 = (int)strtol(argv[2],&ptr,10);
    printf("The input number are : %d,%d \n", n1, n2);

    //allocate dinamically memory for the two vectors
    v1 = malloc(sizeof(int)*n1);
    v2 = malloc(sizeof(int)*n2);

    //fill the two vectors and print the unsorted vectors
    fillArr(v1,n1,v2,n2);
    printf("First vector:\n");
    print(v1,n1);
    printf("\nSecond vector:\n");
    print(v2,n2);
    //Sort the vectors using qsort function
    qsort(v1, n1, sizeof(int), cmpfunc);
    qsort(v2, n2, sizeof(int), cmpfunc);

    printf("\nSorted first vector:\n");
    print(v1,n1);
    printf("\nSorted second vector:\n");
    print(v2,n2);
    printf("\n");

    writeFileTxt("fv1.txt",v1,n1);
    writeFileTxt("fv2.txt",v2,n2);
    writeFileBin("fv1.b",v1,n1);
    writeFileBin("fv2.b",v2,n2);


    return 0;
}
    // method used to fill vectors with randomly generated number
void fillArr(int* v1,int n1, int* v2, int n2) {
    int i;
    int rnum, rnum1;
    srand(time(NULL));

    for (i = 0; i < n1; i++) {      // random even number between 10-100
        rnum = rand();
        rnum *= 2;
        if (rnum < 0)
            rnum = -rnum;

        v1[i] = 10 + (rnum % 90);
    }

    for (i = 0; i < n2; i++) {  // random odd number between 21-101
        rnum1 = rand();
        rnum1 *= 2;
        if (rnum1 < 0)
            rnum1 = -rnum1;

        v2[i] = 21 + (rnum1 % 80);
    }
}

//used by qsort function to make comparison between numbers
int cmpfunc (const void * a, const void * b) {
    return (*(int *) a - *(int *) b);
}


    void print(int *v,int n) {
        int i;
        for(i=0; i< n; i++){
            printf("%d ",v[i]);
        }
    }

//This method is used to write a binary file that has as input
//the filename, buffer and the lenght in bytes
void writeFileBin(char *filename,const void *buffer,int len) {
    int fdo; // file descriptor
    if ((fdo = open(filename, O_WRONLY)) < 0) {
        fprintf(stderr, " error open %s\n", filename);
        return;
    }
    write(fdo, buffer, sizeof(int)*len);
}

//this method is used to write a txt file. Parameter are similar to
//the other method except for buffer that is an integer in this case
//Instead of using write for generating the file, it has been used
//fprintf because it's needed to write char.With a loop get all numbers
//and write on the file
void writeFileTxt(char *filename,int *buffer,int len) {
    FILE *fpw; // file descriptor
    int i;
    if ((fpw = fopen(filename, "w")) == NULL) {
        fprintf(stderr, " error open %s\n", filename);
        return;
    }
    for (i = 0; i < len; i++) {
    fprintf(fpw, "%d ", buffer[i]);
 }
}





