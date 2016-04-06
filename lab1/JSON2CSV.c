#include "JSON2CSV.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table tables[MAX_TABLES];
int numTables;

/**
 * Add a value to a record.
 * Question from Scott: Why is this a key->value mapping? Records don't have a
 *                      key attribute, only an array of values. That's an
 *                      attribute of the Table struct, which we don't have.
 *                      Perhaps it would be a good idea to instead
 *                      use table_lookup_index_for_key() first
 *                      which returns the spot in the record the value should
 *                      go, and then change this to
 *                      record_add(Record *r, int idx, char *value) and use the
 *                      index returned from GetColumnIndex as the idx.
 * I'm doing this below just in case you agree.
 */
/*
void record_add(Record *record, char *key, char *value) {
}
*/

void record_add(Record *r, int idx, char *value) {
   r->value[idx] = value;
   if (idx + 1 > r->numAttributes) {
      r->numAttributes = idx + 1;
   }
}

/**
 * Given a record and an index, return the
 * value of a column.
 */
char *record_get(Record *record, int idx) {
   return record->value[idx];
}

/**
 * Initialize a new record object.
 */
void initialize_record(Record *r) {
   r->numAttributes = 0;
}

Table *add_table(char *table_name) {
   // Need to make a list of table with a size,
   // This will initialize the next size and return a pointer to it.
   Table *lol = &tables[numTables++];
   initialize_table(lol, table_name);
   return lol;
}

/**
 * Initialzie the values of a table.
 */
void initialize_table(Table *table, char *name) {
   table->tableName = name;
   table->numAttributes = 0;
   table->numRecords = 0;
}

/**
 * Add a key to a table.
 */
void table_add_key(Table *table, char *key) {
   table->colNames[table->numAttributes++] = key;
}

/**
 * Add a Record into a table and advance relevant pointers.
 * NOTE: because of how our Table struct works, we are not
 *       just adding a reference to the record into the table,
 *       we're copying the record into the table's array.
 *       This means adding the record to the table should be
 *       done after we've done all the manipulations to it.
 */
void table_add_record(Table *table, Record *record) {
   memcpy(&table->records[table->numRecords++], record, sizeof(record));
}

/**
 * Given a table and a string key, find the index
 * that column belongs at in the record.
 */
int table_lookup_index_for_key(Table *table, char *key) {
   int i = 0;
   for (i; i < table->numAttributes; i++) {
      if (strcmp(key, table->colNames[i]) == 0) {
         return i;
      }
   }
   return 0; // lol
}

void serialize(){
   FILE *fp;
   char *fileName;
   Table current;
   int i = 0, j, k;

   while(i < numTables) {
      current = tables[i];
      fileName = malloc((strlen(current.tableName) + 5) * sizeof(char));
      strcat(fileName, current.tableName);
      strcat(fileName, ".csv");
      fp = fopen(fileName, "w+");
      
      //print out the colNames in the first row
      j = 0;
      while(j < current.numAttributes) {
         fprintf(fp, "%s, ", current.colNames[j]);   
         j++;
      }
      fseek(fp, -2, SEEK_CUR);
      fprintf(fp, "\n");

      j = 0;
      while(j < current.numRecords) {
         k = 0;
         while(k < current.numAttributes) {
            fprintf(fp, "%s, ", current.records[j].value[k]);
            k++;
         }
         fseek(fp, -2, SEEK_CUR);
         fprintf(fp, "\n");
         j++;
      }

      fclose(fp);
      free(fileName);
      i++;
   }

}


int main() {
   return 0;
}
