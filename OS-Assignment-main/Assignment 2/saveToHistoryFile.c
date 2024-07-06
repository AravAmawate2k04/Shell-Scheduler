#include "saveToHistoryFile.h"

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

void saveToHistoryFile(pid_t pid, time_t StartTheDuration, double duration, char *command) {
    FILE *file = fopen("history.txt", "a");
    if (!file) 
    {
        // file doesnt get openend properly
        perror("\n\033[31mError opening history file !! \n\033[0m");
        return;
    }
    /// Write command details to the file
    fprintf(file, "Command: %s\nPID: %d\nStart Time: %sDuration: %.2f seconds\n---------------------\n",
            command, pid, ctime(&StartTheDuration), duration);
    // closing the file
    fclose(file);
}


// printing history when history is Entered as input by the user
void printHistoryFromFile() 
{
    size_t len = 0;
    char *line = NULL;
    FILE *file = fopen("history.txt", "r");
    if (!file) {
        perror("\n\033[31mError opening history file !! \n\033[0m");
        return;
    }
    // getting each line from file and printing on terminal
    while (getline(&line, &len, file) != -1) 
    {
        printf("%s", line);
    }
    // closing file and then freeing up memory
    fclose(file);
    free(line);
}