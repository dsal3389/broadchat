#include <stdio.h>

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
