#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "common.h"
#include "logging.h"


static void * heap_allocations[HEAP_ALLOCATION_BUFFER];
static unsigned int heap_allocated = 0;


void * hallocate(unsigned int elnum, size_t elsize){
    void * mem = calloc(elnum, elsize);

    heap_allocations[heap_allocated++] = mem;
    return mem;
}

void fatal(const char * fmt, ...){
    va_list args;

    va_start(args, fmt);
    vlogging(stderr, CRITICAL, fmt, args);
    perror(NULL);
    va_end(args);

    exit(EXIT_FAILURE);
}

void cleanup(){
    int i = 0;

    for(; i < heap_allocated; i++){
        free(heap_allocations[i]);
    }
}
