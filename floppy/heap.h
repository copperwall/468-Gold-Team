// Heap File Stuff goes here

#define BITMAP_SIZE 128

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

} HeapFileHeader;

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
   // By being 128, we can have a bit for every record with a record size of
   // 4 bytes. We also don't have to do any of that dynamic bullshit.
   char bitmap[BITMAP_SIZE];
} PageHeader;

// File Creation
int createHeapFile(Buffer *buf, char *tableName, tableDescription *createTable);
int deleteHeapFile(Buffer *buf, char *tableName);

//File Header
int getHeapHeader(fileDescriptor fileId, Buffer *buf, void *header);
int heapHeaderGetTableName(fileDescriptor fileId, Buffer *buf, char *name);
int heapHeaderGetRecordSize(fileDescriptor fileId, Buffer *buf);
int heapHeaderGetRecordDesc(fileDescriptor fileId, Buffer *buf, char *bytes);
int heapHeaderSetTableName(fileDescriptor fileId, Buffer *buf, char *name);
int heapHeaderGetNextPage(fileDescriptor fileId, DiskAddress *diskPage, Buffer *buf);
int heapHeaderGetFreeSpace(fileDescriptor fileId, DiskAddress *diskPage, Buffer *buf);

// Data Page
int getRecord(Buffer *buf, DiskAddress diskPage, int recordId, char *bytes);
int putRecord(Buffer *buf, DiskAddress diskPage, int recordId, char *bytes);

// Getters and Setters
int pHGetRecSize(Buffer *buf, DiskAddress page);
int pHSetRecSize(Buffer *buf, DiskAddress diskPage, int recSize);
int pHGetMaxRecords(Buffer *buf, DiskAddress page);
int pHSetMaxRecords(Buffer *buf, DiskAddress diskPage, int maxRecords);
int pHGetNumRecords(Buffer *buf, DiskAddress page);
int pHSetNumRecords(Buffer *buf, DiskAddress diskPage, int numRecords);
int pHGetBitmap(Buffer *buf, DiskAddress page, char *out);
int pHSetBitmap(Buffer *buf, DiskAddress diskPage, char *bitmap);
int pHGetCreateTimestamp(Buffer *buf, DiskAddress diskPage);
int pHSetCreateTimestamp(Buffer *buf, DiskAddress diskPage, int timestamp);
int pHGetFlushTimestamp(Buffer *buf, DiskAddress diskPage);
int pHSetFlushTimestamp(Buffer *buf, DiskAddress diskPage, int timestamp);
int pHGetNextPage(Buffer *buf, DiskAddress page);
int pHSetNextPage(Buffer *buf, DiskAddress diskPage, int nextPage);
int pHGetNextFree(Buffer *buf, DiskAddress page);
int pHSetNextFree(Buffer *buf, DiskAddress diskPage, int nextFree);
//CRUD
int generateRecordDescription(tableDescription table, char *record, int *recordSize);
int insertRecord(Buffer *buf, char * tableName, char * record, DiskAddress * location);
int deleteRecord(Buffer *buf, DiskAddress diskPage, int recordId);
int updateRecord(Buffer *buf, DiskAddress diskPage, int recordId, char *record);
int pHGetNextRecordSpace(char *bitmap);

int pHSetBitmapTrue(char *bitmap, int recordId);
int pHSetBitmapFalse(char *bitmap, int recordId);

int isRecordAvailable(char *bitmap, int recordId);

int pHIncrementNumRecords(Buffer *buf, DiskAddress diskPage);
int pHDecrementNumRecords(Buffer *buf, DiskAddress diskPage);

// Record level
int getField(char *fieldName, char *record, char *rd, int rdSize, char *out);
int setField(char *fieldName, char *record, char *rd, int rdSize, char *value);
int getRecordHeader(char *record, char *rd, char *value);

//Printing
void printRecord(char *recordDesc, int size, char *record);
void printRecordLabel(char *recordDesc, int size);
void printRecordDesc(char *recordDesc, int size);
void printTable(fileDescriptor fileId, Buffer *buf, char *recordDesc);
