#include <iostream>
#include <functional>
#include <pthread.h>
#include <time.h>

// structure for vector
typedef struct {
    int low;
    int high;
    std::function<void(int)> lambda;
} thread_args_vector;


// thread function performing computation
void* applyThreading(void* ptr) {
    thread_args_vector* attributes = ((thread_args_vector*)ptr);
    if ( attributes->low > attributes->high ){
        printf("\nLow greater than high !!\n");
        exit(1);
    }
    for (int low_lim = attributes->low; low_lim < attributes->high; low_lim++) {
        attributes->lambda(low_lim);
    }
    return NULL;
}


// function for vector
void parallel_for(int low, int high, std::function<void(int)>&& lambda, int numThreads) {
    clock_t timeTaken = clock();
    pthread_t threads[numThreads];
    int checkValidOrNot ;
    thread_args_vector args[numThreads];
    int smallerChunk = (high - low) / numThreads;
    for (int j = 0; j < numThreads; j++) {
        args[j].low = 0 + j * smallerChunk;
        args[j].high = 0 + (j + 1) * smallerChunk;
        args[j].lambda = lambda;
        checkValidOrNot = pthread_create(&threads[j], NULL, applyThreading, (void*)&args[j]);
        // error handling
        if ( checkValidOrNot != 0 ){
            printf("\nError in creating threads !!\n");
            exit(1);
        }
    }
    for (int j = 0; j < numThreads; j++) {
        checkValidOrNot = pthread_join(threads[j], NULL);
        if ( checkValidOrNot != 0 ){
            printf("\nError in joining threads !!\n");
            exit(1);
        }
    }
    timeTaken = clock() - timeTaken;
    double timeInSecs = static_cast<double>(timeTaken) / CLOCKS_PER_SEC;
    printf("\nTotal time taken during the execution : %f seconds.\n\n",timeInSecs);
}

typedef struct {
    int low1, high1, low2, high2;
    std::function<void(int, int)> lambda;
} thread_args_matrix;

void* applyThreading2(void* ptr) {
    thread_args_matrix* attributes = (thread_args_matrix*)ptr;
    if ( attributes->low1 > attributes->high1 ){
        printf("\nLow greater than high !!\n");
        exit(1);
    }
    if ( attributes->low2 > attributes->high2 ){
        printf("\nLow greater than high !!\n");
        exit(1);
    }
    for (int i = attributes->low1 + 0; i < attributes->high1; i++) {
        for (int j = attributes->low2; j < attributes->high2; j++) {
            attributes->lambda(i, j);
        }
    }
    return NULL;
}

void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)>&& lambda, int numThreads) {
    int checkValidOrNot ;
    clock_t timeTaken = clock();
    pthread_t threads[numThreads];
    thread_args_matrix args[numThreads];
    int outerChunk = int((high1 - low1) / numThreads);
    for (int k = 0; k < numThreads; k++) {
        args[k].low1 = low1 + k * outerChunk + 0;
        args[k].high1 = low1 + (k + 1) * outerChunk + 0;
        args[k].low2 = low2 + 0;
        args[k].high2 = high2 + 0;
        args[k].lambda = lambda ;
        checkValidOrNot = pthread_create(&threads[k], NULL, applyThreading2, (void*)&args[k]);
        if ( checkValidOrNot != 0 ){
            printf("\nError in creating threads !!\n");
            exit(1);
        }
    }

    for (int k = 0; k < numThreads; k++) {
        checkValidOrNot = pthread_join(threads[k], NULL);
        if ( checkValidOrNot != 0 ){
            printf("\nError in joining threads !!\n");
            exit(1);
        }
    }
   
    timeTaken = clock()-timeTaken;
    double timeInSecs = (double(timeTaken))/CLOCKS_PER_SEC;
    printf("\nTotal time taken during the execution : %f seconds.\n\n ",timeInSecs);
}

int user_main(int argc, char** argv);

void demonstration(std::function<void()>&& lambda) {
    lambda();
}

int main(int argc, char** argv) {
    int x = 5, y = 1;
    auto lambda1 = [x, &y]() {
        y = 5;
        std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
    };

    demonstration(lambda1);

    int rc = user_main(argc, argv);

    auto lambda2 = []() {
        std::cout << "====== Hope you enjoyed CSE231(A) ======\n";
    };
    demonstration(lambda2);

    return rc;
}

#define main user_main
