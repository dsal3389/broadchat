#pragma once


#define MESSAGES_BUFFER 110


void shiftMessage();
void pushMessage(char * restrict, char * restrict);
void fresh();
void printMessages();
void getInput(char *, size_t);
