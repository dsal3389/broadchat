#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // only for inet_addr

#include "common.h"
#include "network.h"
#include "logging.h"

#ifndef __SERVER
    #include "messages.h"
#endif


extern char addr[16];
extern short port;
extern int sock;
extern int logginglevel;

// server password, username is in no use by the server
static char password[PASSWORD_LEN] = "\0";
#ifdef __SERVER
    static char username[USERNAME_LEN] = "server";
#else
    static char username[USERNAME_LEN] = "unknown";
#endif


void printhelp(){
    printf(
        "a simple chat written in C\n"
        "usage: prog <flag> [value]\n\n"
        "-s, --host <addres:%s>\t set the server ip or the client connection ip\n"
        "-p, --port <port:%d>\t set the server port or the client connection port\n"
        "-l, --logging <int:%d>\t set the logging level\n"
        "-u, --username <name:max %d>\t set the server name for outgoing messages, set the client username\n"
        "--password <password:max %d>\t set the server password or the client connection password\n"
        , addr, port, logginglevel, USERNAME_LEN - 1, PASSWORD_LEN - 1
    );
    exit(EXIT_SUCCESS);
}

void parseArgs(int argc, char ** argv){
    char ** ptr = argv;

    // last element ptr - current ptr
    while((&argv[argc] - ptr) > 0){

        if(strncmp(*ptr, "-h", 2) == 0 || strncmp(*ptr, "--help", 6) == 0){
            printhelp(); // will exit the program
        }
        else if(strncmp(*ptr, "-s", 2) == 0 || strncmp(*ptr, "--host", 6) == 0){
            strncpy(addr, *(++ptr), sizeof(addr) - 1);
        }
        else if(strncmp(*ptr, "-p", 2) == 0 || strncmp(*ptr, "--port", 6) == 0){
            port = atoi(*(++ptr));
        }
        else if(strncmp(*ptr, "-l", 2) == 0 || strncmp(*ptr, "--logging", 9) == 0){
            logginglevel = atoi(*(++ptr));
        }
        else if(strncmp(*ptr, "-u", 2) == 0 || strncmp(*ptr, "--username", 10) == 0){
            strncpy(username, *(++ptr), USERNAME_LEN - 1);
        }
        else if (strncmp(*ptr, "--password", 10) == 0){
            strncpy(password, *(++ptr), PASSWORD_LEN - 1);
        }

        ptr++;
    }
}

#ifdef __SERVER
    struct connection{
        int sock;
        struct sockaddr_in addr;
        char username[USERNAME_LEN];
        unsigned int _position; // for the connections array
    };

    struct handle_connection_args{
        int sock;
        struct sockaddr_in addr;
    }; // passed by a thread to the handle_connection function

    struct connection * connections[MAX_CONNECTIONS];
    unsigned int connlen = 0;


    void addClient(struct connection * conn){
        conn -> _position = connlen;
        connections[connlen++] = conn;
    }

    void removeClient(struct connection * conn){
        struct connection ** ptr = &connections[connlen - 1]; // get the last element
        struct connection ** toreplace = &connections[conn -> _position];

        memcpy(toreplace, *ptr, sizeof(struct connection));
        bzero(ptr, sizeof(struct connection));
        connlen--;

        free(*toreplace);
    }

    void broadcast(struct packet * pkt){
        size_t pktsize = CALC_PACKET_SIZE(pkt);
        int i = 0;

        for(; i < connlen; i++){
            netsend(connections[i] -> sock, (uint8_t *) pkt, pktsize);
        }
    }

    void serverAddClient(struct connection * conn){
        struct packet pkt;
        char message[PACKET_CONTENT_BUFFER - USERNAME_LEN];

        snprintf(message, sizeof(message), "new client joined with the name [%s]\n", conn -> username);
        createMessagePacket(&pkt, username, message);

        addClient(conn);
        broadcast(&pkt);
    }
    
    void serverRemoveClient(struct connection * conn){
        struct packet pkt;
        char message[PACKET_CONTENT_BUFFER - USERNAME_LEN];

        snprintf(message, sizeof(message), "[%s] left\n", conn -> username);
        createMessagePacket(&pkt, username, message);

        removeClient(conn);
        broadcast(&pkt);
    }

    void processPacket(struct connection * conn, struct packet * pkt){
        switch(pkt -> type){
            case MESSAGE_TYPE:
                // copy the connection username to the packet username field
                strncpy(((struct message_packet *) pkt -> data) -> username, conn -> username, USERNAME_LEN);
                broadcast(pkt);
                break;
            case EMTPY_TYPE:
                break;
            default:
                sendNotification(conn -> sock, PACKET_UNKNOWN_TYPE, "unknown packet type recved\n", 28);
                break;
        }
    }

    int handshake(struct connection * conn){
        struct packet pkt;
        struct open_packet * ptr = (struct open_packet *) &(pkt.data);

        if(netrecv(conn -> sock, (uint8_t *) &pkt, sizeof(struct packet)) < 0){
            return -1;
        }

        NOTIFICATION_RETURN_IF_FAIL(
            pkt.type != OPEN_TYPE,
            conn -> sock, PACKET_UNEXPECTED_TYPE, "expected OPEN_TYPE packet\n", -1
        );

        NOTIFICATION_RETURN_IF_FAIL(
            strncmp(ptr -> password, password, PASSWORD_LEN) != 0,
            conn -> sock, PACKET_INCCRECT_PASSWORD, "inccorect password\n", -1
        );

        strncpy(conn -> username, ptr -> username, USERNAME_LEN);

        NOTIFICATION_RETURN_IF_FAIL(
            strncmp(conn -> username, username, USERNAME_LEN) == 0,
            conn -> sock, NO_VALID_NAME, "given username is used by server\n", -1
        );

        sendEmtpy(conn -> sock);
        return 1;
    }

    void listenClient(struct connection * conn){
        struct packet pkt;

        while(true){
            bzero(&pkt, sizeof(struct packet));

            // break the listen if client disconnects or there is an error
            if(netrecv(conn -> sock, (uint8_t *) &pkt, sizeof(pkt)) <= 0){
                return;
            }

            logging(INFO, "(socket: %d) type %d | datasize %d\n", conn -> sock, pkt.type, pkt.datasize);
            processPacket(conn, &pkt);
        }
    }

    void handleConnection(void * args){
        struct handle_connection_args * realargs = args;
        struct connection conn;

        NOTIFICATION_RETURN_IF_FAIL(
            (connlen + 1) > MAX_CONNECTIONS,
            realargs -> sock, MAX_CONNECTIONS_REACHED, "max connections reached", 
        );

        bzero(&conn, sizeof(struct connection));

        conn.sock = realargs -> sock;
        memcpy(&conn.addr, &(realargs -> addr), sizeof(struct sockaddr_in));
        free(args);

        if(handshake(&conn) == -1){
            pthread_exit(0);
        }

        logging(INFO, "client handshake with the username of %s\n", conn.username);

        serverAddClient(&conn);
        listenClient(&conn); // a block function, returnes if client left of error eccured 
        serverRemoveClient(&conn);
    }

    void serverListen(){
        struct handle_connection_args * handler_args;
        struct sockaddr_in caddr;
        pthread_t tid; // for the pthread_create, not really using this variable
        int csock; // client socket
        socklen_t caddrsize = sizeof(caddr);

        IF_FAIL(
            (listen(sock, MAX_SOCK_QUEUE) == -1),
            "could not listen to socket", NULL
        );

        while(true){
            handler_args = (struct handle_connection_args *) \
                    hallocate(1, sizeof(struct handle_connection_args));

            csock = accept(sock, (struct sockaddr *) &caddr, &caddrsize);

            handler_args -> sock = csock;
            handler_args -> addr = caddr;

            pthread_create(&tid, NULL, (void *) handleConnection, handler_args);
        }
    }
#else 
    void processPacket(struct packet * pkt){
        switch(pkt -> type){
            case MESSAGE_TYPE:
                struct message_packet * msgpkt =  (struct message_packet *) pkt -> data;

                pushMessage(msgpkt -> username, msgpkt -> message);
                printMessages();
                break;

            default:
                break;
        }
    }

    /* 
    changes the string it self, returns a pointer that tells from where to start read,
    and set a null terminal that tells when to end
    */
    char * strip(char * str, char ch, size_t size){
        char * ptr = str;
        char * end = &str[size - 1]; // last element

        // strip left side and right side
        for(;*ptr == ch; ptr++);
        for(;*end == ch; end--);
        memset(++end, 0, &str[size] - end);

        return ptr;
    }

    int handshake(){
        struct packet pkt;
        struct packet * ppkt = &pkt; 

        createOpenPacket(ppkt, username, password);
        netsend(sock, (uint8_t *) ppkt, CALC_PACKET_SIZE(ppkt));

        if(netrecv(sock, (uint8_t *) ppkt, sizeof(struct packet)) < 0){
            return -1;
        }

        if(pkt.type & NOTIFICATION_TYPE){
            fprintf(stderr, ((struct notification_packet *) pkt.data) -> msg);
            return -1;
        }

        return 1;
    }

    void clientListen(){
        struct packet pkt;

        while(true){
            
            if(netrecv(sock, (uint8_t *) &pkt, sizeof(struct packet)) <= 0){
                return;
            }

            processPacket(&pkt);
            bzero(&pkt, sizeof(struct packet));
        }
    }

    void clientShell(){
        struct packet pkt;
        struct packet * ppkt = &pkt;
        char message[PACKET_CONTENT_BUFFER - USERNAME_LEN - 1] = "";
        char * stripped = NULL;

        while(true){
            printf(">>> ");
            fgets(message, sizeof(message), stdin);

            stripped = strip(message, ' ', strlen(message));

            if(*stripped != '\n' && *stripped != 0){
                createMessagePacket(ppkt, NULL, stripped);
                netsend(sock, (uint8_t *) ppkt, CALC_PACKET_SIZE(ppkt));
            } else {
                printMessages(); // to delete the added line
            }

            fflush(stdin);
            bzero(message, sizeof(message));
        }
    }

    void start(){
        pthread_t tid;

        IF_FAIL(
            handshake() == -1,
            "handshake failed\n", NULL
        );
        
        pthread_create(&tid, NULL, (void *) clientListen, NULL);
        clientShell();
    }
#endif


int main(int argc, char * argv[]){
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;

    atexit(cleanup);

    if(argc > 1){
        parseArgs(argc - 1, &argv[1]); // skip argv[0]
    }

    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(addr);

    IF_FAIL(
        ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1),
        "could not create socket", NULL
    );

#ifdef __SERVER
    IF_FAIL(
        (bind(sock, (struct sockaddr *) &saddr, sizeof(saddr)) == -1),
        "could not bind address and port on socket (%s:%hu)", addr, port
    );
    
    serverListen();
#else
    IF_FAIL(
        connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)),
        "could not connect to server (%s:%hu)", addr, port
    );

    start();
#endif
}

