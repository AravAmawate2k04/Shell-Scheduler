#include "FileStringToArray.h"
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

// getting commands from file and then making array of the commands
char** FileStringToArray(char *input) 
{
    int i = 0;
    char *duplicate;
    int len = 0;

    for (len = 0; input[len] != '\0'; len++);

    duplicate = (char *)malloc((len + 1) * sizeof(char));
    // MEMORY ALLOCATION FAILURE
    if (!duplicate)
    {
        exit(1);
    }
    // making copy of the commands
    for (int j = 0; j <= len; j++) 
    {
        duplicate[j] = input[j];
    }
    // seperating the commands on the basis of spaces
    char *BrokenBySpaces = strtok(duplicate, " ");

    char **array = (char **)calloc(1024, sizeof(char *));
    for (BrokenBySpaces ; BrokenBySpaces != NULL; BrokenBySpaces = strtok(NULL, "&"))  
    {
        array[i++] = strdup(BrokenBySpaces);
    }
    free(duplicate);
    array[i] = NULL;
    return array;
}