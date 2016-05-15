#include <string.h>
#include <stdlib.h>
#include "floppy.h"
#include "bufman.h"
#include "readwrite.h"

/**
 * Begin readwrite functions
 */

// Search volatile buffer and non-volatile buffer for diskAddress
// TODO: If the page isn't in the buffer, should we attempt to read it from
// disk? How do we know if that pageid "exists" yet? Check the number of pages
// in the file? I feel like this problem in more in the scope of readPage()
int getPage(Buffer *buf, DiskAddress page, char *out) {
   int index;

   if ((index = getCacheIndex(buf, page)) == ERROR) {
      if ((index = getBufferIndex(buf, page)) == ERROR) {
         return ERROR;
      }

      memcpy(out, buf->pages[index].block, BLOCKSIZE);
      return SUCCESS;
   }

   memcpy(out, buf->cache[index].block, BLOCKSIZE);
   return SUCCESS;
}

// Need to check volatile storage
// persistent storage and check for isVolatile
int getVolatilePage(Buffer *buf, DiskAddress page, char *out) {
   int index;

   if ((index = getCacheIndex(buf, page)) == ERROR) {
      if ((index = getBufferIndex(buf, page)) == ERROR) {
         return ERROR;
      }

      // Make sure this page is volatile.
      if (!buf->pages[index].isVolatile) {
         return ERROR;
      }

      memcpy(out, buf->pages[index].block, BLOCKSIZE);
      return SUCCESS;
   }

   memcpy(out, buf->cache[index].block, BLOCKSIZE);
   return SUCCESS;
}

int putPage(Buffer *buf, DiskAddress page, char *data, int dataSize) {
   int index;

   if (dataSize < BLOCKSIZE) {
      return ERROR;
   }

   if ((index = getCacheIndex(buf, page)) == ERROR) {
      if ((index = getBufferIndex(buf, page)) == ERROR) {
         // TODO: Make new page
         // UNIMPLEMENTED
         return ERROR;
      }

      memcpy(buf->pages[index].block, data, BLOCKSIZE);
      writePage(buf, page);
   } else {
      memcpy(buf->cache[index].block, data, BLOCKSIZE);
   }

   return SUCCESS;
}

// Check volatile storage
int putVolatilePage(Buffer *buf, DiskAddress page, char *data, int dataSize) {
   int index;

   if (dataSize < BLOCKSIZE) {
      return ERROR;
   }

   if ((index = getCacheIndex(buf, page)) == ERROR) {
      if ((index = getBufferIndex(buf, page)) == ERROR) {
         // TODO: Make new page
         // UNIMPLEMENTED
         return ERROR;
      }

      // Make sure this page is volatile.
      if (!buf->pages[index].isVolatile) {
         return ERROR;
      }

      memcpy(buf->pages[index].block, data, BLOCKSIZE);
      writePage(buf, page);
   } else {
      memcpy(buf->cache[index].block, data, BLOCKSIZE);
   }

   return SUCCESS;

}

int read(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *out) {
   char block[BLOCKSIZE];
   int result = getPage(buf, page, block);

   if (result == ERROR || startOffset >= BLOCKSIZE) {
      return ERROR;
   }

   memcpy(out, block + startOffset, nBytes);
   return SUCCESS;
}

int write(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *data, int dataSize) {
   char block[BLOCKSIZE];
   int result;

   if (startOffset >= BLOCKSIZE) {
      return ERROR;
   }

   result = getPage(buf, page, block);

   if (result == ERROR) {
      // TODO: NEW PAGE
      // UNIMPLEMENTED
      return ERROR;
   }

   if (startOffset + nBytes >= BLOCKSIZE) {
      return ERROR;
   }

   if (dataSize < nBytes) {
      // write |dataSize| bytes, pad the rest up to nBytes with 0
      memcpy(block + startOffset, data, nBytes);
   } else {
      memcpy(block + startOffset, data, nBytes);
   }

   putPage(buf, page, block, BLOCKSIZE);
   return SUCCESS;
}

int readVolatile(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *out) {
   char block[BLOCKSIZE];
   int result = getVolatilePage(buf, page, block);

   if (result == ERROR || startOffset >= BLOCKSIZE) {
      return ERROR;
   }

   memcpy(out, block + startOffset, nBytes);
   return SUCCESS;
}

int writeVolatile(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *data, int dataSize) {
   char block[BLOCKSIZE];
   int result;

   if (startOffset >= BLOCKSIZE) {
      return ERROR;
   }

   result = getVolatilePage(buf, page, block);

   if (result == ERROR) {
      // TODO: NEW PAGE
      // UNIMPLEMENTED
      return ERROR;
   }

   if (startOffset + nBytes >= BLOCKSIZE) {
      return ERROR;
   }

   if (dataSize < nBytes) {
      // write |dataSize| bytes, pad the rest up to nBytes with 0
      memcpy(block + startOffset, data, nBytes);
   } else {
      memcpy(block + startOffset, data, nBytes);
   }

   putPage(buf, page, block, BLOCKSIZE);
   return SUCCESS;
}
