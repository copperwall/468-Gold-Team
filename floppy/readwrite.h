/* readewrite.h */

int getPage(Buffer *buf, DiskAddress page, char *out);
int getVolatilePage(Buffer *buf, DiskAddress page, char *out);
int putPage(Buffer *buf, DiskAddress page, char *data, int dataSize);
int putVolatilePage(Buffer *buf, DiskAddress page, char *data, int dataSize);
int read(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *out);
int write(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *data, int dataSize);
int readVolatile(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *out);
int writeVolatile(Buffer *buf, DiskAddress page, int startOffset, int nBytes, char *data, int dataSize);
