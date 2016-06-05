#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "floppy.h"
#include "bufman.h"
#include "heap.h"

// This should be a test to make a simple table with one column.
//
// TODO: Create a tableDescription with the correct attributes and primary key
//
// Just test createHeapFile, not createPersistentTable yet
void testSingleAttribute() {
   tableDescription test;
   // How are these different?
   Attribute firstColumn;

   // TODO: OKAY need to create a bytearray with the expected value of the
   // recordDescription returned from the heapfile
   // 4 bytes for the name, 1 byte for INT type

   tfs_mkfs("yo.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("yo.dsk");

   test.tableName = "sup";
   test.attributeList = &firstColumn;
   test.pKey = &firstColumn;
   test.isVolatile = 0;
   test.next = NULL;

   firstColumn.attName = "winning";
   firstColumn.attType = INT;
   firstColumn.next = NULL;

   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("yo.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createHeapFile(buf, test.tableName, test);

   char tableName[MAX_TABLENAME_SIZE];
   // At this point, the header should be in the buffer
   heapHeaderGetTableName(0, buf, tableName);
   printf("Table name from header page is %s\n", tableName);
   heapHeaderSetTableName(0, buf, "lols");
   heapHeaderGetTableName(0, buf, tableName);
   printf("Table name from header page after set is %s\n", tableName);
   tfs_unmount();
}

void testDoubleAttribute() {
   tableDescription test;
   // How are these different?
   Attribute firstColumn, secondColumn;

   // TODO: OKAY need to create a bytearray with the expected value of the
   // recordDescription returned from the heapfile
   // 4 bytes for the name, 1 byte for INT type

   tfs_mkfs("yo.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("yo.dsk");

   test.tableName = "sup";
   test.attributeList = &firstColumn;
   test.pKey = &firstColumn;
   test.isVolatile = 0;
   test.next = NULL;

   firstColumn.attName = "winning";
   firstColumn.attType = INT;
   firstColumn.next = &secondColumn;

   secondColumn.attName = "wew";
   secondColumn.attType = BOOLEAN;
   secondColumn.next = NULL;

   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("yo.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createHeapFile(buf, test.tableName, test);

   tfs_unmount();
}

void testStringAttribute() {
   tableDescription test;
   // How are these different?
   Attribute firstColumn, secondColumn;

   tfs_mkfs("yo.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("yo.dsk");

   test.tableName = "sup";
   test.attributeList = &firstColumn;
   test.pKey = &firstColumn;
   test.isVolatile = 0;
   test.next = NULL;

   firstColumn.attName = "string";
   firstColumn.attType = VARCHAR;
   firstColumn.attSize = 10;
   firstColumn.next = &secondColumn;

   secondColumn.attName = "woop";
   secondColumn.attType = INT;
   secondColumn.next = NULL;

   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("yo.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createHeapFile(buf, test.tableName, test);

   tfs_unmount();
}

int main() {
   testSingleAttribute();
   testDoubleAttribute();
   testStringAttribute();
}
