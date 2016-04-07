#include "tinyFS.h"
#include "libTinyFS.h"
#define BUFFER_SIZE 100

typedef struct { /* single disk block */
   char[BLOCKSIZE] block; /* block content */
   fileDescriptor FD; /* tinyFS file descriptor */
   int pageId; /* tinyFS page Id within the file */
} Block;

typedef struct {
   fileDescriptor FD; /* tinyFs file descriptor */
   int pageId; /* tinyFS page Id with the file */
} DiskAddress;

typedef struct { /* Main Memory Buffer */
   char * database; /* name of the disk file used with this buffer */
   int nBlocks; /* number of buffer slots */
   Block[BUFFER_SIZE] pages; /* the buffer itself. stores content */
   long[BUFFER_SIZE] timestamp; /* timestamp for LRU, FIFO and other eviction strategies */
   char[BUFFER_SIZE] pin; /* Pinned Page flags */
   char[BUFFER_SIZE] dirty; /* Dirty Page flags */
   int numOccupied; /* Number of occupied buffer slots */
} Buffer;

//initialize the buffer
int commence(char * Database, Buffer * buf, int nBlocks);

//graciously end the work of the buffer
int squash(Buffer * buf);

//read access to the disk page
int readPage(Buffer * buf, DiskAddress diskPage);

//write access to the disk page
int writePage(Buffer *buf, DiskAddress diskPage);

//flush the page from buffer to disk
int flushPage(Buffer *buf, DiskAddress diskPage);

//pin the page
int pinPage(Buffer *buf, DiskAddress diskPage);

//unpin the page
int unPinPage(Buffer *buf, DiskAddress diskPage);

//add a new disk page
int newPage(Buffer *buf, fileDescriptor FD, DiskAddress * diskPage);