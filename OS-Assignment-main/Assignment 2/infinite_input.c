#include "infinite_input.h"
#include "saveToHistoryFile.h"
#include "StringToArray.h"
#include "CommandWithAnd.h"
#include "PipeCommands.h"
#include "ExecuteCommands.h"


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

void printHistoryFromFile() ;

// clearing the history file, when user enters clear in the command
void ClearFile()
{
    FILE *file = fopen("history.txt", "w");
    if ( !file )
    {
        perror("\n\033[31mError opening history file !! \n\033[0m");
    }
    else
    {
        fclose(file);
        printf("\n\nHISTORY CLEARED !! \n\n");
    }
}


char* read_user_input() 
{
    size_t len = 0;
    char *line = NULL;
    getline(&line, &len, stdin);
    return line;
}

volatile sig_atomic_t flag = 0;

// taking infinite input until ctrl+z is pressed
void infinite_input()
{
    // taking infinite input using do-while loop
    do
    {
        if(flag)  // check if flag is set
            break;
        printf("\n\n\033[32mEnter your command : \033[0m");

        char *command = read_user_input();

        if (strlen(command) == 0)
        {
            printf("\n\033[31mPlease Enter a Valid Command !\n \033[0m");
        }

        int SizeOfCommand = strlen(command);
        int j = 0;
        while ( j < SizeOfCommand )
        {
            if (command[j] == '\\' || command[j] == '\'' || command[j] == '"')
            {
                printf("\n\033[31mOOPS !!, you cannot enter any Backslashes or any quotes");
                printf("\nYou should only use Whitespaces .\033[0m\n");
                break;
            }
            j++;
        }
        command[strcspn(command, "\n")] = 0;
        time_t current_time = time(NULL);
        saveToHistoryFile(getpid(), current_time, 0,command);
        char **arr = StringToArray(command);

        if (strcmp(command, "clear") == 0) 
        {
            ClearFile();
            continue;
        }

        int andOrNot = 0;
        j = 0;
        while ( j < SizeOfCommand )
        {
            if (command[j] == '&')
            {
                andOrNot = 1;
                break;
            }
            j++;
        }

        int pipeOrNot = 0;
        int i = 0;
        while (i < strlen(command))
        {
            if (command[i] == '|')
            {
                pipeOrNot = 1;
                break;
            }
            i++;
        }

        if (andOrNot == 1)
        {
            CommandWithAnd(command);
        }
        else if (pipeOrNot == 1)
        {
            PipeCommands(command);
        }
        else
        {
            bool commandRun = ExecuteCommands(arr);
        }

        if (strcmp(command, "history") == 0) 
        {
            printf("\n");
            printHistoryFromFile();
            continue;   
        }

    } 
    while (true); 
}