CC=gcc
CFLAGS=-Wall -std=gnu11 -g

sourcedir = src
builddir  = build

type: $(sourcedir)/*
	$(CC) $(CFLAGS) -o $(builddir)/$@ $^
	rm -f *.o

clean:
	rm -f *.o $(builddir)/type
