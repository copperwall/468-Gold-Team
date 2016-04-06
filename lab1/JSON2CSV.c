#include "JSON2CSV.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table tables[MAX_TABLES];
int numTables;
int firstObject;

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

/**
 * Copies the given primary key and returns a new key with an extra depth with
 * value |key|.
 */
void add_to_key(PrimaryKey *pk, PrimaryKey *newPk, int key) {
   memcpy(newPk, pk, sizeof(PrimaryKey));
   newPk->key[newPk->numKeys++] = key;
}

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
   memcpy(&table->records[table->numRecords++], record, sizeof(Record));
}

/**
 * Given a table and a string key, find the index
 * that column belongs at in the record.
 */
int table_lookup_index_for_key(Table *table, char *key) {
   int i = 0;
   for (; i < table->numAttributes; i++) {
      if (strcmp(key, table->colNames[i]) == 0) {
         return i;
      }
   }

   // Not found.
   return -1;
}

// TODO: Either include primary keys when creating record or put primary as
// part of table and prepend primary key values before other columns.
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

// TODO:
// Consider if pk needs to be a pointer. I'm not sure it does.
void readObjects(json_t *root, PrimaryKey *pk, Table *table) {
   int json_type, outer = 0;
   size_t index, record_index;
   const char *key;
   char value_str[MAX_TABLE_NAME];
   json_t *value, *array_element;
   Table *nextTable;
   PrimaryKey newPk;
   Record record;

   if (pk->numKeys == 0) {
      // This means that this is the top level of the JSON object.
      // This is pseudo code, consider adding an outer flag
      // Need to add the current id attribute to the primary key
      add_to_key(pk, pk, json_integer_value(json_object_get(root, "id")));
   }

   json_object_foreach(root, key, value) {
      // If is atomic
      // atmoic is string, int, boolean
      json_type = json_typeof(value);
      switch (json_type) {
         case JSON_STRING:
            strcpy(value_str, json_string_value(value));
            break;
         case JSON_INTEGER:
            sprintf(value_str, "%d", (int)json_integer_value(value));
            break;
         case JSON_REAL:
            sprintf(value_str, "%0.2f", json_real_value(value));
            break;
         case JSON_TRUE:
            strcpy(value_str, "true");
            break;
         case JSON_FALSE:
            strcpy(value_str, "false");
            break;
         case JSON_NULL:
            strcpy(value_str, "null");
            break;
         case JSON_OBJECT:
            // Need to add table and initialize
            nextTable = add_table(strncat(table->tableName, key, MAX_TABLE_NAME));
            parse(value, pk, nextTable);
            break;
         case JSON_ARRAY:
            nextTable = add_table(strncat(table->tableName, key, MAX_TABLE_NAME));
            json_array_foreach(value, index, array_element) {
               add_to_key(pk, &newPk, index);
               parse(array_element, &newPk, nextTable);
            }
            break;
      }

      if (json_type != JSON_OBJECT && json_type != JSON_ARRAY) {
         // TODO: Something needs to be done to add each primary key before the
         // first columns in each table.
         if (firstObject) {
            // Add key to table if this is the first object in the collection.
            table_add_key(table, (char *)key);
         }
         // Add attribute key and value to record
         record_index = table_lookup_index_for_key(table, (char *)key);
         record_add(&record, record_index, value_str);
      }
   }

   // Add this point all fields have been iterated over, and the record should
   // be complete. Add record to the table.
   table_add_record(table, &record);
}

void parse(json_t *root, PrimaryKey* pk, Table *table) {

}


int main() {
   return 0;
}
