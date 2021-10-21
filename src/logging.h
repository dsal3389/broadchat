#pragma once


#define DEBUG 0
#define INFO 10
#define WARNING 20
#define ERROR 30
#define CRITICAL 40

#define LOGGING_BUFFER 1024

#define LOGGING_FILE "/var/log/broadchat.log"

/*
    can be called directly, this function is called by "logging",
    the vlogging logs the string to a file and to the passed stream (stdout or stderr)
*/
void vlogging(FILE *, int, const char * restrict, va_list);

/*
    gets a simple logging level and format,
    then calls vlogging to do all the brain work
*/
void logging(int , const char * restrict, ...);
