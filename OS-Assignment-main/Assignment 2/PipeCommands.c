#include "PipeCommands.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include<ctype.h>

// Executing Pipe Commands seperately from executing Other commands
void PipeCommands(char *input) 
{
    char **arr = (char **)calloc(1024,sizeof(char *));
    int i = 0;

    // making a copy of the command
    char *duplicate;
    int len = 0;

    for (len = 0; input[len] != '\0'; len++);

    duplicate = (char *)malloc((len + 1) * sizeof(char));
    // MEMORY ALLOCATION FAILURE
    if (!duplicate)
    {
        exit(1);
    }
    for (int j = 0; j <= len; j++) 
    {
        duplicate[j] = input[j];
    }

    // splitting the copy of commands on basis of pipes
    char *BrokenByPipes = strtok(duplicate, "|"); 
    for (BrokenByPipes ; BrokenByPipes != NULL; BrokenByPipes = strtok(NULL, "|"))
    {
        arr[i++] = strdup(BrokenByPipes);
    }
    arr[i] = NULL;


    char ***array = (char ***)calloc(1024,sizeof(char **));
    int j = 0;
    // now, splitting those into spaces, so that we get a 3d array
    while ( j < i )
    {
        int k = 0;
        array[j] = (char **)calloc(1024, sizeof(char *));
        char *duplicate = strdup(arr[j]);
        char *Broken = strtok(duplicate, " ");

        for(Broken ; Broken != NULL; Broken = strtok(NULL, " ")) 
        {
            array[j][k] = Broken;
            k++;
        }
        array[j][k] = NULL;
        j++;
    }


    time_t startPipeTime = time(NULL);
    // executing pipe commands

    // Initialize loop control variable
    int l = 0;
    
    // Declare an array of file descriptors for the pipe based on the number of commands
    int fd[2 * (i - 1)]; 

    // Create pipes for each command to be executed
    while ( l < i-1  )
    {
        // Create a pipe. If there's an error, terminate the program
        if (pipe(fd+2*l)==-1)
        {
            perror("\n\033[31nPipe Error !!\n\033[0m");
            exit(EXIT_FAILURE);
        }
        l++;
    }

    // Reset the loop control variable
    l = 0;

    // For each command, create a child process and execute the command
    while ( l < i )
    {
        // Create a child process
        int res = fork();
        
        // If this is the child process
        if ( res == 0 )
        {
            // If this isn't the last command, set the write-end of the pipe
            if ( l != i-1)
            {
                dup2(fd[2*l+1] , 1);  // Redirect standard output to the write end of the pipe
            }
            
            // If this isn't the first command, set the read-end of the pipe
            if ( l != 0 )
            {
                dup2(fd[2*(l-1)] , 0); // Redirect standard input to the read end of the previous pipe
            }
            
            // Close all unused file descriptors
            int p = 0;
            while (p < 2*i-2 )
            {
                close(fd[p]);
                p++;
            }

            // Execute the current command
            execvp(array[l][0], array[l]);

            // If there's an error in executing the command, print an error and exit
            perror("\n\033[31mExecvp Failed !! \n\033[0m");
            exit(EXIT_FAILURE);
        }
        l++;
    }

    // closing file descriptors
    int u = 0;
    while (u < (2*i-2) )
    {
        close(fd[u]);
        u++;
    }
    u = 0;
    while ( u < i )
    {
        wait(NULL);
        u++;
    }
    double durationPipe = difftime(time(NULL), startPipeTime); 
}