#include "loader.h"
#include <stdbool.h>


Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd = -1;
char *virtual_mem = NULL;
size_t sizeStored = 0;

// Forward declarations
bool opening_file(char **exe);
int readingAndAllocatingELFHeader();
bool going_to_phdr();
Elf32_Phdr* allocate_and_read_phdr();
int verify_phdr(Elf32_Phdr *temp_phdr, Elf32_Ehdr *temp_ehdr);
int allocatingVirtualMemory();
bool readContentTOMemory();
int executionOfStartMethod(int offset);
int freePHDR();
int freeEHDR();

Elf32_Phdr* findPT_LOAD_helper(int cnt, Elf32_Ehdr* ehdr) {
    // Base case
    // if count becomes greater than or equal to e_phnum
    // we stop the recursion
    if (cnt >= (*ehdr).e_phnum) {
        return NULL;
    }

    Elf32_Phdr *temp_phdr = allocate_and_read_phdr();

    if (verify_phdr(temp_phdr, ehdr)) {
        return temp_phdr;
    }
    else {
        freePHDR(temp_phdr);
        return findPT_LOAD_helper(cnt + 1, ehdr); 
    }
}

// Wrapper function
Elf32_Phdr* findPT_LOAD() {
    return findPT_LOAD_helper(0, ehdr);
}


// 1. Load entire binary content into the memory from the ELF file.
void load_and_run_elf(char **exe) {
    if ( !(opening_file(exe)) )
    {
      printf("Error in opening file Or File is empty !! \n");
    }
    int res = readingAndAllocatingELFHeader();
    if ( res == 0 ){
        printf("Error in reading file !! ");
    }

    going_to_phdr();
    phdr = findPT_LOAD();

    if (phdr == NULL) {
        // we are not able to find PT_LOAD
        printf("Not able to find PT_LOAD segment !! \n");
        return;
    }
    else
    {
        allocatingVirtualMemory();
        int check = readContentTOMemory();
        if ( check == 0 )
        {
            printf("Error in reading file !!!");
        }
        // on subtracting ( ehdr->e_entry - phdr->p_vaddr ) we get the location
        int entry_offset = (*ehdr).e_entry - (*phdr).p_vaddr;
        int result = executionOfStartMethod(entry_offset);
        printf("User _start return value = %d\n", result);
    }

}

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
    freeEHDR(ehdr);
    freePHDR(phdr);
    munmap(virtual_mem, sizeStored);
    close(fd);
    ehdr = NULL;
    phdr = NULL;
}


// "readingAndAllocatingELFHeader" function is used to read the elf header
int readingAndAllocatingELFHeader() {
    // allocating memory so that we can read content 
    // and load the content
    ehdr = (Elf32_Ehdr *)calloc(1,sizeof(Elf32_Ehdr));
    // reading the elf header 
    int res = read(fd, ehdr, sizeof(Elf32_Ehdr));
    if ( res != 0 )
    {
        return 1;
    }
    else
    {
        return false;
    }
}

// Function to navigate to the program header table
bool going_to_phdr() {
    // lseek works like a bookmark, which puts the bookmark from one position to the another position
    lseek(fd, (*ehdr).e_phoff, 0);
    return true;
}

// Allocate and read the program header
Elf32_Phdr* allocate_and_read_phdr() {
    // for reading, we need some space
    Elf32_Phdr *temp_phdr = (Elf32_Phdr *)calloc(1,sizeof(Elf32_Phdr));
    int res = read(fd, temp_phdr, sizeof(Elf32_Phdr));
    if ( res == 0 )
    {
        printf("Error in reading the file !!" );
    }
    else
    {
        return temp_phdr;
    }
}

// Verify the program header against the given conditions
int verify_phdr(Elf32_Phdr *temp_phdr, Elf32_Ehdr *temp_ehdr) {


    int flag = 0;

    // this step is checking whether our phdr's type is PT_LOAD or not
    // if not, then we are not able to find PT_LOAD and hence return False
    bool type_PT_LOAD_or_NOT = (*temp_phdr).p_type == PT_LOAD;


    // consider the analogy :
    // imagine we have a long bookshelf (ELF FILE) containing many books  (SEGMENTS)
    // each book has starting point i.e. pageNo 1  (ENTRY POINT)
    // and we want to read our favourite story from it
    // then, we put a condition "temp_ehdr->e_entry >= temp_phdr->p_vaddr"
    // checking, whether our favourite story start in this book or right after the start of this book
    // if yes, then we proceed to the next condition
    // if not, then we dont read the book
    bool startingCondition = (*temp_ehdr).e_entry >= (*temp_phdr).p_vaddr;


    // temp_ehdr->e_entry == entry point of the program
    // temp_phdr->p_vaddr == starting virtual address of particular segment
    // temp_phdr->p_memsz == tells how big our section is
    // temp_phdr->p_vaddr + temp_phdr->p_memsz == end of the segment
    // the condition, "temp_ehdr->e_entry < (temp_phdr->p_vaddr + temp_phdr->p_memsz)"
    // tells us : if the programs "entry point" is located before the end of this segment
    bool endingCondition = (*temp_ehdr).e_entry < (*temp_phdr).p_vaddr + (*temp_phdr).p_memsz;

    // the nested conditions are checking
    // if the current segment is of type PT_LOAD and
    // if the entry point of the program lies within the boundaries of this segment.

    if ( type_PT_LOAD_or_NOT )
    {
      if ( startingCondition )
      {
        if ( endingCondition )
        {
          flag = 1;
        }
        else
        {
            flag = 0;
        }
      }
    }

    if ( flag == 1 )
    {
        return true;
    }
    else 
    {
        return false;
    }

}


// Allocate virtual memory for the segment
int allocatingVirtualMemory() {
    virtual_mem = mmap(NULL, (*phdr).p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    // now, storing the size of phdr->p_memsz, so that we can free it later
    sizeStored = (*phdr).p_memsz;
    return 0;
}

// Read the segment content to the memory
bool readContentTOMemory() {
    // again, putting the pointer to 0
    lseek(fd, (*phdr).p_offset, 0);
    // phdr->p_filesz represents the file size
    int res = read(fd, virtual_mem, (*phdr).p_filesz);
    if ( res <= 0 )
    {
        return false;
    }
}



// Calculate the offset and execute the _start method
int executionOfStartMethod(int offset) {
    void *func_address = &virtual_mem[offset];
    // type casting
    int (*_start)() = (int (*)())func_address;
    return _start();
}

// Cleanup program header
int freePHDR(Elf32_Phdr *temp_phdr) {
    // not present, or some other issue
    // therefore, free the allocated memory and put phdr to NULL
    if ( temp_phdr != NULL)
    {
        free(temp_phdr);
    }
    return 1;
}

int freeEHDR(Elf32_Ehdr *temp_ehdr)
{
    if ( temp_ehdr != NULL )
    {
        free(temp_ehdr);
    }
    return 1;
}


bool opening_file(char **exe) {
    // fd is the file we are working with
    // "O_RDONYL" : it means that we are opening the file in "READ" mode only
    fd = open(exe[0], O_RDONLY);
    if (fd < 0) {
        return false;
    }
    return true;
}

