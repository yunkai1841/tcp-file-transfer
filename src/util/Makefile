CC = gcc
LIBS = filelist.o
OPT = -O2
CFLAG =

all: $(LIBS)

filelist.o: filelist.c
	$(CC) $(CFLAG) $(OPT) -c filelist.c -o filelist.o

test: test_filelist
	./test_filelist .

test_filelist: filelist.o
	$(CC) $(CFLAG) $(OPT) -o test_filelist test_filelist.c $(LIBS)

.PHONY: clean
clean:
	rm -f *.o test_filelist