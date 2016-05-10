#include <stdio.h>
#include <string.h>
#include "tinyFS.h"
#include "libTinyFS.h"

int main() {
  int index;
  int pageSize = BLOCKSIZE - FE_DATA;
  fileDescriptor a,b,c,d;
  char buffer[pageSize];
  memset(buffer,'1',pageSize);

  char* diskName = DEFAULT_DISK_NAME;

  printf("TinyFS tests =>  Creating a new file system:\n");
  tfs_mkfs(DEFAULT_DISK_NAME,DEFAULT_DISK_SIZE);  

  /*mounting the FS */
  tfs_mount(DEFAULT_DISK_NAME);
  a=tfs_openFile("ff8"); /* 8 alphanum characters, no spaces */
  b=tfs_openFile("file2");
  c=tfs_openFile("file3");
  tfs_readDir();
  tfs_writePage(a,0,buffer);
  memset(buffer,'2',pageSize);
  tfs_writePage(a,1,buffer);
  memset(buffer,'3',pageSize);
  tfs_writePage(a,2,buffer);
  tfs_readPage(a,1,buffer);
  printf("content of file page 1: ");
  for(index=0;index < pageSize; index++)
    printf("%c",buffer[index]);
  printf("\n -- done\n");
  tfs_closeFile(b);

  tfs_readPage(b,0,buffer); /* should fail because file is closed */

  d = tfs_openFile("file2"); 
  printf("file d (descriptor: %i) has %i pages \n",d,tfs_numPages(d));
  tfs_readDir();
  printf("\n Now deleting the file\n");
  tfs_deleteFile(d);
  tfs_readDir();
  tfs_closeFile(a);
  tfs_closeFile(c);
  tfs_unmount();

  tfs_mount(DEFAULT_DISK_NAME);
  a = tfs_openFile("ff8");
  printf("file a (descriptor: %i) has %i pages \n",a,tfs_numPages(a));
  tfs_readDir();
  tfs_unmount();

  
}
