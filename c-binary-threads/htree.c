#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "common.h"
#include "common_threads.h"

// Print out the usage of the program and exit.
void Usage(char*);
uint32_t jenkins_one_at_a_time_hash(const uint8_t*, uint64_t);
void* hashTree(void* args);

// Block size
#define BSIZE 4096

// Argument Struct used to pass arguments to threads
struct argument {
    int nodeNumber;
    uint64_t blocksPerThread;
    uint8_t* arr;
    int nthreads;
};

int main(int argc, char** argv) {
    int32_t fd;
    uint32_t nblocks;

    // Input checking
    if (argc != 3)
        Usage(argv[0]);

    // Open input file
    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    struct stat fileStat;
    fstat(fd, &fileStat);

    // Number of bytes in the file
    uint64_t fileSize = fileStat.st_size;
    nblocks = fileSize / BSIZE;
    int nthreads = atoi(argv[2]);

    if (nthreads == 0){
        printf("num_threads must be an integer\n");
        exit(EXIT_FAILURE);
    }

    uint64_t blocksPerThread = nblocks / nthreads;

    // Mapping file to memory
    uint8_t* arr = (uint8_t*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);

    if (arr == MAP_FAILED) {
        printf("Mapping Failed");
        return 0;
    }

    printf("num Threads = %d\n", nthreads);
    printf("Blocks per Thread = %ld\n", blocksPerThread);

    struct argument args;
    args.arr = arr;
    args.blocksPerThread = blocksPerThread;
    args.nodeNumber = 0;
    args.nthreads = nthreads;

    double start = GetTime();
    
    if (nthreads == 1){
        uint32_t hash = jenkins_one_at_a_time_hash(arr, fileSize);
        printf("hash value = %u\n", hash);

    }
    else {
        // Creating root thread
        pthread_t root;
        char* finalHash;
        Pthread_create(&root, NULL, hashTree, &args);
        Pthread_join(root, (void**)&finalHash);
        printf("hash value = %s \n", finalHash);
    }

    double end = GetTime();
    double elapsed_t = end - start;


    printf("Time taken: %f seconds\n", elapsed_t);
    printf("-----------------------\n");
    close(fd);
    return EXIT_SUCCESS;
}

void* hashTree(void* args) {
    int len = 0;
    char* hashCurrent = (char*)malloc(sizeof(char) * 45);
    char* finalHash = (char*)malloc(sizeof(char) * 15);
    void* hash_left = NULL;
    void* hash_right = NULL;

    struct argument argument = *(struct argument*)args;
    int nodeNumber = argument.nodeNumber + 1;
    uint64_t blocksPerThread = argument.blocksPerThread;
    uint8_t* arr = argument.arr;
    int nthreads = argument.nthreads;

    // Computing hash for current node
    uint32_t hashInt = jenkins_one_at_a_time_hash(&arr[nodeNumber * blocksPerThread], blocksPerThread * BSIZE);
    len = sprintf(hashCurrent, "%u", hashInt);

    struct argument argL;
    struct argument argR;
    pthread_t left;
    pthread_t right;

    // Creating left child if within range
    argL.nodeNumber = 2 * nodeNumber + 1;
    argL.blocksPerThread = blocksPerThread;
    argL.arr = arr;
    argL.nthreads = nthreads;
    if (argL.nodeNumber < nthreads) {
        Pthread_create(&left, NULL, hashTree, &argL);
        Pthread_join(left, &hash_left);
    }

    // Creating right child if within range
    argR.nodeNumber = 2 * nodeNumber + 2;
    argR.blocksPerThread = blocksPerThread;
    argR.arr = arr;
    argR.nthreads = nthreads;
    if (argR.nodeNumber < nthreads) {
        Pthread_create(&right, NULL, hashTree, &argR);
        Pthread_join(right, &hash_right);
    }

    // Concatenating hash values
    if (hash_left != NULL) {
        strcat(hashCurrent, (char*)hash_left);
        len += strlen(hash_left);
    }
    if (hash_right != NULL) {
        strcat(hashCurrent, (char*)hash_right);
        len += strlen(hash_right);
    }

    // Computing final hash for the current node
    uint32_t finalHashInt = jenkins_one_at_a_time_hash((const uint8_t *)hashCurrent, len);
    sprintf(finalHash, "%u", finalHashInt);
    free(hashCurrent);

    return finalHash;
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, uint64_t length) {
    uint64_t i = 0;
    uint32_t hash = 0;

    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

void Usage(char* s) {
    fprintf(stderr, "Usage: %s filename num_threads \n", s);
    exit(EXIT_FAILURE);
}

