CC = gcc
FLAGS = -Wall -g
GCC = g++
CPPFLAGS=-O3 -std=c++0x -Wno-write-strings
FLOPPYSRC=parser/FLOPPY_statements/FLOPPYCreateTableStatement.cpp parser/FLOPPY_statements/FLOPPYCreateColumn.cpp parser/FLOPPY_statements/FLOPPYPrimaryKey.cpp parser/FLOPPY_statements/FLOPPYForeignKey.cpp parser/FLOPPY_statements/FLOPPYDropTableStatement.cpp parser/FLOPPY_statements/FLOPPYCreateIndexStatement.cpp parser/FLOPPY_statements/FLOPPYDropIndexStatement.cpp parser/FLOPPY_statements/FLOPPYInsertStatement.cpp parser/FLOPPY_statements/FLOPPYValue.cpp parser/FLOPPY_statements/FLOPPYDeleteStatement.cpp parser/FLOPPY_statements/FLOPPYNode.cpp parser/FLOPPY_statements/FLOPPYUpdateStatement.cpp parser/FLOPPY_statements/FLOPPYTableSpec.cpp parser/FLOPPY_statements/FLOPPYSelectItem.cpp parser/FLOPPY_statements/FLOPPYSelectStatement.cpp parser/FLOPPY_statements/FLOPPYTableAttribute.cpp
PARSERSRC=parser/lex_parse/FLOPPY_parser.cpp parser/lex_parse/FLOPPY_lexer.cpp

all: floppyobjects floppymain

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
	$(CC) $(CFLAGS) -o heaptest -DDEBUG bufman.c libDisk.o libTinyFS.o readwrite.c heap.c heap_test.c tables.c
awesometest: heap.c awesome_test.c
	$(CC) $(CFLAGS) -o awesometest -DDEBUG bufman.c libDisk.o libTinyFS.o readwrite.c heap.c awesome_test.c tables.c
floppyobjects:
	$(CC) $(CFLAGS) -g -O -c bufman.c libDisk.o libTinyFS.o readwrite.c heap.c heap_test.c tables.c
floppymain:
	$(GCC) $(CPPFLAGS) -o floppy floppy.cpp libDisk.o libTinyFS.o readwrite.o heap.o bufman.o tables.o $(FLOPPYSRC) $(PARSERSRC) parser/FLOPPYParser.cpp parser/FLOPPYOutput.cpp
clean:
	rm -f bufman harness
