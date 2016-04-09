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
   return SUCCESS;
}

//write access to the disk page
int writePage(Buffer *buf, DiskAddress diskPage) {
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


//replaces the least recently used page with the specified page
int replaceLRU(DiskAddress diskPage) {
   return SUCCESS;
}

int main() {
   Buffer buf = {}; //inializes all fields to 0

   chkerr(commence(DEFAULT_DISK_NAME, &buf, MAX_BUFFER_SIZE));

   chkerr(squash(&buf));
   
   return SUCCESS;
}
