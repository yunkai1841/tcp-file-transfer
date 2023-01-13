CC = gcc
LIBS = util/msg.o util/filelist.o util/error.o
OPT = -O2
CFLAG =

all: server client

server: server.c $(LIBS)
	$(CC) $(OPT) -o server server.c $(LIBS)

$(LIBS): util/%.o: util/%.c
	$(CC) $(CFLAG) $(OPT) -c $< -o $@

client: client.c
	$(CC) $(OPT) -o client client.c $(LIBS)

.PHONY: clean
clean:
	rm -f server client $(LIBS)