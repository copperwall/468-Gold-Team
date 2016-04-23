CC = gcc
FLAGS = -Wall -g

TFS468:	
	cp //home/foaad/www/class/468/TFS468/libDisk.o //home/foaad/www/class/468/TFS468/libTinyFS.o //home/foaad/www/class/468/TFS468/*.h .
tests: tests.c 
	$(CC) $(CFLAGS) -o tests tests.c libDisk.o libTinyFS.o
bufman: bufman.c bufman.h libTinyFS.h tinyFS.h libDisk.o libTinyFS.o
	$(CC) $(CFLAGS) -o bufman bufman.c bufman.h tinyFS.h libTinyFS.h libDisk.o libTinyFS.o
harness: bufman.c bufman.h libTinyFS.h tinyFS.h libDisk.o libTinyFS.o bufferTest.c
	$(CC) $(CFLAGS) -o harness bufman.c bufman.h tinyFS.h libTinyFS.h libDisk.o libTinyFS.o bufferTest.c
clean:
	rm bufman tests
