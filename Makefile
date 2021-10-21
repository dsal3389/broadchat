

CC=gcc
C_FLAGS=-lpthread -Wall -O3


all: client server


client: network.o common.o logging.o screen.o client.o
	$(CC) $(C_FLAGS) network.o common.o logging.o screen.o client.o -o $@.out


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


screen.o:
	$(CC) $(C_FLAGS) -c src/screen.c


clean:
	rm -rf *.o *.out
