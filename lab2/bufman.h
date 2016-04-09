#include "tinyFS.h"
#include "libTinyFS.h"
#include "tinyFS_errno.h"
#define MAX_BUFFER_SIZE 100
//macro checks if function returns an error and if it
//does then the error code is immediately returned
#define chkerr(err) if ((((int)(err)) < 0)) return err;

typedef struct { /* single disk block */
   char block[BLOCKSIZE]; /* block content */
   fileDescriptor FD; /* tinyFS file descriptor */
   int pageId; /* tinyFS page Id within the file */
} Block;

typedef struct {
   fileDescriptor FD; /* tinyFs file descriptor */
   int pageId; /* tinyFS page Id with the file */
} DiskAddress;

typedef struct { /* Main Memory Buffer */
   char *database; /* name of the disk file used with this buffer */
   int nBlocks; /* number of buffer slots */
   Block pages[MAX_BUFFER_SIZE]; /* the buffer itself. stores content */
   long timestamp[MAX_BUFFER_SIZE]; /* timestamp for LRU, FIFO and other eviction strategies */
   char pin[MAX_BUFFER_SIZE]; /* Pinned Page flags */
   char dirty[MAX_BUFFER_SIZE]; /* Dirty Page flags */
   int numOccupied; /* Number of occupied buffer slots */
} Buffer;

//initialize the buffer
int commence(char *Database, Buffer *buf, int nBlocks);

//graciously end the work of the buffer
int squash(Buffer *buf);

//read access to the disk page
int readPage(Buffer *buf, DiskAddress diskPage);

//write access to the disk page
int writePage(Buffer *buf, DiskAddress diskPage);

//flush the page from buffer to disk
int flushPage(Buffer *buf, DiskAddress diskPage);

//pin the page
int pinPage(Buffer *buf, DiskAddress diskPage);

//unpin the page
int unPinPage(Buffer *buf, DiskAddress diskPage);

//add a new disk page
int newPage(Buffer *buf, fileDescriptor FD, DiskAddress *diskPage);

//replaces the least recently used page with the specified page
int replaceLRU(DiskAddress diskPage);
