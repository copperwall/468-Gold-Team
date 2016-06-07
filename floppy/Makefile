CC = gcc
FLAGS = -Wall -g

TFS468:
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/libTinyFS.h
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/libTinyFS.o
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/libDisk.h
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/libDisk.o
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/tinyFS.h
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/tests.c
	wget http://users.csc.calpoly.edu/~foaad/class/468/TFS468/README
tests: tests.c
	$(CC) $(CFLAGS) -o tests tests.c libDisk.o libTinyFS.o
bufman: bufman.c bufman.h libTinyFS.h tinyFS.h libDisk.o libTinyFS.o
	$(CC) $(CFLAGS) bufman.c bufman.h tinyFS.h libTinyFS.h libDisk.o libTinyFS.o
harness: bufman.c bufman.h libTinyFS.h tinyFS.h libDisk.o libTinyFS.o bufferTest.c
	$(CC) $(CFLAGS) -o harness bufman.c bufman.h tinyFS.h libTinyFS.h libDisk.o libTinyFS.o bufferTest.c
heaptest:
	$(CC) $(CFLAGS) -o heaptest -DDEBUG bufman.c libDisk.o libTinyFS.o readwrite.c heap.c heap_test.c
floppyobjects:
	$(CC) $(CFLAGS) -g -O -c bufman.c libDisk.o libTinyFS.o readwrite.c heap.c heap_test.c
floppymain:
	g++ -o floppy floppy.cpp libDisk.o libTinyFS.o readwrite.o heap.o bufman.o
clean:
	rm -f bufman harness
