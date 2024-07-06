#include "DoBonus.h"
#include "FileStringToArray.h"
#include "ExecuteFileCommands.h"

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

// executing commands that are present in bash.sh file
bool DoBonus()
{
    FILE *file = fopen("bash.sh", "r");

    if (!file) 
    {
        perror("\n\033[31mError opening file\n\033[0m");
        return false;
    }

    char *line = NULL;
    size_t len = 0;
    size_t read;

    char ***cds = (char ***)calloc(1024, sizeof(char **));
    int cmd_counter = 0;

    bool Line0OrNot = 1;

    for(read = getline(&line, &len, file); read != -1; read = getline(&line, &len, file))
    {
        if (Line0OrNot) 
        {
            Line0OrNot = false;
            continue;
        }
        line[strcspn(line, "\n")] = 0;
        cds[cmd_counter++] = FileStringToArray(line);
        }

    ExecuteFileCommands(cds, cmd_counter);
    int q = 0;
    while (q < cmd_counter)
    {
        for (int j = 0; cds[q][j] != NULL; j++) 
            {
                free(cds[q][j]);
            }
        free(cds[q]);
        q++;
    }

    free(cds);
            
    if (file)
        fclose(file);

    if (line)
        free(line);
    return true;
}