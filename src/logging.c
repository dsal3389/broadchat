#include <stdio.h>
#include <stdarg.h>

#include "logging.h"


int logginglevel = INFO;


static void logToFile(int level, const char * restrict message){
    FILE * f = fopen(LOGGING_FILE, "a");

    fputs(message, f);
    fclose(f);
}

void vlogging(FILE * stream, int level, const char * restrict fmt, va_list vargs){
    char buff[LOGGING_BUFFER];

    vsnprintf(buff, LOGGING_BUFFER, fmt, vargs);

    // output to stdout if given level is bigger then global level
    if(
        (stream == stdout || stream == stderr) && \
        logginglevel <= level
    ){
        fprintf(stream, buff);
    }

    logToFile(level, buff);
}

void logging(int level, const char * restrict fmt, ...){
    va_list vargs;
    FILE * out = stdout;

    if(level < WARNING){
        out = stderr;
    }

    va_start(vargs, fmt);
    vlogging(out, level, fmt, vargs);
    va_end(vargs);
}
