#include "bufman.h"



//initialize the buffer
int commence(char * Database, Buffer * buf, int nBlocks) {
   return 0;
}

//graciously end the work of the buffer
int squash(Buffer * buf) {
   return 0;
}

//read access to the disk page
int readPage(Buffer * buf, DiskAddress diskPage) {
   return 0;
}

//write access to the disk page
int writePage(Buffer *buf, DiskAddress diskPage) {
   return 0;
}

//flush the page from buffer to disk
int flushPage(Buffer *buf, DiskAddress diskPage) {
   return 0;
}

//pin the page
int pinPage(Buffer *buf, DiskAddress diskPage) {
   return 0;
}

//unpin the page
int unPinPage(Buffer *buf, DiskAddress diskPage) {
   return 0;
}

//add a new disk page
int newPage(Buffer *buf, fileDescriptor FD, DiskAddress * diskPage) {
   return 0;
}

int main() {

   return 0;
}
