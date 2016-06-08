#include "floppy.h"
#include "bufman.h"
#include "tables.h"
#include "heap.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Given a Buffer and tableDescription, create a new persistent table.
 *
 * Returns Table Error if a table with the given name already exists.
 * Otherwise
 */
int createPersistentTable(Buffer *buf, tableDescription table) {
   printf("PERSISTING TABLE\n");
   tableDescription *tables = buf->tables;
   tableDescription *prev;
   DiskAddress header;

   if (table.isVolatile) {
      // The table being created should not be volatile.
      printf("ERROR TABLE IS VOLATILE");
      return ERROR;
   }

   while (tables != NULL) {
      if (strcmp(tables->tableName, table.tableName) == 0) {
         printf("ERROR TABLE EXISTS");
         return E_TABLE_EXISTS;
      }

      prev = tables;
      tables = tables->next;
   }

   // Add table to buf->tables
   tableDescription *newTable = calloc(sizeof(tableDescription), 1);
   memcpy(newTable, &table, sizeof(tableDescription));

   if (buf->tables == NULL) {
      buf->tables = newTable;
      printf("Set buf->tables\n");
   } else {
      printf("appended to %s\n", prev->tableName);
      prev->next = newTable;
   }

   // Create heapfile for new table
   chkerr(createHeapFile(buf, table.tableName, &table));
   newTable->fd = table.fd;

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

   chkerr(createHeapFile(buf, table.tableName, &table));
   return SUCCESS;
}

int getFileDescriptorForTable(Buffer *buf, char *name) {
   tableDescription *tables = buf->tables;

   while (tables != NULL) {
      if (strcmp(tables->tableName, name) == 0) {
         printf("Found table %s %d\n", tables->tableName, tables->fd);
         return tables->fd;
      } else {
         printf("next\n");
         tables = tables->next;
      }
   }
   printf("ERROR LOOKING UP TABLE\n");

   return ERROR;
}

/**
 * Given a buffer, a table name, and a pointer to a tableDescription, return a
 * tableDescription pointer to the tableDescription after it's populated with
 * the description for the given table name.
 */
tableDescription *getTableDescription(Buffer *buf, int fd, tableDescription *out) {
   tableDescription *tables = buf->tables;

   while (tables != NULL) {
      if (tables->fd = fd) {
         printf("Found table %s %d\n", tables->tableName, tables->fd);
         memcpy(out, tables, sizeof(tableDescription));
         return out;
      } else {
         printf("next\n");
         tables = tables->next;
      }
   }
   printf("DID NOT FIND TABLE DESCRIPTION\n");

   return NULL;
}
