#include <printf.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>


void writeFile(char *,int *,int len);
void randomGenerator(int* arr, int random);

int main(int argc,const char *argv[]) {
    struct dirent *dir;
    char *ptr;
    int n,k,random,i,rnum;
    struct stat st = {0};
    int *arr;
    char filename[3];
    
 

	/* Check if params from command line are the exact ones*/
	if(argc < 3) {
        printf ("usage: %s <number_of_file> <k> <filename>\n", argv[0]);
        return 0;
    	}
    

    //read from input the numbers to use for the program
    n = (int)strtol(argv[1], &ptr,10);
    k = (int)strtol(argv[2],&ptr,10);
    printf("The value of n is: %d \nValue of k is: %d \n", n, k);

    //check if the directory already exist, if not it is created
    if (stat("./data", &st) == -1) {
        mkdir("./data", 0700);
    }
   
    srand(time(NULL));
    rnum = rand();
    rnum *= 2;
    if (rnum < 0)
       rnum = -rnum;
    random = 1 + (rnum % k); // random even number between 1-k
    arr = malloc(sizeof(int)*random);
    printf("Random generated number between 1 and k is: %d \n", random);
   
    
     for(i=0; i< n; i++){
        sprintf(filename,"f%02d", i);
         randomGenerator(arr,random);
        writeFile(filename,arr,random);
    }
    
    

    return 0;
}


void randomGenerator(int* arr, int random){
    int rnum;
    int i;
    
    for (i = 0; i < random; i++) {
        rnum = rand();
        rnum *= 2;
        if (rnum < 0)
           rnum = -rnum;   
        arr[i] = 1 + (rnum % 100);
    }
}

void writeFile(char *filename,int *buffer,int len) {
    FILE *fpw; // file descriptor
    int i;
    char pathfile[10];
    
    sprintf(pathfile,"./data/%s",filename);
    if ((fpw = fopen(pathfile, "w")) == NULL) {
        fprintf(stderr, " error open %s\n", filename);
        return;
    }
    for (i = 0; i < len; i++) {
    fprintf(fpw, "%d", buffer[i]);
	fprintf(fpw,"\n");
 }
}







