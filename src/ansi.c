/*
most of the code in this file was taken from the video:
https://www.youtube.com/watch?v=bQ8qaBjJtYU&list=LL&index=3
*/
#include <stdio.h>
#include <sys/ioctl.h>

#include "ansi.h"

#ifdef _WIN
    #define _CRT_SECURE_NO_WARNINGS 1
    #include <windows.h>


    static HANDLE stdoutHandle;
    static DWORD outModeInit;


    void setupTerminal(){
        DWORD outMode = 0;
        stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        if(stdoutHandle == INVALID_HANDLE_VALUE){
            exit(GetLastError());
        }

        if(!GetConsoleMode(stdoutHandle, &outMode)) {
            exit(GetLastError());
        }

        outModeInit = outMode;
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if(!SetConsoleMode(stdoutHandle, outMode)) {
            exit(GetLastError());
        }
    }

    void restoreTerminal(){
        SET_COLOR_CODE(0);

        if(!SetConsoleMode(stdoutHandle, outModeInit)) {
            exit(GetLastError());
        }
    }
#else
    void setupTerminal() {}
    void restoreTerminal() {
        SET_COLOR_CODE(0);
    }
#endif

void blankScreen(){
    struct winsize window;
    ioctl(1, TIOCGWINSZ, &window);

    CLEAN_ALL;
    MOVE_CURSOR_DN(window.ws_row);
}
