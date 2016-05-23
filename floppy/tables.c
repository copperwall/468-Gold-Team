#include "floppy.h"
#include "bufman.h"
#include "tables.h"
#include "heap.h"
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

   // Create heapfile for new table
   chkerr(createHeapFile(buf, table.tableName, table));

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

   chkerr(createHeapFile(buf, table.tableName, table));
   return SUCCESS;
}
