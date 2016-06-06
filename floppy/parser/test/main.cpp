#include <stdlib.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <stdio.h>

#include "../FLOPPYParser.h"

void catchsigsegv(int sig) {
   exit(SIGSEGV);
}

int main(int argc, char *argv[]) {
   // Set up Segfault handler
   struct sigaction act;
   act.sa_handler = catchsigsegv;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   sigaction(SIGSEGV, &act, 0);

   std::string query = std::string(argv[1]);
   std::cout << argv[1] << std::endl;

   FLOPPYOutput *result = FLOPPYParser::parseFLOPPYString(query);

   delete result->statement;

   return !result->isValid;
}
