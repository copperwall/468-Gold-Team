#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bufman.h"
#define MAX_LINE_SIZE 20

// Take a fd,disk_address string and return the two values
DiskAddress *handle_args(char *args) {
   char *fd, *pageid;
   DiskAddress *address = malloc(sizeof(DiskAddress));

   fd = strsep(&args, ",");
   pageid = strsep(&args, ",");

   address->FD = atoi(fd);
   address->pageId = atoi(pageid);

   return address;
}

// Needs to read from file and call commands for the following inputs
// While reading from file is not null
// Strtok on whitespace to split the string
// Take the first of the strtok to be the command
// Based on the command handle the options
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
         // Expect a filename and a number of blocks
         char *diskName = strsep(&line, " ");
         char *numBlocks = strsep(&line, " ");
         int nBlocks;

         nBlocks = atoi(numBlocks);
         commence(diskName, buf, nBlocks);
      } else if (!strcmp(command, "end")) {
         // No args
         squash(buf);
         return 0;
      } else if (!strcmp(command, "read")) {
         // Takes fd,pageid
         address = handle_args(line);
         printf("Reading page with fd %d and pageid %d\n", address->FD, address->pageId);
         readPage(buf, *address);
      } else if (!strcmp(command, "write")) {
         // Takes fd,pageid
         address = handle_args(line);
         writePage(buf, *address);
      } else if (!strcmp(command, "flush")) {
         // Takes fd,pageid
         address = handle_args(line);
         flushPage(buf, *address);
      } else if (!strcmp(command, "pin")) {
         // Takes fd,pageid
         address = handle_args(line);
         pinPage(buf, *address);
      } else if (!strcmp(command, "unpin")) {
         // Takes fd,pageid
         address = handle_args(line);
         unPinPage(buf, *address);
      } else if (!strcmp(command, "new")) {
         // Takes num pages
         char *numPages = strsep(&line, " ");
         int nPages = atoi(numPages);

         int i;
         for (i = 0; i < nPages; i++) {
            newPage(buf, tfs_fd, &newAddress);
         }
         for (i = 0; i < buf->numOccupied; i++) {
            printf("FD: %d pageid: %d\n", buf->pages[i].diskPage.FD, buf->pages[i].diskPage.pageId);
         }
      } else if (!strcmp(command, "check")) {
         // Call checkpoint of Test API
         checkpoint(buf);
      } else {
         printf("Unknown command: %s\n", command);
         printf("%d\n", strcmp(command, "end"));
         /* exit(-1); */
      }
   }
}

