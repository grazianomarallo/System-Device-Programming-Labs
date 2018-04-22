#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define BUFFER_SIZE 512
#define SECONDS 60

void sigUsr1(int);
void sigUsr2(int);

// waiting variable will be used only by parent
int waiting = 0;
int cpid = 0;

int main(int argc, char **argv) {
    // check on number of parameters
/* Check if params from command line are the exact ones*/
	if(argc < 2) {
        printf ("usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    	}
    cpid = fork();
    if(cpid) {
        // parent
        FILE *fp = fopen(argv[1], "rt");
        if(fp == NULL) {
            printf("Error opening file\n");
            // kill also the child
            kill(cpid, SIGKILL);
            return -1;
        }
        char *buffer = malloc(BUFFER_SIZE * sizeof(char));
        int i;
        signal(SIGUSR1, sigUsr1);
        signal(SIGUSR2, sigUsr2);
        while(1) {
            // this loop repeats the whole file read
            i = 0;
            while(fgets(buffer, BUFFER_SIZE, fp)) {
                // this inner loop iterates on the lines of the file
                if(waiting) {
                    pause();
                }
                printf("%d:  %s", ++i, buffer);
                fflush(stdout);
            }
            rewind(fp);
        }
    } else {
        // child
        int secs = SECONDS;
        int randoms;
        time_t t;
        srand((unsigned) time(&t));
        while(secs > 0) {
            if(secs < 10) {
                randoms = secs;
            } else {
                randoms = rand() % 10 + 1;
            }
            secs -= randoms;
            sleep(randoms);
            // send SIGUSR1 to parent
            kill(getppid(), SIGUSR1);
            //printf("\nStill remaining %d seconds\n", secs_remaining);
        }
        //printf("%d: I'm killing my parent %d\n", getpid(), getppid());
        kill(getppid(), SIGUSR2);
    }
    return 0;
}

void sigUsr1(int signo) {
    waiting = ++waiting % 2;
    signal(SIGUSR1, sigUsr1);
    //printf("%d: I received SIGUSR1 from my child %d\n", getpid(), cpid);
}

void sigUsr2(int signo) {
    //printf("%d: I received SIGUSR2 from my child %d\n", getpid(), cpid);
    exit(0);
}
