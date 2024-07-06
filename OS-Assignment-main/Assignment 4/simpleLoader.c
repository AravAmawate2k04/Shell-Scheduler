#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <elf.h>
#include <stdbool.h>
#include <sys/mman.h>

int fd = -1;                    // file descriptor
int pageFaults = 0;
int pageAllocations = 0;
int  internalFragmentation = 0;
Elf32_Ehdr *ehdr = NULL;
Elf32_Phdr *phdrArr = NULL;
int  pageSize = 4096;
int idxAtSegFaultHappened = -1;
char *virtual_mem = NULL;


// we handle segmentation fault in the below function 
void segv_handler(int signo, siginfo_t *info, void *context) {
    pageFaults++;
    int totalPagesRequired = (*phdrArr).p_memsz/pageSize;
    if ( totalPagesRequired * pageSize <  (*phdrArr).p_memsz ) { totalPagesRequired = totalPagesRequired + 1; }
    virtual_mem = mmap(NULL, pageSize * totalPagesRequired, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    if (virtual_mem == MAP_FAILED){
        printf("Allocating virutal memory failed !!");
        exit(1);
    }
    int sizeStored = (*phdrArr).p_memsz;      // we can use it for freeing up the memory later
    internalFragmentation += (totalPagesRequired*pageSize);
    pageAllocations = totalPagesRequired;
}


// cleaning up the memory
void cleanTheResources(){
    if ( ehdr == NULL ){
        free(ehdr);
    }
    if ( phdrArr == NULL ){
        free(phdrArr);
    }
    if ( virtual_mem == NULL ){
        free(virtual_mem);
    }
    close(fd);
}

// read command line arguments
bool readArgs(int tot_args, char *args[]) {
    if (tot_args != 2) {
        return false;
    }
    return true;
}

// opens file descriptor
bool openFileDescriptor(int fileDescriptor, char *argv[]) {
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        return false;
    }
    return true;
}

// reading and allocating memory for ehdr
bool readEhdr() {
    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    size_t content = read(fd, ehdr, sizeof(Elf32_Ehdr));
    if (content == -1) {
        return false;
    } else if (content != sizeof(Elf32_Ehdr)) {
        return false;
    }
    return true;
}


int main(int argc, char *argv[]) {
    if (!readArgs(argc,argv)){
        printf("\nError in reading args !!\n");
        return -1;
    }
    if ( !openFileDescriptor(fd, argv) ) {
        printf("\nError in opening fd !!");
        return -1;
    }

    // a signal for segmentation fault which call segv_handler if segmentation fault is occured
    struct sigaction segFault;
    segFault.sa_sigaction = segv_handler;
    segFault.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &segFault, NULL);

    if (!readEhdr()) {
        printf("\nError in reading EHDR !!\n");
        return 1;
    }

    // allocating memory for phdr
    phdrArr = (Elf32_Phdr *)malloc(ehdr->e_phnum * sizeof(Elf32_Phdr));
    if (phdrArr == NULL) {
        printf("\nError in doing malloc for phdr !!\n");
        return 1;
    }

    // checking seg fault addresss 
    int cnt = 0;
    while (cnt < ehdr->e_phnum) {
        off_t seek = lseek(fd, ehdr->e_phoff + cnt * sizeof(Elf32_Phdr), SEEK_SET);
        if (seek == -1) {
            printf("\nError in lseek !!\n");
            return -1;
        }

        ssize_t reading = read(fd, &phdrArr[cnt], sizeof(Elf32_Phdr));
        if (reading != sizeof(Elf32_Phdr)) {
            printf("\nError in reading !!\n");
            return -1;
        }
        cnt++;
    }

    // executing the fib or some other file
    int (*entry_point)() = (int (*)())ehdr->e_entry;
    int return_value = entry_point();

    // user statistics
    printf("User _start return value: %d\n", return_value);
    printf("\nTotal number of page Faults      : %d\n", pageFaults);
    printf("Total number of page allocations : %d\n", pageAllocations);
    printf("Amount of Internal fragmentation : %zu KB\n", internalFragmentation );

    // cleaning up the memory used
    cleanTheResources();

    return 0;
}

