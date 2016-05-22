#include <string.h>
#include "floppy.h"
#include "bufman.h"
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
   HeapFilerHeader file_header;

   file_header.next_page = file_header.freelist = 1;
   strncpy(file_header.table_name, tableName, MAX_TABLENAME_SIZE);
   char recordDescription[2048];
   int recordSize;

   file_header.record_desc_size = generateRecordDescription(createTable, recordDescription, &recordSize);
   file_header.record_size = recordSize;
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
