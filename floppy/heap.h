// Heap File Stuff goes here

typedef struct heap_file_header {
   // Table name
   char table_name[MAX_TABLENAME_SIZE] table_name;
   // TODO: Record description as another char array
   // The number of bytes in a record
   int record_size;                                      
   // The first data pageid of the file
   int next_page;
   // The pageid of the first page with free record spaces.
   int freelist;

   // Could just add a buffer with an arbitrary number of spaces incase we
   // want to expand later.
} HeapFilerHeader;

typedef struct page_header {
   // The filename that this belongs to.
   char filename[MAX_TABLENAME_SIZE]; 
   // The tinyFS pageid that this is.
   int pageid;                      
   // This should be determined dynamically.
   int max_records;                 
   // Number of record slots used.
   int num_occupied;                
   // Needs to be large enough to hold the number of records in a page.
   // Maybe the bitmap should be at the end of the struct because it's dynamic.
   // It makes de/serialization a little weird, but oh well
   // TODO: bitmap,                 
   // Timestamp for when the page is first created.
   int create_timestamp;
   // Timestamp for when the page was last flushed to disk.
   int flush_timestamp;
   // Pageid for the next page in the heap file.
   int next_page;
   // Pageid for the first page in the freelist.
   int freelist;
} PageHeader

// File Creation
int createHeapFile(Buffer *buf, char *tableName, tableDescription createTable, int volatileFlag);
int deleteHeapFile(Buffer *buf, char *tableName);
