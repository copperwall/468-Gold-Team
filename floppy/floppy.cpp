#include <iostream>
#include <string>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "parser/FLOPPYParser.h"

extern "C" {
#include "floppy.h"
#include "bufman.h"
#include "heap.h"
#include "tables.h"
}

/*
int project(Buffer *buf, int fd, std::vector<FLOPPYSelectItem *> selectItems, int *outTable){
   tableDescription tempTable;
   getTableDescription(buf, fd, &tableDescription);

   tableDescription.tableName = strcat("TEMP_");

   createVolitileTable(bu);
}

int selectScan(int fd, int *outTable) {
   return 1;
}*/

void selectStatement(Buffer *buf, FLOPPYSelectStatement *statement) {
   std::vector<FLOPPYTableSpec *> tables = *(statement->tableSpecs);
   printf("TABLE TO SELECT: %s\n", tables[0]->tableName);
   char *recordDesc = (char *)malloc(2048);

   int origFD = getFileDescriptorForTable(buf, tables[0]->tableName);
   printf("FD FOR TABLE: %d\n", origFD);
   heapHeaderGetRecordDesc(origFD, buf, recordDesc);

   printTable(origFD, buf, recordDesc);

   //std::cout << "Select Statement Not Yet Implemented" << std::endl;
}

floppy_data_type convertColumnType(ColumnType t) {
   switch(t){
      case(INT) :
         return TYPE_INT;
      case(FLOAT) :
         return TYPE_FLOAT;
      case(DATETIME) :
         return TYPE_DATETIME;
      case(VARCHAR) :
         return TYPE_VARCHAR;
      case(BOOLEAN) :
         return TYPE_BOOLEAN;
   }
}


void insert(Buffer *buf, FLOPPYInsertStatement *statement) {
   // Get the table description
   int target_fd = getFileDescriptorForTable(buf, statement->name);
   tableDescription td;
   printf("Getting table desc buf\n");
   getTableDescription(buf, target_fd, &td);

   printf("INSERT INTO %d\n", target_fd);

   // Get the size of the record:

   // Create a buffer for the record's description
   char* rd = new char[2048];
   // get the rd
   int rd_size = heapHeaderGetRecordDesc(target_fd, buf, rd);
   int record_size = heapHeaderGetRecordSize(target_fd, buf);

   char* record = new char[2048];

   // Now we have the rd, rdSize, and the field name

   // Iterate over the attributes of the FLOPPYInsertStatement and 
   //   call setField, passing the buffer
   // call insertRecord
   std::vector<FLOPPYValue*> values = *statement->values;
   int index = 0;
   Attribute* list = td.attributeList;
   while (list != NULL) {
      FLOPPYValue* val = values[index++];
      switch (val->type()) {
         case ValueType::StringValue:
            printf("String\n");
            setField(list->attName, record, rd,
             rd_size, val->sVal);
            break;
         case ValueType::IntValue:
            /*
            setField(val->tableAttribute->attribute, record, rd,
             rd_size, val->iVal);
            */
            break;
         case ValueType::FloatValue:
            /*
            setField(val->tableAttribute->attribute, record, rd,
             rd_size, val->fVal);
            */
            break;
         case ValueType::BooleanValue:
            /*
            setField(val->tableAttribute->attribute, record, rd,
             rd_size, val->bVal);
             */
            break;
         case ValueType::NullValue:
            break;
         default:
            printf("Default case\n");
      }

      list = list->next;
   }

   printf("Iterating over the values\n");
   printf("Finished iterating\n");

   DiskAddress recordLocation;
   //insertRecord(buf, char *tableName, char *record, DiskAddress *location);
   printf("insertin gthe record\n");
   insertRecord(buf, statement->name, record, &recordLocation);
}

void createTable(Buffer *buf, FLOPPYCreateTableStatement *statement) {
   tableDescription td;
   strcpy(td.tableName, statement->tableName.c_str());

   Attribute *current;
   std::vector<FLOPPYCreateColumn *> columns = *(statement->columns);
   td.attributeList = NULL;

   for(int i = columns.size() - 1; i >= 0; i--){
      current = new Attribute();
      current->attName = columns[i]->name;
      current->attSize = columns[i]->size;
      current->attType = convertColumnType(columns[i]->type);
      current->next = td.attributeList;
      td.attributeList = current;
   }

   std::vector<char *> primKeys = *(statement->pk->attributes);
   td.pKey = NULL;

   for(int i = primKeys.size() - 1; i >= 0; i--){
      current = new Attribute();
      current->attName = primKeys[i];
      //attribute type probably not needed
      current->next = td.pKey;
      td.pKey = current;
   }


   //Per the email from alex, I am leaving foreign keys out of tables
   /*
   foreignKeys *currentfk;
   std::vector<FLOPPYForeignKey *> fks  = *(statement->fk);
   td.fkeys = NULL;
   for(int i = fks.size() - 1; i >= 0; i--){
      currentfk = new foreignKeys();
      currentfk->tableName = fks[i]->refTableName;
      //add attributes later
      currentfk.next = td.attributeList;
      td.attributeList = current;
   }
   */
   
   printf("TABLE JUST ADDED: %s\n", td.tableName);

   td.isVolatile = 0;
   if(createPersistentTable(buf, td) == E_TABLE_EXISTS) {
      std::cout << "Table already exists" << std::endl;
   }
}

void dropTable(Buffer *buf, FLOPPYDropTableStatement *statement) {
   deleteHeapFile(buf, statement->table);
}

/*
 * Takes in a statement as a string, parses it and performs the
 * required action using helper methods 
 */
void executeStatement(Buffer *buf, char *statement) {
   FLOPPYOutput *output = FLOPPYParser::parseFLOPPYString(statement);
   if (output->isValid) { // Use isValid flag to detect if parser parsed correctly.
      switch(output->statement->type()) {
         case(StatementType::ErrorStatement) : 
            std::cout << "Found Error Statement" << std::endl;
            break;
         case(StatementType::CreateTableStatement) :
            std::cout << "Found Create Table Statement" << std::endl;
            createTable(buf, (FLOPPYCreateTableStatement *)output->statement);
            break;
         case(StatementType::DropTableStatement) :
            std::cout << "Found Drop Table Statement" << std::endl;
            dropTable(buf, (FLOPPYDropTableStatement *)output->statement);
            break;
         case(StatementType::CreateIndexStatement) :
            std::cout << "Found Create Index Statement" << std::endl;
            break;
         case(StatementType::DropIndexStatement) :
            std::cout << "Found Drop Index Statement" << std::endl;
            break;
         case(StatementType::InsertStatement) :
            std::cout << "Found Insert Statement" << std::endl;
            //dropTable(buf, (FLOPPYInsertStatement *)output->statement);
            insert(buf, (FLOPPYInsertStatement*)output->statement);
            break;
         case(StatementType::DeleteStatement) :
            std::cout << "Found Delete Statement" << std::endl;
            break;
         case(StatementType::UpdateStatement) :
            std::cout << "Found Update Statement" << std::endl;
            break;
         case(StatementType::SelectStatement) :
            std::cout << "Found Select Statement" << std::endl;
            selectStatement(buf, (FLOPPYSelectStatement *)output->statement);
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
   tfs_mkfs("floppyTest.dsk", DEFAULT_DISK_SIZE);
   tfs_mount("floppyTest.dsk");
   Buffer *buf = new Buffer();

   commence("floppyTest.dsk", buf, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);

   /*
   executeStatement(buf, "CREATE TABLE list (LastName VARCHAR(16), FirstName VARCHAR(16), grade INT, classroom INT, PRIMARY KEY(FirstName, LastName));");
    */
   executeStatement(buf, "CREATE TABLE scott(foo VARCHAR(20), bar VARCHAR(50), PRIMARY KEY(foo));");
   executeStatement(buf, "INSERT INTO scott VALUES('abcd', 'efgh');");
   //executeStatement(buf, "SELECT * FROM scott;");

   executeStatement(buf, "DROP TABLE scott;");

   std::cout << "done" << std::endl;

   tfs_unmount();
}
