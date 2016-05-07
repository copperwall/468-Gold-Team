/* readewrite.h */

char * getPage(Buffer * buf, DiskAddress page);

// I think we can assume that the size of |data| will be the size of a page.
int putPage(Buffer buf, DiskAddress page, char * data);
char * read(Buffer * buf, DiskAddress page, int startOffset, int nBytes);
// I think Dekhtyar said we could use another parameter for the size of data.
int write(Buffer * buf, DiskAddress page, int startOffset, int nBytes, char * data);
char * readVolatile(Buffer * buf, DiskAddress page, int startOffset, int nBytes);
int writeVolatile(Buffer* buf, DiskAddress page, int startOffset, int nBytes, char * data);
