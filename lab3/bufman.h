#include "tinyFS.h"
#include "libTinyFS.h"
#include "tinyFS_errno.h"
#define MAX_BUFFER_SIZE 100
//macro checks if function returns an error and if it
//does then the error code is immediately returned
#define chkerr(err) if ((((int)(err)) < 0)) return err;

#define ALL_PAGES_PINNED -3

typedef struct {
   fileDescriptor FD; /* tinyFs file descriptor */
   int pageId; /* tinyFS page Id with the file */
} DiskAddress;

typedef struct { /* single disk block */
   char block[BLOCKSIZE]; /* block content */
   DiskAddress diskPage;
} Block;

typedef struct { /* Main Memory Buffer */
   char *database; /* name of the disk file used with this buffer */
   int nBufferBlocks; /* number of persistent buffer slots */
   int nCacheBlocks; /* number of volative buffer slots */
   Block pages[MAX_BUFFER_SIZE]; /* the persistent buffer itself. stores content */
   Block cache[MAX_BUFFER_SIZE]; /* the volatile buffer */
   long timestamp[MAX_BUFFER_SIZE]; /* timestamp for LRU, FIFO and other eviction strategies */
   char pin[MAX_BUFFER_SIZE]; /* Pinned Page flags */
   char dirty[MAX_BUFFER_SIZE]; /* Dirty Page flags */
   int numBufferOccupied; /* Number of occupied persistent buffer slots */
   int numCacheOccupied; /* Number of occupied volatile buffer slots */
} Buffer;

//initialize the buffer
int commence(char *Database, Buffer *buf, int nBufferBlocks, int nCacheBlocks);

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

//Selects the next page to be replaced using the LRU algorithm
//return value: returns the pageId of the LRU page or an error code
int getLRUPage(Buffer *buf);

//Loads a page into the buffer, replacing another if necessary
int loadPage(Buffer *buf, DiskAddress diskPage);


//return 1 if the page exists in the buffer.  Otherwise 0.
int pageExistsInBuffer(Buffer *buf, DiskAddress diskPage);

//Updates the page access timestamp
int touchBlock(Buffer *buf, DiskAddress diskPage);


//returns the index of the page in the buffer
//returns an error code if the page does not exist in buffer
int getBufferIndex(Buffer *buf, DiskAddress diskPage);
   
void checkpoint(Buffer *buf);
