#include <string.h>
#include "floppy.h"
#include "bufman.h"
#include "readwrite.h"
#include "tables.h"
#include "heap.h"

// Heap File Stuff goes here

////////////////////////////
// File Creation
////////////////////////////

/**
 * createHeapFile
 *
 * Create a new tinyFS file based on the name of the table given.
 * If the volatileFlag is set, create the table in volatile storage.
 */
int createHeapFile(Buffer *buf, char *tableName, tableDescription createTable) {
   // If the table is non-volatile, create a new tinyFS file and read the
   // header page into the buffer.
   //
   // Place tableName into the first MAX_TABLENAME_SIZE bytes of the header
   // page.
   //
   // Given the tableDescription, calculate the record description. This should
   // be the name and size of each attribute. Given this, we should know the
   // size of each record.
   //
   // We could also set pageid 1 as the next_page and freelist value.
   HeapFilerHeader fileHeader;
   DiskAddress diskPage;
   // TODO: Make a constant for the max recorddescription size.
   char recordDescription[2048];
   char headerPage[BLOCKSIZE];
   int recordSize, fd;

   fileHeader.next_page = fileHeader.freelist = 1;
   strncpy(fileHeader.table_name, tableName, MAX_TABLENAME_SIZE);

   fileHeader.record_desc_size = generateRecordDescription(createTable, recordDescription, &recordSize);
   fileHeader.record_size = recordSize;

   // Have a page buffer and fill it first with the static part of the file
   // header.
   //
   // After that place the record description in it.
   memcpy(headerPage, &fileHeader, sizeof(HeapFilerHeader));
   memcpy(headerPage + sizeof(HeapFilerHeader), recordDescription, fileHeader.record_desc_size);

   // Open new file with the table name
   diskPage.FD = tfs_openFile(createTable.tableName);
   diskPage.pageId = 0;

   if (!createTable.isVolatile) {
      // Then put the page into the buffer with the new fd and pageid 0 and
      // flush the page.
      putPage(buf, diskPage, headerPage, BLOCKSIZE);
   } else {
      putVolatilePage(buf, diskPage, headerPage, BLOCKSIZE);
   }

   flushPage(buf, diskPage);

   return SUCCESS;
}

/**
 * deleteHeapFile
 *
 * Removes the heap file from tinyFS and removes all of its pages from the
 * buffer.
 */
int deleteHeapFile(Buffer *buf, char *tableName) {
   tableDescription *table = buf->tables;
   tableDescription *prev = NULL;
   int i, fd;

   // Find the table and remove it from the linked list.
   while (table != NULL) {
      if (!strcmp(table->tableName, tableName)) {
         fd = table->fd;

         // Remove the entry from the linked list.
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
      // Table not found.
      return ERROR;
   }

   tfs_deleteFile(fd);

   // Mark all pages in the buffer with that fd as available.
   for (i = 0; i < MAX_BUFFER_SIZE; i++) {
      if (!buf->pages[i].isAvailable) {
         if (buf->pages[i].diskPage.FD == fd) {
            buf->pages[i].isAvailable = 1;
         }
      }
   }

   // Mark all pages in the cache with that fd as available.
   for (i = 0; i < MAX_BUFFER_SIZE; i++) {
      if (!buf->cache[i].isAvailable) {
         if (buf->cache[i].diskPage.FD == fd) {
            buf->cache[i].isAvailable = 1;
         }
      }
   }

   return SUCCESS;
}

/**
 * Given a table description, create a record description for all of the
 * attributes.
 *
 * I think we should do padding instead of reordering so that we don't need to
 * keep track of original order in a separate place.
 *
 * It would also be helpful to know the largest record size for the demo.
 *
 * Return the size of the record description.
 *
 * Record description spec:
 *
 * The size of the record description will be stored in the file header page.
 *
 * null-terminated filename followed by a single byte with an Enum for what
 * type of attribute it is.
 *
 * |record| is an out parameter allocated by the caller?
 */
int generateRecordDescription(tableDescription table, char *record, int *recordSize) {
   Attribute *current = table.attributeList;
   char *record_head = record;
   int bytes_copied = 0;
   int record_size = 0;
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
            if (record_size % 4 != 0) {
               padding = 4 - (record_size % 4);
               memset(record_head++, PADDING, 1);
               memcpy(record_head++, &padding, sizeof(uint8_t));
               record_size += padding;
            }

            break;
         case FLOAT:
            // Make sure the record_size is divisible by eight
            if (record_size % 8 != 0) {
               padding = 8 - (record_size % 8);
               memset(record_head++, PADDING, 1);
               memcpy(record_head++, &padding, sizeof(uint8_t));
               record_size += padding;
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
            record_size += 4;
            break;
         case FLOAT:
            record_size += 8;
            break;
         case VARCHAR:
            // Need to read the next byte to get the variable size of the
            // varchar.
            record_size += current->attSize;
            break;
         default:
            // Problem! The type did not match any known type.
            return ERROR;
      }
   }

   return bytes_copied;
}

/////////////////////
// CRUD Operations
/////////////////////

int insertRecord(char * tableName, char * record, DiskAddress * location) {
   // Need to lookup fd from tableName
   // Go to the first pageid of the freelist and find the first free location
   // from the bitmap. Update the bitmap.
   // Update the freelist if that was the last free space.
   // Record the diskaddress in the location out param.
   return ERROR;
}

int deleteRecord(DiskAddress page, int recordId) {
   // Read the diskaddress into the buffer, read the record size and go to that
   // offset after the page header.
   // Maybe just set the bitmap to available.
   //
   // Update the freelist if that's the first free space.
   return ERROR;
}

int updateRecord(DiskAddress page, int recordId, char *record) {
   // Read in the diskaddress and go to the record offset and write in the
   // updated record.
   return ERROR;
}

// Loads the Heap File Header page into the buffer if not present, otherwise returns contents of that page 
int getHeapHeader(fileDescriptor fileId, Buffer *buf, char *out) {
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
   return SUCCESS;
}

int heapHeaderGetTableName(fileDescriptor fileId, Buffer *buf, char *name) {
   char blockContent[BLOCKSIZE];
   //get the contents of the heap file header page
   getHeapHeader(fileId, buf, blockContent);
   //Load the tablename into *name
   memcpy(name, blockContent, MAX_TABLENAME_SIZE);
   return SUCCESS;
}

int heapHeaderGetRecordDesc(fileDescriptor fileId, char *bytes) {
   //Get the record descriptor structure
   return ERROR;
}

int heapHeaderGetNextPage(fileDescriptor fileId, DiskAddress *page, Buffer *buf) {
   char blockContent[BLOCKSIZE];
   // Get the contents of the heap file header page
   getHeapHeader(fileId, buf, blockContent);
   // Return the address of the next page in the PageList list
   page->FD = fileId;
   memcpy(&(page->pageId), blockContent + MAX_TABLENAME_SIZE + 2 * sizeof(int), sizeof(int));
   return SUCCESS;
}

int heapHeaderGetFreeSpace(fileDescriptor fileId, DiskAddress *page, Buffer *buf) {
   char blockContent[BLOCKSIZE];
   // Get the contents of the heap file header page
   getHeapHeader(fileId, buf, blockContent);
   // Return the address of the next page in the PageList list
   page->FD = fileId;
   memcpy(&(page->pageId), blockContent + MAX_TABLENAME_SIZE + 3 * sizeof(int), sizeof(int));
   return ERROR;
}
