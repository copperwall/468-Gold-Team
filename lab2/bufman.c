#include "bufman.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//initialize the buffer
int commence(char *database, Buffer *buf, int nBlocks) {
   //check if file exists and creates it if necessary
   if(access(database, F_OK ) == -1 ) {
      chkerr(tfs_mkfs(database, nBlocks * BLOCKSIZE));
   }

   chkerr(tfs_mount(database));
   buf->database = strdup(database);
   buf->nBlocks = nBlocks;
   
   return SUCCESS;
}

//graciously end the work of the buffer
int squash(Buffer * buf) {
   free(buf->database);
   return SUCCESS;
}

//read access to the disk page
int readPage(Buffer * buf, DiskAddress diskPage) {
   if(pageExistsInBuffer(buf, diskPage)) {
      touchBlock(buf, diskPage);
   }
   else {
      chkerr(loadPage(buf, diskPage));
   }
   return SUCCESS;
}

//write access to the disk page
int writePage(Buffer *buf, DiskAddress diskPage) {
   int bufferIndex;
   if(pageExistsInBuffer(buf, diskPage)) {
      touchBlock(buf, diskPage);
   }
   else {
      chkerr(loadPage(buf, diskPage));
   }

   //set the page dirty bit since we are writing to this page
   chkerr(bufferIndex = getBufferIndex(buf, diskPage));
   buf->dirty[bufferIndex] = 1;

   return SUCCESS;
}

//flush the page from buffer to disk
int flushPage(Buffer *buf, DiskAddress diskPage) {
   return SUCCESS;
}

//pin the page
int pinPage(Buffer *buf, DiskAddress diskPage) {
   return SUCCESS;
}

//unpin the page
int unPinPage(Buffer *buf, DiskAddress diskPage) {
   return SUCCESS;
}

//add a new disk page
int newPage(Buffer *buf, fileDescriptor FD, DiskAddress * diskPage) {
   return SUCCESS;
}

//Selects the next unpinned page to be replaced using the LRU algorithm
//return value: returns the pageId of the LRU page or an error code
int getLRUPage(Buffer *buf) {
   return SUCCESS;
}

//Loads a page into the buffer, replacing another if necessary
int loadPage(Buffer *buf, DiskAddress diskPage) {
   int victimPage;
   if(buf->numOccupied < buf->nBlocks){
      tfs_readPage(diskPage.FD, diskPage.pageId,
            buf->pages[buf->numOccupied++].block);

   } else {
      chkerr(victimPage = getLRUPage(buf));
      if (buf->dirty[victimPage]) {
         chkerr(flushPage(buf, buf->pages[victimPage].diskPage));
      }
      
      tfs_readPage(diskPage.FD, diskPage.pageId, 
         buf->pages[victimPage].block);
   }
   touchBlock(buf, diskPage);
   return SUCCESS;
}

//return 1 if the page exists in the buffer.  Otherwise 0.
int pageExistsInBuffer(Buffer *buf, DiskAddress diskPage) {
   return SUCCESS;
}

//returns the index of the page in the buffer
//returns an error code if the page does not exist in buffer
int getBufferIndex(Buffer *buf, DiskAddress diskPage) {
   return SUCCESS;
}

//Updates the page access timestamp
int touchBlock(Buffer *buf, DiskAddress diskPage) {
   return SUCCESS;
}

int main() {
   Buffer buf = {}; //inializes all fields to 0

   chkerr(commence(DEFAULT_DISK_NAME, &buf, MAX_BUFFER_SIZE));

   chkerr(squash(&buf));
   return SUCCESS;
}
