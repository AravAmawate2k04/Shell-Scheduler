#include "ExecuteFileCommands.h"
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

// executing the file commands
void ExecuteFileCommands(char ***cds, int totalCommands) 
{
    int k = 0;
    while ( k < totalCommands )
    {
        pid_t res = fork();
        time_t StartTheDuration = time(NULL); 
        if ( res < 0 )
        {
            printf("\n\033[31mFork Execution Failed !! \n\033[0m");
            exit(0);
        }
        else if ( res == 0 )
        {
            execvp(cds[k][0] , cds[k]);
            perror("\n\033[31mExecvp failed !! \n\033[0m");
            exit(EXIT_FAILURE);
        }
        else
        {
            int TotalLength = 0;
            wait(NULL);
            double duration = difftime(time(NULL), StartTheDuration);
            for (int j = 0; cds[k][j] != NULL; j++) 
            {
                TotalLength += strlen(cds[k][j]) + 1; 
            }
            // for the history purpose
            char *ArrayToString = (char *)calloc(TotalLength, sizeof(char));
            ArrayToString[0] = '\0';
            for (int j = 0; cds[k][j] != NULL; j++) 
            {
                strcat(ArrayToString, cds[k][j]);
                if (cds[k][j + 1] != NULL)
                    strcat(ArrayToString, " ");
            }
            saveToHistoryFile(res, StartTheDuration, duration,ArrayToString);
            free(ArrayToString);
        }
        k++;
    }
}