#pragma once

// ANSI escape sequences code + code names
#define ESC "\033["
#define EL "K"
#define ED "J"
#define UP "A"
#define DN "B"
#define LT "D"
#define RT "C"
#define CSAVE "s"
#define CRESTORE "u"
#define SD "T"
#define SU "S"
#define DSR "6n"
#define HVP "f"
#define OSC "\x9D"
#define COLR "m"

#define CURSOR_POS(x, y) printf(ESC "%d;%d" HVP, x, y)
#define MOVE_CURSOR_UP(n) printf(ESC "%d" UP, n)
#define MOVE_CURSOR_DN(n) printf(ESC "%d" DN, n)
#define MOVE_CURSOR_LT(n) printf(ESC "%d" LT, n)
#define MOVE_CURSOR_RT(n) printf(ESC "%d" RT, n)
#define SCROLL_DOWN(n) printf(ESC "%d" SD, n)
#define SCROLL_UP(n) printf(ESC "%d" SU, n)
#define CLEAN_LINE printf(ESC "2" EL)
#define CLEAN_ALL printf(ESC "2" ED)
#define CLEAN_ALL_FROM_CURSOR printf(ESC ED)
#define CLEAN_ALL_TO_CURSOR printf(ESC "1" ED)
#define CURSOR_SAVE printf("\x1b%d", 7)
#define CURSOR_RET printf("\x1b%d", 8)
#define CURSOR_REP printf(ESC DSR)
#define SET_OSC printf(ESC OSC)
#define SET_COLOR_CODE(n) printf(ESC "%d" COLR, n) 


void setupTerminal();
void restoreTerminal();
void blankScreen();
