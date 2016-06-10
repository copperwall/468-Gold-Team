#include "floppy.h"
#include "bufman.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

//initialize the buffer
int commence(char *database, Buffer *buf, int nBufferBlocks, int nCacheBlocks) {
   //check if file exists and creates it if necessary
   if(access(database, F_OK ) == -1) {
      chkerr(tfs_mkfs(database, nBufferBlocks * BLOCKSIZE));
   }

   chkerr(tfs_mount(database));
   buf->database = strdup(database);
   buf->nBufferBlocks = nBufferBlocks;
   buf->nCacheBlocks = nCacheBlocks;

   // Initialize all buffer and cache slots to be available.
   int i;
   for (i = 0; i < MAX_BUFFER_SIZE; i++) {
      buf->cache[i].isAvailable = 1;
      buf->cache[i].isVolatile = 1;
      buf->pages[i].isAvailable = 1;
      buf->pages[i].isVolatile = 0;
   }

   // Initialize table list to be empty.
   buf->tables = NULL;

   return SUCCESS;
}

//graciously end the work of the buffer
// Scott
int squash(Buffer * buf) {
   // Free the dynamic memory containing the database name
   free(buf->database);
   // Free the Buffer object itself.
   free(buf);
   return SUCCESS;
}

//read access to the disk page
int readPage(Buffer *buf, DiskAddress diskPage) {
   if (pageExistsInBuffer(buf, diskPage) == SUCCESS) {
//      printf("touching block\n");
      touchBlock(buf, diskPage);
   }
   else {
      //printf("page doesn't exist in buffer\n");
      chkerr(loadPage(buf, diskPage));
   }
   return SUCCESS;
}

//write access to the disk page
int writePage(Buffer *buf, DiskAddress diskPage) {
   int bufferIndex;
   chkerr(readPage(buf, diskPage));

   //set the page dirty bit since we are writing to this page
   chkerr(bufferIndex = getBufferIndex(buf, diskPage));
   buf->dirty[bufferIndex] = 1;

   return SUCCESS;
}

//flush the page from buffer to disk
int flushPage(Buffer *buf, DiskAddress diskPage) {
   int bufferIndex;
   chkerr(bufferIndex = getBufferIndex(buf, diskPage));

   tfs_writePage(diskPage.FD, diskPage.pageId, buf->pages[bufferIndex].block);

   buf->dirty[bufferIndex] = 0;

   return SUCCESS;
}

// Pin the page.
int pinPage(Buffer *buf, DiskAddress diskPage) {
   int bufferIndex = getBufferIndex(buf, diskPage);

   if (bufferIndex == ERROR) {
      return ERROR;
   }

   buf->pin[bufferIndex] = 1;
   return SUCCESS;
}

// Unpin the page.
int unPinPage(Buffer *buf, DiskAddress diskPage) {
   int bufferIndex = getBufferIndex(buf, diskPage);

   if (bufferIndex == ERROR) {
      return ERROR;
   }

   buf->pin[bufferIndex] = 0;
   return SUCCESS;
}

// Add a new disk page.
int newPage(Buffer *buf, fileDescriptor FD, DiskAddress * diskPage) {
   int newBufferIndex;
   Block *newBlock;

   // Check to see if any pages can be allocated in the buffer.
   chkerr(newBufferIndex = getLRUPage(buf));

   // If the lru block is dirty, flush it, otherwise just clobber it.
   if (buf->dirty[newBufferIndex]) {
      flushPage(buf, buf->pages[newBufferIndex].diskPage);
   }

   // Grab the next page in tinyFS
   diskPage->FD = FD;
   diskPage->pageId = tfs_numPages(FD) + 1;

   newBlock = &(buf->pages[newBufferIndex]);

   // Initialize new empty block
   memset(newBlock->block, 0, BLOCKSIZE);
   // Copy DiskAddress to new block
   memcpy(&(newBlock->diskPage), diskPage, sizeof(DiskAddress));
   // Flush new empty block to new disk page.
   flushPage(buf, *diskPage);

   return SUCCESS;
}

//Selects the next unpinned page to be replaced using the LRU algorithm
//return value: returns the pageId of the LRU page or an error code
int getLRUPage(Buffer *buf) {
   int i, replaceIndex;
   long minTimestamp = ULONG_MAX;

   // Look for available pages first.
   for (i = 0; i < buf->nBufferBlocks; i++) {
      if (buf->pages[i].isAvailable) {
         return i;
      }
   }

   // If no available, grab the lowest timestamp page.
   for (i = 0; i < buf->nBufferBlocks; i++) {
      if (buf->pin[i] == 0 && buf->timestamp[i] < minTimestamp) {
         minTimestamp = buf->timestamp[i];
         replaceIndex = i;
      }
   }

   // In this case all blocks are pinned
   if (minTimestamp == ULONG_MAX) {
      return ERROR;
   }

   return replaceIndex;
}

//Loads a page into the buffer, replacing another if necessary.
//IMPORTANT: after all pages are filled
//initially, no page is ever removed from the buffer.  Pages
//are simply swapped out if necessary.
int loadPage(Buffer *buf, DiskAddress diskPage) {
   int victimPage;

   chkerr(victimPage = getLRUPage(buf));
   //printf("lrupage is %d\n", victimPage);
   if (buf->dirty[victimPage]) {
      //printf("buf is dirty\n");
      chkerr(flushPage(buf, buf->pages[victimPage].diskPage));
   }

   //printf("reading page\n");
   tfs_readPage(diskPage.FD, diskPage.pageId,
      buf->pages[victimPage].block);
   buf->pages[victimPage].isAvailable = 0;
   touchBlock(buf, diskPage);
   return SUCCESS;
}

// Returns SUCCESS (0) if the page exists in the buffer.  Otherwise ERROR (-1).
int pageExistsInBuffer(Buffer *buf, DiskAddress diskPage) {
   int ndx;

   for (ndx = 0; ndx < buf->nBufferBlocks; ndx++) {
      if ((buf->pages[ndx].diskPage.FD == diskPage.FD) && (buf->pages[ndx].diskPage.pageId == diskPage.pageId)) {
         return SUCCESS;
      }
   }

   return ERROR;
}

// Returns SUCCESS (0) if the page exists in the cache.  Otherwise ERROR (-1).
int pageExistsInCache(Buffer *buf, DiskAddress diskPage) {
   int ndx = 0;

   for (ndx; ndx < MAX_BUFFER_SIZE; ndx++) {
      if (buf->cache[ndx].diskPage.FD == diskPage.FD && buf->cache[ndx].diskPage.pageId == diskPage.pageId) {
         return SUCCESS;
      }
   }

   return ERROR;
}

//returns the index of the page in the buffer
//returns an error code if the page does not exist in buffer
int getBufferIndex(Buffer *buf, DiskAddress diskPage) {
   int ndx;

   //printf("nBufferBlocks is %d\n", buf->nBufferBlocks);
   for (ndx = 0; ndx < buf->nBufferBlocks; ndx++) {
      if (buf->pages[ndx].isAvailable) {
         //printf("page is available\n");
         if (buf->pages[ndx].diskPage.FD == diskPage.FD
          && buf->pages[ndx].diskPage.pageId == diskPage.pageId) {
            //printf("GETBUFFERINDEX returning buffer index %d\n", ndx);
            return ndx;
         }
      } else {
         //printf("page is not available\n");
      }
   }

   //printf("getBufferIndex ERROR\n");
   return ERROR;
}

//returns the index of the page in the cache
//returns an error code if the page does not exist in cache
int getCacheIndex(Buffer *buf, DiskAddress diskPage) {
   int ndx;

   for (ndx = 0; ndx < buf->nCacheBlocks; ndx++) {
      if (buf->cache[ndx].isAvailable) {
         if (buf->cache[ndx].diskPage.FD == diskPage.FD
          && buf->cache[ndx].diskPage.pageId == diskPage.pageId) {
            return ndx;
         }
      }
   }

   return ERROR;
}

/**
 * This function is useful for when a volatile disk address is not found in
 * the volatile cache. We need to search the non-volatile buffer for any
 * volatile pages.
 */
int getCacheIndexFromBuffer(Buffer *buf, DiskAddress diskPage) {
   int ndx;
   Block *current;

   for (ndx = 0; ndx < buf->nBufferBlocks; ndx++) {
      current = &(buf->pages[ndx]);
      if (current->isVolatile && !current->isAvailable) {
         if (current->diskPage.FD == diskPage.FD && current->diskPage.pageId == diskPage.pageId) {
            return ndx;
         }
      }
   }

   return ERROR;
}

//Updates the page access timestamp
int touchBlock(Buffer *buf, DiskAddress diskPage) {
   int bufferIndex;

   chkerr(bufferIndex = getBufferIndex(buf, diskPage));
   buf->timestamp[bufferIndex] = time(NULL);

   return SUCCESS;
}

// print general information about current buffer
void checkpoint(Buffer *buf) {
   int i, slotsOccupied = 0;

   printf("Disk: %s\n", buf->database);
   for (i = 0; i < buf->nBufferBlocks; i++) {
      if (!buf->pages[i].isAvailable) {
         continue;
      }

      printf("Buffer Slot %d\n", i);
      printf("\tFD: %d\n", (buf->pages[i]).diskPage.FD);
      printf("\tPageid: %d\n", (buf->pages[i]).diskPage.pageId);
      printf("\tTimestamp: %ld\n", buf->timestamp[i]);
      printf("\tPin: %d\n", buf->pin[i]);
      printf("\tDirty page: %d\n\n", buf->dirty[i]);
      slotsOccupied++;
   }
   printf("Slots occupied: %d\n", slotsOccupied);
}

// print contents of page at the index of buffer
int pageDump(Buffer *buf, int index) {
   int i;
   if (buf->pages[index].diskPage.pageId) {
      Block page = buf->pages[index];
      for (i = 0; i < BLOCKSIZE; i++) {
         printf("%c ", page.block[i]);
      }
      printf("\n");
      return SUCCESS;
   }
   else
      return ERROR;
}

// prints from buffer with given diskaddress
int printPage(Buffer *buf, DiskAddress diskPage) {
   int i, ndx;
   if (readPage(buf, diskPage)) {
      if (pageExistsInBuffer(buf, diskPage) == SUCCESS) {
         ndx = getBufferIndex(buf, diskPage);
         return pageDump(buf, ndx);
      }
      else {
         printf("Page not in the buffer.\n");
         return ERROR;
      }
   }
   else {
      printf("Block Id does not exist on disk.\n");
      return ERROR;
   }
}

// prints from disk with given diskaddress
int printBlock(Buffer *buf, DiskAddress diskPage) {
   int i;
   char block[BLOCKSIZE];
   if (tfs_readPage(diskPage.FD, diskPage.pageId, block)) {
      for (i = 0; i < BLOCKSIZE; i++) {
         printf("%c ", block[i]);
      }
      printf("\n");
      return SUCCESS;
   }
   else
      return ERROR;
}

int getAvailableCachePage(Buffer *buf) {
   int i;

   for (i = 0; i < buf->nCacheBlocks; i++) {
      if (buf->cache[i].isAvailable) {
         return i;
      }
   }

   return ERROR;
}

// TODO: This isn't used, used it.
int getAvailableBufferPage(Buffer *buf) {
   int i;

   for (i = 0; i < buf->nBufferBlocks; i++) {
      if (buf->pages[i].isAvailable) {
         return i;
      }
   }

   return ERROR;
}

int allocateCachePage(Buffer *buf, DiskAddress diskPage) {
   // Lookup available page.
   int availCachePage = getAvailableCachePage(buf);
   int availBufferPage;
   int eviction;

   if (availCachePage < 0) {
      // pick cache page for eviction
      // random eviction strategy
      eviction = rand() % MAX_BUFFER_SIZE;

      // cpy block to persistent storage
      availBufferPage = getLRUPage(buf);

      // If the page is dirty, flush
      if (buf->dirty[availBufferPage]) {
         flushPage(buf, buf->pages[availBufferPage].diskPage);
      }

      memcpy(&buf->cache[availCachePage], &buf->pages[availBufferPage], sizeof(Block));

      buf->cache[eviction].isAvailable = 0;
      memcpy(&(buf->cache[eviction].diskPage), &diskPage, sizeof(DiskAddress));
   } else {
      buf->cache[availCachePage].diskPage = diskPage;
      buf->cache[availCachePage].isVolatile = 1;
      buf->cache[availCachePage].isAvailable = 0;
   }

   return SUCCESS;
}

/**
 * Remove a cache page.
 * Mark it as available, zero out its things.
 */

// TODO: Does this search the buffer? No. It should
// TODO: Make a function that searches non-volatile storage
int removeCachePage(Buffer *buf, DiskAddress diskPage) {
   int index;
   if ((index = getCacheIndex(buf, diskPage)) == ERROR) {
      // The page is not in volatile storage.
      // We have to search non-volatile storage to make sure it wasn't evicted.
      if ((index = getCacheIndexFromBuffer(buf, diskPage)) == ERROR) {
         return ERROR;
      }

      // Set available bit to 0 volatile bit is to 0
      buf->pages[index].isVolatile = 0;
      buf->pages[index].isAvailable = 1;

      return SUCCESS;
   }

   buf->cache[index].isAvailable = 1;
   // zero out the block and diskpage, too.
   memset(&(buf->cache[index].diskPage), 0, sizeof(DiskAddress));
   memset(&(buf->cache[index].block), 0, BLOCKSIZE);

   return SUCCESS;
}
