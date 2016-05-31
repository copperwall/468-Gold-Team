#define MAX_TABLENAME_SIZE 50
#define MAX_INDEXNAME_SIZE 50

typedef struct sequential_file_header {
	// Index structure the table is representing
	char index_name[MAX_INDEXNAME_SIZE];
	// Could be storage file or log file
	char table_name[MAX_TABLENAME_SIZE];
	// TODO: Record description
	// Number of bytes the record description is
	int record_desc_size;
	// Record Size
	int record_size;
	// The first data pageid of the file
	int page_list;
	// Pageid of the last page, or total pages stored
	int last_page;

} SeqFileHeader;

typedef struct seq_page_header {
	// Filename and pageId 
	DiskAddress diskPage;
	// Record Size
	int record_size;
	// Number of Records on Page
	int num_records;
	// Number of Occupied slots
	int num_occupied;
	// Timestamp for when page is first created
	int create_timestamp;
	// Timestamp for when page was last flushed to disk
	int flush_timesstamp;
	// PageId of the next page in the disk file
	int next_page;
	// PageId of previous page (might be useful)
} SeqPageHeader;

// File Creation
int createSequentialFile(Buffer *buf, char *tablename, tableDescription createTable, int volatileFlag);
int deleteSequentialFile(Buffer *buf, char *tablename);
int generateRecordDescription(tableDescription table, char *record, int *recordSize);
// File Header
int getSeqHeader(fileDescriptor fileId, Buffer *buf, SeqFileHeader *header);
int seqHeaderGetTableName(fileDescriptor fileId, Buffer *buf, char *name);
int seqHeaderGetRecordDesc(fileDescriptor fileId, char *bytes);
int seqHeaderGetFirstPage(fileDescriptor fileId, DiskAddress *page, Buffer *buf);
int seqHeaderGetLastPage(fileDescriptor fileId, DiskAddress *page, Buffer *buf);
