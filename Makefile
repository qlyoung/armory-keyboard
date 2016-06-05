CC=gcc
CFLAGS=-Wall -std=gnu11 -g

sourcedir   = src
testdir     = tests
builddir    = build
includedir  = include

type: $(sourcedir)/*
	$(CC) $(CFLAGS) -I $(includedir) -o $(builddir)/$@ $^
	rm -f *.o

test: $(testdir)/* $(sourcedir)/*
	@$(CC) $(CFLAGS) -I $(includedir) -c $(testdir)/*.c
	@$(CC) $(CFLAGS) -I $(includedir) -DTESTING -c $(sourcedir)/*.c
	@$(CC) $(CFLAGS) -o $(builddir)/$@ ./*.o
	@rm -rf *.o
	@cp $(testdir)/test.layout $(builddir)/
	@cd $(builddir); ./test

clean:
	rm -f *.o $(builddir)/type $(builddir)/test
