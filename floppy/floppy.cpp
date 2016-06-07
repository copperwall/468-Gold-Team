#include <iostream>
#include <string>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "parser/FLOPPYParser.h"

extern "C" {
#include "floppy.h"
#include "bufman.h"
#include "heap.h"
}


void executeStatement(char *statement){
   FLOPPYOutput *output = FLOPPYParser::parseFLOPPYString(statement);
   if (output->isValid) { // Use isValid flag to detect is parser parsed correctly.
      switch(output->statement->type()) {
         case(StatementType::ErrorStatement) : 
            std::cout << "Found Error Statement" << std::endl;
            break;
         case(StatementType::CreateTableStatement) :
            std::cout << "Found Create Table Statement" << std::endl;
            break;
         case(StatementType::DropTableStatement) :
            std::cout << "Found Drop Table Statement" << std::endl;
            break;
         case(StatementType::CreateIndexStatement) :
            std::cout << "Found Create Index Statement" << std::endl;
            break;
         case(StatementType::DropIndexStatement) :
            std::cout << "Found Drop Index Statement" << std::endl;
            break;
         case(StatementType::InsertStatement) :
            std::cout << "Found Insert Statement" << std::endl;
            break;
         case(StatementType::DeleteStatement) :
            std::cout << "Found Delete Statement" << std::endl;
            break;
         case(StatementType::UpdateStatement) :
            std::cout << "Found Update Statement" << std::endl;
            break;
         case(StatementType::SelectStatement) :
            std::cout << "Found Select Statement" << std::endl;
            break;
         default :
            std::cout << "Unknown statement type" << std::endl;
            break;
      }
   }
   else {
      std::cout << "Parse Failed" << std::endl;
   }
}

int main(int argc, char *argv[]) {
   executeStatement("DROP TABLE test;");
}
