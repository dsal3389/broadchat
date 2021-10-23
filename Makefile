

CC=gcc
C_FLAGS=-lpthread -Wall -O3


all: client server


client: network.o common.o logging.o messages.o ansi.o client.o
	$(CC) $(C_FLAGS) network.o common.o logging.o messages.o ansi.o client.o -o $@.out


server: network.o common.o logging.o server.o 
	$(CC) $(C_FLAGS) network.o common.o logging.o server.o -o $@.out


client.o:
	$(CC) $(C_FLAGS) -c src/main.c -o $@


server.o:
	$(CC) $(C_FLAGS) -c -D__SERVER src/main.c -o $@


network.o:
	$(CC) $(C_FLAGS) -c src/network.c


common.o:
	$(CC) $(C_FLAGS) -c src/common.c


logging.o:
	$(CC) $(C_FLAGS) -c src/logging.c


messages.o:
	$(CC) $(C_FLAGS) -c src/messages.c


ansi.o:
	$(CC) $(C_FLAGS) -c src/ansi.c


clean:
	rm -rf *.o *.out
