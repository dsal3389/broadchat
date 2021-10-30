/*
this program uses ANSI escape codes, can read more about them
here: https://en.wikipedia.org/wiki/ANSI_escape_code

some terminals like the windows default terminal might not support
those ANSI codes and the program may look corrupted
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ansi.h"
#include "network.h"
#include "messages.h"
#include "logging.h"


struct s_message{
    char username[USERNAME_LEN];
    char message[PACKET_CONTENT_BUFFER - USERNAME_LEN];
};


static struct s_message messages_buffer[MESSAGES_BUFFER];
static unsigned int messages_len = 0;


void shiftMessage(){
    int i = 0;

    if(messages_len == 0){
        return;
    }
    
    // shift all elements 1 block to the left
    while(i != messages_len){
        memcpy(&messages_buffer[i], &messages_buffer[i + 1], sizeof(struct s_message));
        i++;
    }

    messages_len--;
    bzero(&messages_buffer[i], sizeof(struct s_message));
}

void pushMessage(char * restrict username, char * restrict message){
    struct s_message msg;

    if((messages_len + 1) >= MESSAGES_BUFFER){
        shiftMessage(); // delete first element to prevent overflow
    }

    bzero(&msg, sizeof(struct s_message));
    strncpy(msg.username, username, sizeof(msg.username) - 1);
    strncpy(msg.message,  message,  sizeof(msg.message) - 1);

    memcpy(&messages_buffer[messages_len++], &msg, sizeof(struct s_message));
    logging(DEBUG, "pushMessage called, sum of messages: %d\n", messages_len);
}

void printMessages(){
    struct winsize window;
    struct s_message * messageptr = messages_buffer;
    int i = 1; // skip 1 line for the input

    ioctl(1, TIOCGWINSZ, &window);

    if(messages_len > window.ws_row){
        messageptr = &messages_buffer[messages_len - window.ws_row + 1];
    }

    // save current position
    CURSOR_SAVE;
    MOVE_CURSOR_UP(1);
    CLEAN_LINE;
    CLEAN_ALL_TO_CURSOR;
    MOVE_CURSOR_UP(window.ws_row);

    while(i != window.ws_row){

        // more screen rows then messages
        if(i > messages_len){
            puts("~");
        } else {
            printf("[%s]: %s", messageptr -> username, messageptr -> message);
            messageptr++;
        }

        i++;
    }

    CURSOR_RET;
}
