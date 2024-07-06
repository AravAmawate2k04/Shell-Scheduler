#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

// structure of a process
typedef struct Process {
    pid_t pid;
    char command[128];
    bool new;
} Process;

// making a linked list that can work as a queue
typedef struct QueueNode {
    Process* process;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

// globals
int NCPU;
int TSLICE;
int ALARM = 15;
pid_t schedulerPID;
Queue* process_queue;
Queue* running_queue;

// helper functions
void Initialise();
Process* create_new_process(pid_t pid, char command[]);
void enqueue(Process* p, Queue* q);
QueueNode* dequeue(Queue *q);
void show_queue(Queue* q);
void destroyQueue(Queue* q);
void scheduler();
void handler_for_alarm();

// initialization of the queue
void Initialise() {
    process_queue = malloc(sizeof(Queue));
    process_queue->front = NULL;
    process_queue->rear  = NULL;

    running_queue = malloc(sizeof(Queue));
    running_queue->front = NULL;
    running_queue->rear = NULL;
}

// creating a new process
Process* create_new_process(pid_t pid, char command[]) {
    Process* p = (Process*) malloc(sizeof (Process));
    strcpy(p->command, command);
    p->pid = pid;
    p->new = true;
}

// adding a process to the queue
void enqueue(Process* p, Queue* q) {
    QueueNode* tmp = malloc(sizeof(QueueNode));
    tmp->process = p;
    tmp->next = NULL;

    if (!q->front) {
        q->front = tmp;
        q->rear = tmp;
    } else {
        q->rear->next = tmp;
        q->rear = tmp;
    }
}

// removing a process from the queue and it will give us the pid
// we always remove a process from the front
QueueNode* dequeue(Queue *q) {
    if (!q->front) {
        return NULL;
    }
    QueueNode* tmp = q->front;

    q->front = q->front->next;
    tmp->next = NULL;
    return tmp;
}

// showing the status of the queue
void show_queue(Queue* q) {
    if (!q->front) {
        printf("empty");
        return;
    }

    QueueNode* tmp = q->front;
    while (tmp!=NULL)
    {
        printf("%d %s ", tmp->process->pid, tmp->process->command);
        if (tmp->process->new) {
            printf("new\n");
        } else {
            printf("ready\n");
        }
        tmp = tmp->next;
    }
}


// cleaning up the memory used
void destroyQueue(Queue* q) {
    if (!q->front) {
        return;
    }

    QueueNode* tmp = q->front;
    QueueNode* tmp2;
    while (tmp!=NULL)
    {
        tmp2 = tmp->next;
        free(tmp->process);
        free(tmp);
        tmp = tmp2;
    }

    free(q);
}


// main schedular funcitons
void scheduler(int num_procs) {
    // printf("In the scheduler function\n");
    // stop the already running processes
    while (1) {
        QueueNode* n = dequeue(running_queue);
        if (!n) {
            break;
        }

        kill(n->process->pid, SIGTSTP);
        enqueue(n->process, process_queue);
        free(n);
    }

    // add new processes to running queue
    for (int i = 0; i < num_procs; i++) {
        QueueNode* n = dequeue(process_queue);
        if (!n) {
            // no more processes left to run
            return;
        } 
        
        enqueue(n->process, running_queue);
        free(n);
    }

    // run the processes added to running queue
    for (int i = 0; i < num_procs; i++) {
        // dequeuing a process and running it
        QueueNode* n = dequeue(running_queue);
        if (n->process->new) {
            pid_t child = fork();
            if (!child) {
                char *arguments[] = {n->process->command, NULL};
                execvp(arguments[0], arguments);
                exit(0);
            } else {
                n->process->pid = child;
                n->process->new = false;
            }
        } else {
            kill(n->process->pid, SIGCONT);
        }
        enqueue(n->process, running_queue);
        free(n);
    }
    
    for (int i = 0; i < num_procs; i++) {
        QueueNode* n = dequeue(running_queue);
        waitpid(n->process->pid, NULL, WNOHANG);
        enqueue(n->process, running_queue);
        free(n);

        usleep(TSLICE * 1000);
        scheduler(NCPU);
    }
}


// main handler function
void handler_for_alarm() {
    scheduler(NCPU);
}
