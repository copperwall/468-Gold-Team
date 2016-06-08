#include <stdio.h>
#include <string.h>
#include "floppy.h"
#include "bufman.h"
#include "readwrite.h"
#include "tables.h"
#include "heap.h"

void printRecordDesc(char *recordDesc, int size) {
   int i = 0;
   int bytesRead = 0;
   uint8_t type;
   uint8_t varcharLen;
   char *recordHead = recordDesc;
   char attribute[MAX_TABLENAME_SIZE];

   while (i < size) {
      // Read attribute
      strcpy(attribute, recordHead);
      bytesRead = strlen(attribute);
      recordHead += bytesRead + 1;
      i += bytesRead + 1;
      printf("Attribute: %s\n", attribute);

      memcpy(&type, recordHead, sizeof(uint8_t));
      recordHead++;
      i++;

      switch (type) {
         case TYPE_INT:
            printf("INT\n");
            break;
         case TYPE_DATETIME:
            printf("DATETIME\n");
            break;
         case TYPE_BOOLEAN:
            printf("BOOLEAN\n");
            break;
         case TYPE_VARCHAR:
            printf("VARCHAR ");
            memcpy(&varcharLen, recordHead, sizeof(uint8_t));
            recordHead++;
            i++;
            printf("Length: %d\n", varcharLen);
            break;
         case TYPE_FLOAT:
            printf("FLOAT\n");
            break;
         case TYPE_PADDING:
            printf("PADDING\n");
            memcpy(&varcharLen, recordHead, sizeof(uint8_t));
            recordHead++;
            i++;
            printf("Length: %d\n", varcharLen);
            break;
         default:
            printf("Unknown type\n");
      }
   }

   printf("Done\n");
}
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
   HeapFileHeader fileHeader;
   DiskAddress diskPage;
   // TODO: Make a constant for the max recorddescription size.
   char recordDescription[2048];
   char headerPage[BLOCKSIZE];
   int recordSize, fd;

   fileHeader.next_page = fileHeader.freelist = 1;
   strncpy(fileHeader.table_name, tableName, MAX_TABLENAME_SIZE);

   fileHeader.record_desc_size = generateRecordDescription(createTable, recordDescription, &recordSize);
   fileHeader.record_size = recordSize;

#ifdef DEBUG
   printf("DEBUG: record desc size %d\n", fileHeader.record_desc_size);
   printf("DEBUG: record size %d\n", fileHeader.record_size);

   printRecordDesc(recordDescription, fileHeader.record_desc_size);
#endif

   // Have a page buffer and fill it first with the static part of the file
   // header.
   //
   // After that place the record description in it.
   memcpy(headerPage, &fileHeader, sizeof(HeapFileHeader));
   memcpy(headerPage + sizeof(HeapFileHeader), recordDescription, fileHeader.record_desc_size);

   // Open new file with the table name
   printf("Opening file with name %s\n", createTable.tableName);
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
    *recordSize = 0;
   // The number of bytes needed for padding.
   uint8_t padding;

   while (current != NULL) {

      // If the number of bytes copied to the buffer is not divisble by the
      // number of bytes required by the current attribute's data type, add the
      // correct number of padding bytes.
      switch (current->attType) {
         case TYPE_INT:
         case TYPE_DATETIME:
         case TYPE_BOOLEAN:
         case TYPE_VARCHAR:
            // Make sure the record_size is divisible by four
            if (*recordSize % 4 != 0) {
               padding = 4 - (*recordSize % 4);
               memset(record_head++, 0, 1);
               memset(record_head++, TYPE_PADDING, 1);
               memcpy(record_head++, &padding, sizeof(uint8_t));
               *recordSize += padding;
               bytes_copied += 3;
            }

            break;
         case TYPE_FLOAT:
            // Make sure the record_size is divisible by eight
            if (*recordSize % 8 != 0) {
               padding = 8 - (*recordSize % 8);
               memset(record_head++, TYPE_PADDING, 1);
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

      if (current->attType == TYPE_VARCHAR) {
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
         case TYPE_INT:
         case TYPE_DATETIME:
         case TYPE_BOOLEAN:
            *recordSize += 4;
            break;
         case TYPE_FLOAT:
            *recordSize += 8;
            break;
         case TYPE_VARCHAR:
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

/////////////////////
// CRUD Operations
/////////////////////

int insertRecord(Buffer *buf, char * tableName, char * record, DiskAddress * location) {
   // Need to lookup fd from tableName
   int fd = getFileDescriptorForTable(buf, tableName);
   char bitmap[BITMAP_SIZE];
   DiskAddress nextFree;
   heapHeaderGetFreeSpace(fd, &nextFree, buf);
   // Go to the first pageid of the freelist and find the first free location
   // from the bitmap. Update the bitmap.
   pHGetBitmap(buf, nextFree, bitmap);

   int nextRecordId = pHGetNextRecordSpace(bitmap);

   putRecord(buf, nextFree, nextRecordId, record);
   pHSetBitmapTrue(bitmap, nextRecordId);
   pHSetBitmap(buf, nextFree, bitmap);
   // Update the freelist if that was the last free space.
   // Record the diskaddress in the location out param.
   return SUCCESS;
}

// Read the diskaddress into the buffer, read the record size and go to that
// offset after the page header.
// Maybe just set the bitmap to available.
//
// Update the freelist if that's the first free space.
int deleteRecord(Buffer *buf, DiskAddress diskPage, int recordId) {
   char bitmap[BITMAP_SIZE];
   pHGetBitmap(buf, diskPage, bitmap);

   int deletedRecordId = pHGetNextRecordSpace(bitmap);

   pHSetBitmapFalse(bitmap, deletedRecordId);
   pHSetBitmap(buf, diskPage, bitmap);
   return SUCCESS;
}

// Read in the diskaddress and go to the record offset and write in the
// updated record.
int updateRecord(Buffer *buf, DiskAddress diskPage, int recordId, char *record) {
   putRecord(buf, diskPage, recordId, record);
   return SUCCESS;
}

// Loads the Heap File Header page into the buffer if not present, otherwise returns contents of that page
//
// IMPORTANT: I made the header out param a void * so that the caller can
// decide whether or not to cast the buffer to a HeapFileHeader or a char
// buffer.
int getHeapHeader(fileDescriptor fileId, Buffer *buf, void *header) {
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
   memcpy(header, out, BLOCKSIZE);

   return SUCCESS;
}

int heapHeaderGetTableName(fileDescriptor fileId, Buffer *buf, char *name) {
   HeapFileHeader *header;
   char page[BLOCKSIZE];
   //get the contents of the heap file header page
   getHeapHeader(fileId, buf, page);
   header = (HeapFileHeader *)page;
   //Load the tablename into *name
   memcpy(name, header->table_name, MAX_TABLENAME_SIZE);
   return SUCCESS;
}

int heapHeaderSetTableName(fileDescriptor fileId, Buffer *buf, char *name) {
   HeapFileHeader *header;
   char page[BLOCKSIZE];
   char oldName[MAX_TABLENAME_SIZE];
   DiskAddress diskPage;
   diskPage.FD = fileId;
   diskPage.pageId = 0;
   // Get the contents of the heap file header page
   getHeapHeader(fileId, buf, page);
   header = (HeapFileHeader *)page;

   strcpy(oldName, header->table_name);
   // Set the table name
   memcpy(header->table_name, name, MAX_TABLENAME_SIZE);
   // Write changes back to disk
   putPage(buf, diskPage, page, BLOCKSIZE);

   // Update the tables linked list in the buffer
   tableDescription *tables = buf->tables;

   while (tables != NULL) {
      if (!strcmp(tables->tableName, oldName)) {
         strcpy(tables->tableName, header->table_name);
      }
   }
}

/**
 * Grab the header, get the record desc size
 *
 * memcpy [sizeof(HeapFileHeader), record desc size] of page
 */
int heapHeaderGetRecordDesc(fileDescriptor fileId, Buffer *buf, char *bytes) {
   char page[BLOCKSIZE];
   HeapFileHeader *header;
   int recordDescSize;

   getHeapHeader(fileId, buf, page);
   header = (HeapFileHeader *)page;
   recordDescSize = header->record_desc_size;

   memcpy(bytes, page + sizeof(HeapFileHeader), recordDescSize);
   //Get the record descriptor structure
   return SUCCESS;
}

int heapHeaderGetNextPage(fileDescriptor fileId, DiskAddress *diskPage, Buffer *buf) {
   char page[BLOCKSIZE];
   HeapFileHeader *header;
   // Get the contents of the heap file header page
   getHeapHeader(fileId, buf, page);
   header = (HeapFileHeader *)page;
   // Return the address of the next page in the PageList list
   diskPage->FD = fileId;
   memcpy(&(diskPage->pageId), &(header->next_page), sizeof(int));
   return SUCCESS;
}

int heapHeaderGetFreeSpace(fileDescriptor fileId, DiskAddress *diskPage, Buffer *buf) {
   char page[BLOCKSIZE];
   HeapFileHeader *header;
   // Get the contents of the heap file header page
   getHeapHeader(fileId, buf, &header);
   header = (HeapFileHeader *)page;
   // Return the address of the next page in the PageList list
   diskPage->FD = fileId;
   memcpy(&(diskPage->pageId), &(header->freelist), sizeof(int));
   return SUCCESS;
}

/////////////////////////////
// Page level operations
/////////////////////////////

int getRecord(Buffer *buf, DiskAddress diskPage, int recordId, char *bytes) {
   // Get the page header from buffer
   char page[BLOCKSIZE];
   PageHeader *header;
   int recordSize, offset;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   recordSize = header->record_size;
   offset = sizeof(PageHeader) + (recordSize * recordId);

   memcpy(bytes, page + offset, recordSize);
}

int putRecord(Buffer *buf, DiskAddress diskPage, int recordId, char *bytes) {
   // Get the page header from buffer
   char page[BLOCKSIZE];
   PageHeader *header;
   int recordSize, offset;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   recordSize = header->record_size;
   offset = sizeof(PageHeader) + (recordSize * recordId);

   memcpy(page + offset, bytes, recordSize);
}

int pHGetRecSize(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->record_size;
}

int pHSetRecSize(Buffer *buf, DiskAddress diskPage, int recSize) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->record_size = recSize;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

int pHGetMaxRecords(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->max_records;
}

int pHSetMaxRecords(Buffer *buf, DiskAddress diskPage, int maxRecords) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->max_records = maxRecords;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

int pHGetNumRecords(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->num_occupied;
}

int pHSetNumRecords(Buffer *buf, DiskAddress diskPage, int numRecords) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->num_occupied = numRecords;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

int pHGetBitmap(Buffer *buf, DiskAddress diskPage, char *out) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   memcpy(out, header->bitmap, BITMAP_SIZE);
}

int pHSetBitmap(Buffer *buf, DiskAddress diskPage, char *bitmap) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   memcpy(header->bitmap, bitmap, BITMAP_SIZE);
}

int pHGetNextPage(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->next_page;
}

int pHGetCreateTimestamp(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->create_timestamp;
}

int pHSetCreateTimestamp(Buffer *buf, DiskAddress diskPage, int timestamp) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->create_timestamp = timestamp;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

int pHGetFlushTimestamp(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->flush_timestamp;
}

int pHSetFlushTimestamp(Buffer *buf, DiskAddress diskPage, int timestamp) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->flush_timestamp = timestamp;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

int pHSetNextPage(Buffer *buf, DiskAddress diskPage, int nextPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->next_page = nextPage;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

int pHGetNextFree(Buffer *buf, DiskAddress diskPage) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   return header->freelist;
}

int pHSetNextFree(Buffer *buf, DiskAddress diskPage, int nextFree) {
   char page[BLOCKSIZE];
   PageHeader *header;

   getPage(buf, diskPage, page);
   header = (PageHeader *)page;

   header->freelist = nextFree;
   putPage(buf, diskPage, page, BLOCKSIZE);
}

// For each bit, check to see if it is a 0.
// If it is, return the record slot id that is available.
int pHGetNextRecordSpace(char *bitmap) {
   int i;

   for (i = 0; i < BITMAP_SIZE; i++) {
      if (!(*bitmap >> (i % 8)) & 0x01) {
         return i;
      }

      if (i % 8 == 7) {
         // Move on to the next byte.
         bitmap++;
      }
   }

   return ERROR;
}

int pHSetBitmapTrue(char *bitmap, int recordId) {
   int byteNum;

   byteNum = recordId / 8;

   bitmap[byteNum] = bitmap[byteNum] | (0x01 << (recordId % 8));

   return SUCCESS;
}

int pHSetBitmapFalse(char *bitmap, int recordId) {
   int byteNum;

   byteNum = recordId / 8;

   bitmap[byteNum] = bitmap[byteNum] & (!(0x01 << (recordId % 8)));

   return SUCCESS;
}

int pHIncrementNumRecords(Buffer *buf, DiskAddress diskPage) {
   int numRecords = pHGetNumRecords(buf, diskPage);

   pHSetNumRecords(buf, diskPage, numRecords + 1);
   return SUCCESS;
}

int pHDecrementNumRecords(Buffer *buf, DiskAddress diskPage) {
   int numRecords = pHGetNumRecords(buf, diskPage);

   pHSetNumRecords(buf, diskPage, numRecords - 1);
   return SUCCESS;
}
