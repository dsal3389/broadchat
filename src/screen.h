#pragma once

#include "network.h"


#define MESSAGES_BUFFER 110

// ANSI escape sequences code + code names
#define ESC "\033["
#define CPL "F"
#define EL "K"
#define CHA "G"
#define ED "J"
#define CUB "D"
#define CNL "E"
#define UP "A"
#define DN "B"
#define CSAVE "s"
#define CRESTORE "u"

#define MOVE_CURSOR_UP(n) printf(ESC "%d" UP, n)
#define MOVE_CURSOR_DN(n) printf(ESC "%d" DN, n)
#define CLEAN_LINE printf(ESC "2" EL)
#define CLEAN_ALL printf(ESC "2" ED)
#define CLEAN_ALL_FROM_CURSOR printf(ESC ED)
#define CURSOR_SAVE printf(ESC CSAVE)
#define CURSOR_RET printf(ESC CRESTORE)


/* deletes first element in the buffer */
void shiftmessage();

/* push element to the end of the buffer */
void pushmessage(char * restrict, char * restrict);
void fresh();
void printmessages();
void getinput(char *, size_t);
