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


// Running commands which are not either pipe and not containing &
bool ExecuteCommands(char **cdsArr)
{
    int res = fork();
    if ( res == 0 )
    {
        execvp(cdsArr[0],cdsArr);
        perror("execvp");  
        exit(EXIT_FAILURE);
    }
    else if ( res < 0 )
    {
        perror("\nNot able to execute command !!\n");
        exit(0);
    }
    else
    {
        wait(NULL);
    }
    return true;
}