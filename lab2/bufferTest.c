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

// Needs to read input and call commands for the following inputs
// While reading from file is not null
// Strtok on whitespace to split the string
// Take the first of the strtok to be the command
// Based on the command handle the options
int main() {
   char *line = NULL;
   char *command;
   size_t size = MAX_LINE_SIZE;
   DiskAddress *address, newAddress;
   Buffer buf;

   while (getline(&line, &size, stdin)) {
      // strsep the line based on
      command = strsep(&line, " ");
      printf("You entered command %s\n", command);

      if (!strcmp(command, "start")) {
         // Expect a filename and a number of blocks
         char *filename = strsep(&line, " ");
         char *numBlocks = strsep(&line, " ");
         int nBlocks;

         nBlocks = atoi(numBlocks);
         commence(filename, &buf, nBlocks);
      } else if (!strcmp(command, "end")) {
         // No args
         squash(&buf);
      } else if (!strcmp(command, "read")) {
         // Takes fd,pageid
         address = handle_args(line);
         readPage(&buf, *address);
      } else if (!strcmp(command, "write")) {
         // Takes fd,pageid
         address = handle_args(line);
         writePage(&buf, *address);
      } else if (!strcmp(command, "flush")) {
         // Takes fd,pageid
         address = handle_args(line);
         flushPage(&buf, *address);
      } else if (!strcmp(command, "pin")) {
         // Takes fd,pageid
         address = handle_args(line);
         pinPage(&buf, *address);
      } else if (!strcmp(command, "unpin")) {
         // Takes fd,pageid
         address = handle_args(line);
         unPinPage(&buf, *address);
      } else if (!strcmp(command, "new")) {
         // Takes num pages
         char *numPages = strsep(&line, " ");
         int nPages = atoi(numPages);
         newPage(&buf, /* FD */ 1, &newAddress);
      } else if (!strcmp(command, "check")) {
         // Call checkpoint of Test API
      } else {
         printf("Unknown command: %s\n", command);
         exit(-1);
      }
   }
}

