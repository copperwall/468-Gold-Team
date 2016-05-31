#include <string.h>
#include "floppy.h"
#include "bufman.h"
#include "readwrite.h"
#include "tables.h"
#include "sequential.h"

// This follows teh same process as it's counterpart in heap.c, largely the same
int createSequentialFile(Buffer *buf, char *tablename, tableDescription createTable, int volatileFlag) {
	SeqFileHeader fileHeader;
	DiskAddress diskPage;

	char recordDescription[2048];
	char headerPage[BLOCKSIZE];
	int recordSize, fd;

	fileHeader.page_list = 1;
	strncpy(fileHeader.table_name, tablename, MAX_TABLENAME_SIZE);

	fileHeader.record_desc_size = generateRecordDescription(createTable, recordDescription, &recordSize);
   fileHeader.record_size = recordSize;
   // TODO: fileHeader.lastPage
   memcpy(headerPage, &fileHeader, sizeof(SeqFileHeader));
   memcpy(headerPage + sizeof(SeqFileHeader), recordDescription, fileHeader.record_desc_size);

   diskPage.FD = tfs_openFile(createTable.tableName);
   diskPage.pageId = 0;

   if (!createTable.isVolatile) {
      putPage(buf, diskPage, headerPage, BLOCKSIZE);
   } else {
      putVolatilePage(buf, diskPage, headerPage, BLOCKSIZE);
   }

   flushPage(buf, diskPage);

   return SUCCESS;
}


// This follows teh same process as it's counterpart in heap.c, largely the same
int deleteSequentialFile(Buffer *buf, char *tablename) {
	tableDescription *table = buf->tables;
   tableDescription *prev = NULL;
   int ndx, fd;

   while (table != NULL) {
      if (!strcmp(table->tableName, tablename)) {
         fd = table->fd;

         if (prev == NULL) {
            buf->tables = table->next;
         } else {
            prev->next = table->next;
         }

         break;
      }

      prev = table;
      table = table->next;
   }

   if (table == NULL) {
      return ERROR;
   }

   tfs_deleteFile(fd);

   for (ndx = 0; ndx < MAX_BUFFER_SIZE; ndx++) {
      if (!buf->pages[ndx].isAvailable) {
         if (buf->pages[ndx].diskPage.FD == fd) {
            buf->pages[ndx].isAvailable = 1;
         }
      }
   }

   for (ndx = 0; ndx < MAX_BUFFER_SIZE; ndx++) {
      if (!buf->cache[ndx].isAvailable) {
         if (buf->cache[ndx].diskPage.FD == fd) {
            buf->cache[ndx].isAvailable = 1;
         }
      }
   }

   return SUCCESS;
}

// Should be same as heap version
int generateRecordDescription(tableDescription table, char *record, int *recordSize) {
   Attribute *current = table.attributeList;
   char *record_head = record;
   int bytes_copied = 0;
    *recordSize = 0;
   // The number of bytes needed for padding.
   uint8_t padding;

   while (current != NULL) {

      // If the number of bytes copied to the buffer is not divisble by the
      // number of bytes required by the current attribute's data type, add the
      // correct number of padding bytes.
      switch (current->attType) {
         case INT:
         case DATETIME:
         case BOOLEAN:
         case VARCHAR:
            // Make sure the record_size is divisible by four
            if (*recordSize % 4 != 0) {
               padding = 4 - (*recordSize % 4);
               memset(record_head++, 0, 1);
               memset(record_head++, PADDING, 1);
               memcpy(record_head++, &padding, sizeof(uint8_t));
               *recordSize += padding;
               bytes_copied += 3;
            }

            break;
         case FLOAT:
            // Make sure the record_size is divisible by eight
            if (*recordSize % 8 != 0) {
               padding = 8 - (*recordSize % 8);
               memset(record_head++, PADDING, 1);
               memcpy(record_head++, &padding, sizeof(uint8_t));
               *recordSize += padding;
            }

            break;
         default:
            // Problem! The type did not match any known type.
            return ERROR;
      }

      memcpy(record_head, current->attName, strlen(current->attName) + 1);
      record_head += strlen(current->attName) + 1;
      bytes_copied += strlen(current->attName) + 1;

      memcpy(record_head++, &(current->attType), sizeof(uint8_t));
      ++bytes_copied;

      if (current->attType == VARCHAR) {
         // We need to put in another byte for size
         // This can be a single byte since the largest varchar is 140
         //
         // We also need to make sure that the size is set by the
         // parser/whatever to be the varchar size + 1 for the null terminating
         // byte.
         memcpy(record_head++, &(current->attSize), sizeof(uint8_t));
         ++bytes_copied;
      }

      // The record_head is now pointing at the type byte.
      switch (current->attType) {
         case INT:
         case DATETIME:
         case BOOLEAN:
            *recordSize += 4;
            break;
         case FLOAT:
            *recordSize += 8;
            break;
         case VARCHAR:
            // Need to read the next byte to get the variable size of the
            // varchar.
            *recordSize += current->attSize + 1;
            break;
         default:
            // Problem! The type did not match any known type.
            return ERROR;
      }

      current = current->next;
   }

   return bytes_copied;
}

int getSeqHeader(fileDescriptor fileId, Buffer *buf, SeqFileHeader *header) {
   char out[BLOCKSIZE];
   DiskAddress diskPage;
   diskPage.FD = fileId;
   diskPage.pageId = 0;
   // If not in buffer
   if (pageExistsInBuffer(buf, diskPage) == ERROR) {
      // If not in buffer, check in cache
      if (pageExistsInCache(buf, diskPage) == ERROR) {
         // If not in chache, read to buffer
         readPage(buf, diskPage);
         getPage(buf, diskPage, out);
      } else {
         // If in cache, get page from cache
         getVolatilePage(buf, diskPage, out);
      }
   } else {
      // If in Buffer, read from Buffer
      getPage(buf, diskPage, out);
   }
   memcpy(header, out, sizeof(SeqFileHeader));

   return SUCCESS;
}

int seqHeaderGetTableName(fileDescriptor fileId, Buffer *buf, char *name) {
   SeqFileHeader header;
   // Get the contents of the seq file header page
   getSeqHeader(fileId, buf, &header);
   // Copy tablename into pointer
   memcpy(name, header.table_name, MAX_TABLENAME_SIZE);
   return SUCCESS;   
}

int seqHeaderGetRecordDesc(fileDescriptor fileId, char *bytes) {
   return ERROR;
}

int seqHeaderGetFirstPage(fileDescriptor fileId, DiskAddress *page, Buffer *buf) {
   SeqFileHeader header;
   // Get the contents of the seq file header
   getSeqHeader(fileId, buf, &header);
   // Return the address of the first page in the PageList list
   memcpy(&(page->pageId), &(header.page_list), sizeof(int));
   return SUCCESS;
}
// TODO: make sure this gets set to something in CreateSequentialFIle()
int seqHeaderGetLastPage(fileDescriptor fileId, DiskAddress *page, Buffer *buf) {
   SeqFileHeader header;
   // Get the contents of the seq file header
   getSeqHeader(fileId, buf, &header);
   // Return the address of the last page in the PageList list
   memcpy(&(page->pageId), &(header.last_page), sizeof(int));
   return SUCCESS;
}