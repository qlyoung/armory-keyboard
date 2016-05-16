CC=gcc
CFLAGS=-Wall -std=gnu11

type: src/type.c src/kybdutil.c src/kybdutil.h
	$(CC) $(CFLAGS) -o $@ $^
	rm -f *.o

clean:
	rm -f *.o type
