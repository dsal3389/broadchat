#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "network.h"


int sock;
short port = 5100;
char addr[16] = "127.0.0.1";


int netrecv(int sock, uint8_t * buff, size_t size){
    return recv(sock, buff, size, 0);
}

void netsend(int sock, uint8_t * data, size_t size){
    int bytessent = 0;

    while(bytessent != size){
        bytessent += send(sock, &data[bytessent], size - bytessent, 0);
    }
}

void create_open_packet(struct packet * buff, const char * restrict username, const char * restrict password){
    struct open_packet opnpkt;

    bzero(&opnpkt, sizeof(struct open_packet));

    strncpy(opnpkt.username, username, sizeof(opnpkt.username) - 1);
    strncpy(opnpkt.password, password, sizeof(opnpkt.password) - 1); // -1 for the null terminator

    buff -> type = OPEN_TYPE;
    buff -> datasize = sizeof(struct open_packet);
    memcpy(buff -> data, (char *) &opnpkt, sizeof(struct open_packet));
}

void create_message_packet(struct packet * buff, const char * restrict username, const char * restrict message){
    struct message_packet msgpkt;

    bzero(&msgpkt, sizeof(struct message_packet));

    buff -> type = MESSAGE_TYPE;
    buff -> datasize = sizeof(struct message_packet);
 
    if(username != NULL){
        strncpy(msgpkt.username, username, sizeof(msgpkt.username) - 1);
    }

    strncpy(msgpkt.message,  message,  sizeof(msgpkt.message)  - 1);
    memcpy(buff -> data, &msgpkt, sizeof(struct message_packet));
}

void create_empty_packet(struct packet * buff){
    buff -> type = EMTPY_TYPE;
    buff -> datasize = 0;
    memset(buff -> data, 0, 1);
}

void create_notification_packet(struct packet * buff, int type, const char * restrict msg, size_t msgsize){
    struct notification_packet notpkt;

    bzero(&notpkt, sizeof(struct notification_packet));

    notpkt.err = type;
    buff -> type = NOTIFICATION_TYPE;
    buff -> datasize = sizeof(struct notification_packet);
    
    strncpy(notpkt.msg, msg, msgsize);
    memcpy(buff -> data, (char *) &notpkt, sizeof(struct notification_packet));
}

void sendemtpy(int sock){
    struct packet pkt;
    struct packet * ppkt = &pkt;

    create_empty_packet(ppkt);
    netsend(sock, (uint8_t *) ppkt, CALC_PACKET_SIZE(ppkt));
}

void sendnotification(int sock, int type, const char * restrict msg, size_t msgsize){
    struct packet pkt;
    struct packet * ppkt = &pkt;

    create_notification_packet(ppkt, type, msg, msgsize);
    netsend(sock, (uint8_t *) ppkt, CALC_PACKET_SIZE(ppkt));
}
