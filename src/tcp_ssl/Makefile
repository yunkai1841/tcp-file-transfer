CC = gcc
LIBS = -lssl -lcrypto
OPT = -O2
CFLAG = 

all: server client

server: server.c
	$(CC) $(OPT) -o server server.c $(LIBS)

client: client.c
	$(CC) $(OPT) -o client client.c $(LIBS)

clean:
	rm -f server client