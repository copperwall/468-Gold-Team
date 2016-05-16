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
}

/**
 * deleteHeapFile
 *
 * Removes the heap file from tinyFS and removes all of its pages from the
 * buffer.
 */
int deleteHeapFile(Buffer *buf, char *tableName) {
   // Grab the table from the table list in the buffer.
   //
   // If it's volatile, remove the dummy tinyFS fd and remove all of its pages
   // from volatile/persistent storage.
   //
   // NOTE: Volatile pages can be in both the volatile and non-volatile
   // storage, so check both.
   //
   // If it's not volatile then only look for pages with its fd in the
   // persistent storage.
   //
   // NOTE: I'm not sure if we have a function for removing persistent pages
   // from the buffer. Might need to go in buffer manager.
   //
   // I think that should be it.
}
