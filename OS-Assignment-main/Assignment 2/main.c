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

// Including all the function headers
#include "saveToHistoryFile.h"
#include "PipeCommands.h"
#include "StringToArray.h"
#include "ExecuteCommands.h"
#include "CommandWithAnd.h"
#include "FileStringToArray.h"
#include "ExecuteFileCommands.h"
#include "infinite_input.h"
#include "DoBonus.h"

void ClearFile();
void printHistoryFromFile() ;


// helper method for executing the commands
int launch(char *command) 
{
    time_t StartTheDuration = time(NULL);
    // Breaking the string input to array
    char **arr = StringToArray(command);
    int flagged = 0;

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
    if (pipeOrNot == 1)
    {
        PipeCommands(command);
    }
    else 
    {
        flagged = ExecuteCommands(arr);
    }
    
    double duration = difftime(time(NULL), StartTheDuration);
    saveToHistoryFile(getpid(), StartTheDuration, duration,command);

    return flagged;
}


extern volatile sig_atomic_t flag;



// Exit the shell on ctrl+z and clearing the history file
void ExitTheShell(int sig) 
{
    if(sig == SIGINT || sig == SIGTSTP) {
        printf("\n\n\033[32mExiting SimpleShell...\n\n\n\033[0m");
        ClearFile();
        flag = 1;
        exit(0); // Exit the program
    }
}

int main() 
{
    // BONUS PART
    // executing commands from .sh file
    printf("\n\n");
    DoBonus();
    printf("\n\n");
    signal(SIGINT, ExitTheShell);
    infinite_input();

    return 0;
}
