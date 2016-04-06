#include "JSON2CSV.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table tables[MAX_TABLES];
int numTables;

void record_add(Record *record, char *key, char *value) {

}

char *record_get(Record *record, char *key) {
   return "lol";
}

Table *add_table(char *table_name) {
   // Need to make a list of table with a size,
   // This will initialize the next size and return a pointer to it.
   Table *lol;
   return lol;
}

void table_add_key(Table *table, char *key) {

}

void table_add_record(Table *table, Record *record) {

}

int table_lookup_index_for_key(Table *table, char *key) {
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
