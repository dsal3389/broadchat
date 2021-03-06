#pragma once

#include <string.h>
#include <netinet/in.h>


#define MAX_SOCK_QUEUE 10 
#define MAX_CONNECTIONS 256
#define PACKET_CONTENT_BUFFER 512
#define PASSWORD_LEN 9
#define USERNAME_LEN 9

#define OPEN_TYPE 0x1
#define MESSAGE_TYPE 0x4
#define EMTPY_TYPE 0x8
#define NOTIFICATION_TYPE 0x80

// notification error types
#define PACKET_UNEXPECTED_TYPE 0x1
#define PACKET_INCCRECT_PASSWORD 0x2
#define PACKET_UNKNOWN_TYPE 0x4
#define MAX_CONNECTIONS_REACHED 0x8
#define NO_VALID_NAME 0x10

// returns the real packet size
#define CALC_PACKET_SIZE(pkt) ( sizeof(uint8_t) + sizeof(uint16_t) + (pkt -> datasize) )
#define PACKET_HEADER_SIZE (sizeof(struct packet) - PACKET_CONTENT_BUFFER)

#define NOTIFICATION_IF_FAIL(cond, sock, type, msg) ({ \
    if(cond) { \
        sendNotification(sock, type, msg, strlen(msg)); \
    } })

#define NOTIFICATION_RETURN_IF_FAIL(cond, sock, type, msg, ret) ({ \
    if(cond) { \
        sendNotification(sock, type, msg, strlen(msg)); \
        return ret; \
    } })

extern int sock;
extern short port;
extern char addr[16];

struct packet{
    uint8_t type;
    uint16_t datasize;
    uint8_t data[PACKET_CONTENT_BUFFER];
};

struct open_packet{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
};

struct message_packet{
    char username[USERNAME_LEN];
    char message[PACKET_CONTENT_BUFFER - USERNAME_LEN];
};

struct notification_packet{
    uint8_t err;
    char msg[PACKET_CONTENT_BUFFER - sizeof(uint8_t)];
};


int netrecv(int, uint8_t *, size_t);
void netsend(int, uint8_t *, size_t);
void createOpenPacket(struct packet *, const char * restrict, const char * restrict);
void createMessagePacket(struct packet *, const char * restrict, const char * restrict);
void createNotificationPacket(struct packet *, int, const char * restrict, size_t);
void createEmptyPacket(struct packet *);
void sendEmtpy(int);
void sendNotification(int, int, const char * restrict, size_t);
