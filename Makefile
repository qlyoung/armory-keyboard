CC=gcc
CFLAGS=-Wall -std=gnu11

type: src/*
	$(CC) $(CFLAGS) -o $@ $^
	rm -f *.o

clean:
	rm -f *.o type
