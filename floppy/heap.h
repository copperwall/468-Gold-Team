// Heap File Stuff goes here

#define MAX_TABLENAME_SIZE 50

typedef struct heap_file_header {
   // Table name
   char table_name[MAX_TABLENAME_SIZE];
   // The number of bytes the record description is.
   int record_desc_size;
   // The number of bytes in a record
   int record_size;
   // The first data pageid of the file
   int next_page;
   // The pageid of the first page with free record spaces.
   int freelist;

} HeapFilerHeader;

typedef struct page_header {
   // The filename that this belongs to.
   // NOTE: I think tinyfs puts its own limits on filename size
   char filename[MAX_TABLENAME_SIZE];
   // The tinyFS pageid that this is.
   int pageid;
   // The size of the record
   int record_size;
   // This should be determined dynamically.
   int max_records;
   // Number of record slots used.
   int num_occupied;
   // Timestamp for when the page is first created.
   int create_timestamp;
   // Timestamp for when the page was last flushed to disk.
   int flush_timestamp;
   // Pageid for the next page in the heap file.
   int next_page;
   // Pageid for the first page in the freelist.
   int freelist;
   // Needs to be large enough to hold the number of records in a page.
   // Maybe the bitmap should be at the end of the struct because it's dynamic.
   // It makes de/serialization a little weird, but oh well
   // TODO: bitmap
} PageHeader;

// File Creation
int createHeapFile(Buffer *buf, char *tableName, tableDescription createTable);
int deleteHeapFile(Buffer *buf, char *tableName);
//File Header
int getHeapHeader(fileDescriptor fileId, Buffer *buf, void *header);
int heapHeaderGetTableName(fileDescriptor fileId, Buffer *buf, char *name);
int heapHeaderGetRecordDesc(fileDescriptor fileId, Buffer *buf, char *bytes);
int heapHeaderSetTableName(fileDescriptor fileId, Buffer *buf, char *name);
int heapHeaderGetNextPage(fileDescriptor fileId, DiskAddress *diskPage, Buffer *buf);
int heapHeaderGetFreeSpace(fileDescriptor fileId, DiskAddress *diskPage, Buffer *buf);
// Data Page
int pHGetRecSize(Buffer *buf, DiskAddress page);
int pHGetMaxRecords(Buffer *buf, DiskAddress page);
int pHGetNumRecords(Buffer *buf, DiskAddress page);
int pHGetBitmap(Buffer *buf, DiskAddress page);
int pHGetNextPage(Buffer *buf, DiskAddress page);
int pHGetNextFree(Buffer *buf, DiskAddress page);
//CRUD
int generateRecordDescription(tableDescription table, char *record, int *recordSize);
int insertRecord(char * tableName, char * record, DiskAddress * location);
int deleteRecord(DiskAddress diskPage, int recordId);
int updateRecord(DiskAddress diskPage, int recordId, char *record);
