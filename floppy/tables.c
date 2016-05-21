#include "floppy.h"
#include "bufman.h"
#include "tables.h"
#include <string.h>
#include <stdlib.h>

/**
 * Given a Buffer and tableDescription, create a new persistent table.
 *
 * Returns Table Error if a table with the given name already exists.
 * Otherwise 
 */
int createPersistentTable(Buffer *buf, tableDescription table) {
   tableDescription *tables = buf->tables;
   tableDescription *prev;
   DiskAddress header;

   if (table.isVolatile) {
      // The table being created should not be volatile.
      return ERROR;
   }

   while (tables != NULL) {
      if (!strcmp(tables->tableName, table.tableName)) {
         return E_TABLE_EXISTS;
      }

      prev = tables;
      tables = tables->next;
   }

   // Add table to buf->tables
   tableDescription *newTable = calloc(sizeof(tableDescription), 1);
   memcpy(newTable, &table, sizeof(tableDescription));

   prev->next = newTable;


   // Create a descriptor for the record structure of the database file
   // Maybe this means just a byte array that has enough space for all of the
   // attributes listed?

   // Create a new tinyFS datafile for storing the data. Filename should be
   // tablename.
   // TODO: Hmmm, should fd be part of the tables list in the buffer?
   header.FD = newTable->fd = tfs_openFile(table.tableName);
   header.pageId = 0;

   // Read file header page into buffer (pageid 0)
   chkerr(readPage(buf, header));

   // Write necessary information onto header page.
   // This includes the descriptor of the record strucutre that you have
   // constructed in Step 2.
   // TODO: What else should be stored in header page?
   // TODO: Need read/write functionality to write header page.

   // Force write the page back to disk.
   flushPage(buf, header);

   return SUCCESS;
}

/**
 * Given a Buffer and tableDescription, create a new volatile table.
 *
 * Should this just add the table description to the Buffer?
 * It definitely should not write the description to disk.
 */
int createVolatileTable(Buffer *buf, tableDescription table) {
   tableDescription *tables, *prev;
   DiskAddress header;
   prev = tables = buf->tables;

   if (!table.isVolatile) {
      // The table being created should not be persistent.
      return ERROR;
   }
   while (tables != NULL) {
      if (!strcmp(tables->tableName, table.tableName)) {
         return E_TABLE_EXISTS;
      }

      prev = tables;
      tables = tables->next;
   }

   // Add table to buf->tables
   tableDescription *newTable = calloc(sizeof(tableDescription), 1);
   memcpy(newTable, &table, sizeof(tableDescription));

   prev->next = newTable;

   // TODO: Is there anything else to do?
   // Do we write a table header to a page header in the volatile cache?
   return SUCCESS;
}
