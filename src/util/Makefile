CC = gcc
LIBS = filelist.o error.o
OPT = -O2
CFLAG =

all: $(LIBS)

filelist.o: filelist.c
	$(CC) $(CFLAG) $(OPT) -c filelist.c -o filelist.o

error.o: error.c
	$(CC) $(CFLAG) $(OPT) -c error.c -o error.o

test: test_filelist test_error
	./test_filelist .
	./test_error

test_filelist: filelist.o
	$(CC) $(CFLAG) $(OPT) -o test_filelist test_filelist.c $(LIBS)

test_error: error.o
	$(CC) $(CFLAG) $(OPT) -o test_error test_error.c $(LIBS)

.PHONY: clean
clean:
	rm -f *.o test_filelist test_error