#include "StringToArray.h"
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

// converting the string input to the array
// e.g. 
// command = "wc -l fib.c"
// arrayOfWords = { "wc" , "-l" , "fib.c" };
char** StringToArray(char *input) 
{

    int i = 0;
    char *duplicate;
    char **arrayOfWords = (char **)calloc(1024, sizeof(char *));
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


    // splitting the commands on the basis of spaces
    char *BrokenFromSpace = strtok(duplicate, " ");
    for (BrokenFromSpace ; BrokenFromSpace != NULL; BrokenFromSpace = strtok(NULL, " ")) 
    {
        arrayOfWords[i++] = strdup(BrokenFromSpace);
    }

    arrayOfWords[i] = NULL;
    free(duplicate);
    return arrayOfWords;
}