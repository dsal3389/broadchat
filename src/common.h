#pragma once

#include <errno.h>

#define HEAP_ALLOCATION_BUFFER 256 // 256 pointers not bytes

#define IF_FAIL(cond, msg, ...) ({ if(cond) { fatal(msg "(errno: %d)\n", __VA_ARGS__, errno); } })


void * hallocate(unsigned int, size_t);
void fatal(const char *, ...);
void cleanup();
