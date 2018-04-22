#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 100

int main(int argc, char* argv[])
{
    // Check if there's a correct number of arguments from the command line.
    if(argc < 3){
        printf ("usage: %s <number_of_file>  <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // The maximum number of children that the main process can create.
    int C = atoi(argv[1]);
    char* dir = argv[2];
    char* res;
    char fileName[N], filePath[N];
    pid_t pid;
    // Counter of the children processes created.
    int childrenCreated = 0;
   
    // Outputs in a file called "list.txt" the list of files in the directory "dir".
    system("ls data > list.txt");
    
    // Opens the file called "list.txt".
    FILE* fd = fopen("list.txt", "r");
    if( fd == NULL ){
        perror("Error opening file.\n");
        exit(1);
    }
    
    while(1){
        // Reads each line (a filename) from "list.txt".
        res = fgets(fileName, sizeof(fileName), fd);
        
        // Exits from the loop if the file is empty or if it's finished.
        if(res == NULL)
            break;

        // Removes the '\n' character from the name of the file.
        strtok(fileName, "\n");
        
        // Copies the name of the directory in filePath, then adds the '/' character and concatenates it with fileName.
        strcpy(filePath, dir);
        strcat(filePath, "/");
        strcat(filePath, fileName);
        
        // Creates a child process.
        pid = fork();
        
        if(pid > 0)
        {
            // Parent.
            if(childrenCreated > C)
            {
                // Awaits the termination of the C child process/processes before reading the next filename.
                wait((int*) 0);

                childrenCreated ++;
            }
            else
                childrenCreated ++;
        }
        else
        {
            /*
             * Child.
             * Through the syscall named "execlp" executes the Unix sort program,
             * which order each file in "dir" according to a numerical order.
             */
            execlp("sort", "sort", "-n", "-o", filePath, filePath, (char*) 0);
            
            // Process termination of the child.
            exit(0);
        }
    }

    // Close the file.
    fclose(fd);


    
    
    char* token;

    char sortcall[4000] = "";

    strcpy(sortcall, "sort -n -o ");
    strcat(sortcall, "\"");
    strcat(sortcall, dir);
    strcat(sortcall, "/../all_sorted.txt\"");
    

    // Opens the file called "list.txt".
    fd = fopen("list.txt", "r");
    if( fd == NULL )
    {
        perror("Error opening file.\n");
        exit(1);
    }

    while(1)
    {
	// Reads each line (a filename) from "list.txt".
        res = fgets(fileName, sizeof(fileName), fd);
        
        // Exits from the loop if the file is empty or if it's finished.
        if(res == NULL)
            break;
        
	token = strtok(fileName, "\n");
        
        char expath[3000] = "";

        strcpy(expath, " \"");
        strcat(expath, dir);
        strcat(expath, "/");
        strcat(expath, fileName);
        strcat(expath,"\"");
        strcat(sortcall, expath);
    }

    fclose(fd);
    system(sortcall);
    
    exit(0);
}

