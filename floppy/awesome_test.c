#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "floppy.h"
#include "bufman.h"
#include "heap.h"
#include "tables.h"

void testInsertRecordOneInt() {
   tableDescription tableDef;
   Attribute firstColumn;

   tfs_mkfs("awesome.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("awesome.dsk");

   // Set the table description
   strcpy(tableDef.tableName, "list");
   tableDef.attributeList = &firstColumn;
   tableDef.pKey = &firstColumn;
   tableDef.isVolatile = 0;
   tableDef.next = NULL;

   // First column definition
   firstColumn.attName = "foo";
   firstColumn.attType = TYPE_INT;
   firstColumn.next = NULL;

   // Initialize buffer and create table.
   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("awesome.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createPersistentTable(buf, tableDef);

   char recordDescription[2048];

   int fd = getFileDescriptorForTable(buf, "list");
   // This assertion is 0 because there should be no other files.
   assert(fd == 0);
   int rdSize = heapHeaderGetRecordDesc(fd, buf, recordDescription);

   int recordSize = heapHeaderGetRecordSize(fd, buf);
   char *record = calloc(1, recordSize);
   int value = 4;
   setField("foo", record, recordDescription, rdSize, (char *)&value);

   DiskAddress recordPage;
   insertRecord(buf, "list", record, &recordPage);
   printf("Checking max records after insert %d\n" ,pHGetMaxRecords(buf, recordPage));

   char *testRecord = calloc(1, sizeof(Buffer));
   int testField = 0;
   // I'm pretty sure that recordid 0 is the first?
   getRecord(buf, recordPage, 0, testRecord);

   getField("foo", testRecord, recordDescription, rdSize, (char *)&testField);

   printf("Expected: %d\nActual: %d\n", value, testField);
   assert(testField == value);

   DiskAddress out;
   heapHeaderGetNextPage(fd, &out, buf);
   printf("File Header next page: %d\n", out.pageId);

   printf("New page next page: %d\n", pHGetNextPage(buf, recordPage));
   printf("New page max records: %d\n", pHGetMaxRecords(buf, recordPage));
}

void testInsertRecordOneString() {
   tableDescription tableDef;
   Attribute firstColumn;

   tfs_mkfs("awesome.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("awesome.dsk");

   // Set the table description
   strcpy(tableDef.tableName, "list");
   tableDef.attributeList = &firstColumn;
   tableDef.pKey = &firstColumn;
   tableDef.isVolatile = 0;
   tableDef.next = NULL;

   // First column definition
   firstColumn.attName = "foo";
   firstColumn.attType = TYPE_VARCHAR;
   firstColumn.attSize = 6;
   firstColumn.next = NULL;

   // Initialize buffer and create table.
   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("awesome.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createPersistentTable(buf, tableDef);

   char recordDescription[2048];

   int fd = getFileDescriptorForTable(buf, "list");
   // This assertion is 0 because there should be no other files.
   assert(fd == 0);
   int rdSize = heapHeaderGetRecordDesc(fd, buf, recordDescription);

   int recordSize = heapHeaderGetRecordSize(fd, buf);
   char *record = calloc(1, recordSize);
   char value[10] = "hello";
   setField("foo", record, recordDescription, rdSize, (char *)&value);

   DiskAddress recordPage;
   insertRecord(buf, "list", record, &recordPage);

   char *testRecord = calloc(1, sizeof(Buffer));
   char testField[10];
   // I'm pretty sure that recordid 0 is the first?
   getRecord(buf, recordPage, 0, testRecord);

   getField("foo", testRecord, recordDescription, rdSize, (char *)&testField);

   printf("Expected: %s\nActual: %s\n", value, testField);
   assert(!strcmp(value, testField));

   DiskAddress out;
   heapHeaderGetNextPage(fd, &out, buf);
   printf("File Header next page: %d\n", out.pageId);

   printf("New page next page: %d\n", pHGetNextPage(buf, recordPage));
   printf("New page max records: %d\n", pHGetMaxRecords(buf, recordPage));
}

void testInsertRecordTwoInt() {
   tableDescription tableDef;
   Attribute firstColumn, secondColumn;

   tfs_mkfs("awesome.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("awesome.dsk");

   // Set the table description
   strcpy(tableDef.tableName, "list");
   tableDef.attributeList = &firstColumn;
   tableDef.pKey = &firstColumn;
   tableDef.isVolatile = 0;
   tableDef.next = NULL;

   // First column definition
   firstColumn.attName = "foo";
   firstColumn.attType = TYPE_INT;
   firstColumn.next = &secondColumn;

   // Second column definition
   secondColumn.attName = "bar";
   secondColumn.attType = TYPE_INT;
   secondColumn.next = NULL;

   // Initialize buffer and create table.
   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("awesome.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createPersistentTable(buf, tableDef);

   char recordDescription[2048];

   int fd = getFileDescriptorForTable(buf, "list");
   // This assertion is 0 because there should be no other files.
   assert(fd == 0);
   int rdSize = heapHeaderGetRecordDesc(fd, buf, recordDescription);

   int recordSize = heapHeaderGetRecordSize(fd, buf);
   char *record = calloc(1, recordSize);
   int expected1 = 4;
   int expected2 = 10;

   setField("foo", record, recordDescription, rdSize, (char *)&expected1);
   setField("bar", record, recordDescription, rdSize, (char *)&expected2);

   DiskAddress recordPage;
   /* printf("Checking max records before insert %d\n", pHGetMaxRecords(buf, recordPage)); */
   insertRecord(buf, "list", record, &recordPage);
   printf("Checking max records after insert %d\n" ,pHGetMaxRecords(buf, recordPage));

   char *testRecord = calloc(1, sizeof(Buffer));
   int testField1 = 0;
   int testField2 = 0;
   // I'm pretty sure that recordid 0 is the first?
   getRecord(buf, recordPage, 0, testRecord);

   getField("foo", testRecord, recordDescription, rdSize, (char *)&testField1);
   getField("bar", testRecord, recordDescription, rdSize, (char *)&testField2);

   printf("Expected: %d\nActual: %d\n", expected1, testField1);
   assert(testField1 == expected1);
   printf("Expected: %d\nActual: %d\n", expected2, testField2);
   assert(testField2 == expected2);

   DiskAddress out;
   heapHeaderGetNextPage(fd, &out, buf);
   printf("File Header next page: %d\n", out.pageId);

   printf("New page next page: %d\n", pHGetNextPage(buf, recordPage));
   printf("New page max records: %d\n", pHGetMaxRecords(buf, recordPage));
}

void testInsertTwoRecordsOneInt() {
   tableDescription tableDef;
   Attribute firstColumn;

   tfs_mkfs("awesome.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("awesome.dsk");

   // Set the table description
   strcpy(tableDef.tableName, "list");
   tableDef.attributeList = &firstColumn;
   tableDef.pKey = &firstColumn;
   tableDef.isVolatile = 0;
   tableDef.next = NULL;

   // First column definition
   firstColumn.attName = "foo";
   firstColumn.attType = TYPE_INT;
   firstColumn.next = NULL;

   // Initialize buffer and create table.
   Buffer *buf = calloc(1, sizeof(Buffer));
   commence("awesome.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
   createPersistentTable(buf, tableDef);

   char recordDescription[2048];

   int fd = getFileDescriptorForTable(buf, "list");
   // This assertion is 0 because there should be no other files.
   assert(fd == 0);
   int rdSize = heapHeaderGetRecordDesc(fd, buf, recordDescription);

   int recordSize = heapHeaderGetRecordSize(fd, buf);
   char *record = calloc(1, recordSize);

   // Set the first record to 4
   int value1 = 4;
   int value2 = 10;
   setField("foo", record, recordDescription, rdSize, (char *)&value1);

   DiskAddress recordPage;
   insertRecord(buf, "list", record, &recordPage);

   // Set the second record to 10
   setField("foo", record, recordDescription, rdSize, (char *)&value2);
   insertRecord(buf, "list", record, &recordPage);

   char *testRecord = calloc(1, sizeof(Buffer));
   int testField = 4;
   // I'm pretty sure that recordid 0 is the first?
   getRecord(buf, recordPage, 0, testRecord);

   getField("foo", testRecord, recordDescription, rdSize, (char *)&testField);

   printf("Expected: %d\nActual: %d\n", value1, testField);
   assert(testField == value1);

   testField = 10;

   getRecord(buf, recordPage, 1, testRecord);

   getField("foo", testRecord, recordDescription, rdSize, (char *)&testField);

   printf("Expected: %d\nActual: %d\n", value2, testField);
   assert(testField == value2);
}

int main() {
   testInsertRecordOneInt();
   testInsertRecordTwoInt();
   testInsertRecordOneString();
   testInsertTwoRecordsOneInt();
}
