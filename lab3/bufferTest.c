#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bufman.h"
#define MAX_LINE_SIZE 20

// Take a filename and disk address string and return the two values
DiskAddress *handle_args(char *args) {
   char *filename, *pageid;
   DiskAddress *address = malloc(sizeof(DiskAddress));

   filename = strsep(&args, " ");
   pageid = strsep(&args, " ");

   address->FD = tfs_openFile(filename);
   address->pageId = atoi(pageid);

   return address;
}

// Close the file that was worked on.
void teardown(DiskAddress *address) {
   tfs_closeFile(address->FD);
   free(address);
}

// Needs to read from file and call commands for the following inputs
// While reading from file is not null
int main(int argc, char *argv[]) {
   char *line = NULL;
   char *command;
   size_t size = MAX_LINE_SIZE;
   DiskAddress *address, newAddress;
   Buffer *buf = calloc(1, sizeof(Buffer));
   FILE *input;
   int tfs_fd = 0;

   if (argc != 2) {
      printf("Usage: ./harness <filename>\n");
      return -1;
   }

   input = fopen(argv[1], "r");

   while (getline(&line, &size, input)) {
      // strsep the line based on
      printf("%s\n", line);
      command = strsep(&line, " ");

      if (command[strlen(command) - 1] == '\n') {
         command[strlen(command) - 1] = '\0';
      }

      if (!strcmp(command, "start")) {
         // Expect a diskName and a number of blocks
         char *diskName = strsep(&line, " ");
         char *numBlocks = strsep(&line, " ");
         int nBlocks;

         nBlocks = atoi(numBlocks);
         commence(diskName, buf, nBlocks);
      } else if (!strcmp(command, "end")) {
         squash(buf);
         return 0;
      } else if (!strcmp(command, "read")) {
         address = handle_args(line);
         readPage(buf, *address);
         teardown(address);
      } else if (!strcmp(command, "write")) {
         address = handle_args(line);
         writePage(buf, *address);
         teardown(address);
      } else if (!strcmp(command, "flush")) {
         address = handle_args(line);
         flushPage(buf, *address);
         teardown(address);
      } else if (!strcmp(command, "pin")) {
         address = handle_args(line);
         pinPage(buf, *address);
         teardown(address);
      } else if (!strcmp(command, "unpin")) {
         address = handle_args(line);
         unPinPage(buf, *address);
         teardown(address);
      } else if (!strcmp(command, "new")) {
         // Takes file, begin page, end page
         char *filename = strsep(&line, " ");
         char *page1 = strsep(&line, " ");
         char *page2 = strsep(&line, " ");

         int i, beginPage, endPage, tfs_fd;

         tfs_fd = tfs_openFile(filename);
         beginPage = atoi(page1);
         endPage = atoi(page2);
         newAddress.FD = tfs_fd;

         for (i = beginPage; i <= endPage; i++) {
            newAddress.pageId = i;
            newPage(buf, tfs_fd, &newAddress);
         }
         for (i = 0; i < buf->numOccupied; i++) {
            printf("FD: %d pageid: %d\n", buf->pages[i].diskPage.FD, buf->pages[i].diskPage.pageId);
         }

         tfs_closeFile(tfs_fd);
      } else if (!strcmp(command, "check")) {
         // Call checkpoint of Test API
         checkpoint(buf);
      } else {
         printf("Unknown command: %s\n", command);
      }
   }
}

