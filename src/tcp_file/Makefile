CC = gcc
LIBS =
OPT = -O2
CFLAG =

all: server client

server: server.c
	$(CC) $(OPT) -o server server.c

client: client.c
	$(CC) $(OPT) -o client client.c

.PHONY: clean
clean:
	rm -f server client received.png