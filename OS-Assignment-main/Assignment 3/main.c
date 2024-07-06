#include "SimpleSchedular.c"

// helper functions
void ExitTheShell(int sig);
void ClearFile();
void infinite_input();
char** StringToArray(char *input);
char* read_user_input();
void ExecuteCommands(char **cdsArr);



int main(int argc, char* argv[])
{
    if (argc != 3) return 1;

    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);

    printf("NCPU=%d\n", NCPU);
    printf("TSLICE=%d\n", TSLICE);

    signal(SIGINT, ExitTheShell);
    signal(SIGALRM, handler_for_alarm);
    // alarm(ALARM);

    Initialise();

    infinite_input();

    return 0;
}

// Exit the shell on ctrl+c and clearing the history file
void ExitTheShell(int sig) 
{
    printf("\n\n\033[1;32mExiting SimpleShell...\n\n\n\033[0m");
    ClearFile();
    kill(schedulerPID, SIGTERM);
    exit(0); // Exit the program
}

// freeing up the memory
void ClearFile() {
    destroyQueue(process_queue);
    destroyQueue(running_queue);
}

char* read_user_input() 
{
    size_t len = 0;
    char *line = NULL;
    getline(&line, &len, stdin);
    return line;
}


// converting the string to the array splitted by words
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


// taking input from the user
void infinite_input()
{
    // taking infinite input using do-while loop
    do
    {
        printf("\033[1;32mEnter your command : \033[0m");
        char *command = read_user_input();

        if (strlen(command) == 0)
        {
            printf("\n\033[1;31mPlease Enter a Valid Command !\n \033[0m");
        }

        int SizeOfCommand = strlen(command);
        int j = 0;
        while ( j < SizeOfCommand )
        {
            if (command[j] == '\\' || command[j] == '\'' || command[j] == '"')
            {
                printf("\n\033[1;31mOOPS !!, you cannot enter any Backslashes or any quotes");
                printf("\nYou should only use Whitespaces .\033[0m\n");
                break;
            }
            j++;
        }

        command[strcspn(command, "\n")] = 0;
        char **arr = StringToArray(command);

        if (strcmp(arr[0],"submit")==0) {
            // do some error checks
            enqueue(create_new_process(-1, arr[1]), process_queue);
            show_queue(process_queue);
        } else if (strcmp(arr[0],"run")==0) {
            scheduler(NCPU);
        } else {
            ExecuteCommands(arr);
        }
    } 
    while (true); 
}


// executing the commands using execvp
void ExecuteCommands(char **cdsArr)
{
    int res = fork();
    if ( res == 0 ) {
        execvp(cdsArr[0],cdsArr);
        exit(EXIT_FAILURE);
    } else if ( res < 0 ) {
        perror("\nNot able to execute command !!\n");
        exit(0);
    } else {
        wait(NULL);
    }
}
