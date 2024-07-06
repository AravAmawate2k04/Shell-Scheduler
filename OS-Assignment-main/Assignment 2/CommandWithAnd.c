#include "CommandWithAnd.h"
#include "StringToArray.h"
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

// trimming the whitespaces 
char *trim(char *str)
{
    char *end;

    // Trim leading spaces
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Null terminate
    end[1] = '\0';

    return str;
}

// executing commands containing &
void CommandWithAnd(char *input) 
{
    // Split input based on '&'
    char *BrokenByAnd;
    int n = 0;
    char *commands[1024];
    
    int i = 0;
    char *duplicate;
    int len = 0;

    for (len = 0; input[len] != '\0'; len++);

    duplicate = (char *)malloc((len + 1) * sizeof(char));
    if (!duplicate)
    {
        exit(1);
    }
    // making a copy of the command
    for (int j = 0; j <= len; j++) 
    {
        duplicate[j] = input[j];
    }

    BrokenByAnd = strtok(duplicate, "&");
    for (BrokenByAnd ; BrokenByAnd != NULL; BrokenByAnd = strtok(NULL, "&"))  
    {
        char *trimmedCommand = trim(BrokenByAnd);
        if(strlen(trimmedCommand) == 0) 
        {
            continue; 
        }
        commands[i++] = strdup(trimmedCommand);
    }

    int j = 0;
    // executing the commands now
    while ( j < i )
    {
        pid_t pid = fork();
        time_t StartTheDuration = time(NULL);
        if (pid < 0 )
        {
            printf("\n\033[31mFork failed !!\n\033[0m");
            exit(EXIT_FAILURE);
        }
        else if ( pid == 0 )
        {
            char **BrokenCommands = StringToArray(commands[j]);
            execvp(BrokenCommands[0] , BrokenCommands);
            perror("\n\033[31mExecvp failed!! \n\033[0m");
            exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
            double duration = difftime(time(NULL), StartTheDuration);
            saveToHistoryFile( pid, StartTheDuration, duration , commands[j]);
        }
        j++;
    }

    free(duplicate);
}