#pragma once

#include <errno.h>

#define HEAP_ALLOCATION_BUFFER 256 // 256 pointers not bytes

#define IF_FAIL(cond, msg, ...) ({ if(cond) { fatal(msg "(errno: %d)\n", __VA_ARGS__, errno); } })


void * hallocate(unsigned int, size_t);

/* dies, killes the program and logging passed string */
void fatal(const char *, ...);

/* dynamicly allocated memory via "hallocate" is beinged free */
void cleanup();
