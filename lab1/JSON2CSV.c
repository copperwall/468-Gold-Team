#include "JSON2CSV.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table tables[MAX_TABLES];
int numTables;
int currentObject;

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
   char *temp = malloc(MAX_TABLE_NAME);
   strcpy(temp, value);
   printf("Adding value %s to idx %d\n", value, idx);

   r->value[idx] = temp;
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

   int i;
      printf("Transferring record to table\n");
   for (i = 0; i < record->numAttributes; i++) {
      printf("%s\n", record->value[i]);
   }

   /* Record *thing = &table->records[table->numRecords - 1]; */

   /* printf("This is the table record\n"); */
   /* for (i = 0; i < thing->numAttributes; i++) { */
   /*    printf("%s\n", thing->value[i]); */
   /* } */
}

Table *get_table(char *name) {
   int i;

   for (i = 0; i < numTables; i++) {
      if (!strcmp(tables[i].tableName, name)) {
         return &tables[i];
      }
   }

   return NULL;
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
         printf("col name %s\n", current.colNames[j]);
         fprintf(fp, "%s, ", current.colNames[j]);
         j++;
      }
      fseek(fp, -2, SEEK_CUR);
      fprintf(fp, "\n");

      printf("numrecords, %d\n", current.numRecords);
      j = 0;
      while(j < current.numRecords) {
         k = 0;
         printf("attributes %d\n", current.numAttributes);
         while(k < current.numAttributes) {
            printf("value %s\n", current.records[j].value[k]);
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
// Primary keys need to be added to Record before the other keys.
//
// Tables should not be created after the first object.
//
// If it isn't the first object do a lookup on the global list of tables for
// the name that it should have.
void readObjects(json_t *root, PrimaryKey *pk, Table *table) {
   int json_type, i, outer = 0;
   size_t index, record_index;
   const char *key;
   char value_str[MAX_TABLE_NAME], key_name[MAX_TABLE_NAME], table_name[MAX_TABLE_NAME];
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

   // Add primary keys to table keys
   i = 0;

   if (currentObject == 0) {
      table_add_key(table, "id");
   }

   sprintf(key_name, "%d", pk->key[i]);
   printf("Adding id key %s\n", key_name);
   record_index = table_lookup_index_for_key(table, "id");
   record_add(&record, 0, key_name);

   for (i = 1; i < pk->numKeys; i++) {
      sprintf(key_name, "position%d", i);
      if (currentObject == 0) {
         table_add_key(table, key_name);
      }

      sprintf(key_name, "%d", pk->key[i]);
      printf("Adding position key: %s\n", key_name);
      record_add(&record, i, key_name);
   }

   json_object_foreach(root, key, value) {
      json_type = json_typeof(value);
      switch (json_type) {
         case JSON_STRING:
            strcpy(value_str, json_string_value(value));
            printf("Found string %s\n", value_str);
            break;
         case JSON_INTEGER:
            sprintf(value_str, "%d", (int)json_integer_value(value));
            printf("Found int %s\n", value_str);
            break;
         case JSON_REAL:
            sprintf(value_str, "%0.2f", json_real_value(value));
            printf("Found double %s\n", value_str);
            break;
         case JSON_TRUE:
            strcpy(value_str, "true");
            printf("Found true %s\n", value_str);
            break;
         case JSON_FALSE:
            strcpy(value_str, "false");
            printf("Found false %s\n", value_str);
            break;
         case JSON_NULL:
            strcpy(value_str, "null");
            printf("Found null %s\n", value_str);
            break;
         case JSON_OBJECT:
            // Need to add table and initialize
            if (currentObject == 0) {
               sprintf(table_name, "%s%s", table->tableName, key);
               nextTable = add_table(table_name);
            } else {
               nextTable = get_table(table_name);
            }
            parse(value, pk, nextTable);
            break;
         case JSON_ARRAY:
            sprintf(table_name, "%s%s", table->tableName, key);

            if (currentObject == 0) {
               nextTable = add_table(table_name);
            } else {
               nextTable = get_table(table_name);
            }
            json_array_foreach(value, index, array_element) {
               add_to_key(pk, &newPk, index);
               parse(array_element, &newPk, nextTable);
            }
            break;
      }

      if (json_type != JSON_OBJECT && json_type != JSON_ARRAY && strcmp(key, "id")) {
         // TODO: Something needs to be done to add each primary key before the
         // first columns in each table.
         if (currentObject == 0) {
            // Add key to table if this is the first object in the collection.
            table_add_key(table, (char *)key);
         }
         // Add attribute key and value to record
         record_index = table_lookup_index_for_key(table, (char *)key);
         printf("About to add value %s for key %s\n", value_str, key);
         record_add(&record, record_index, value_str);
      }
   }

   printf("Table debug\n");
   for (i = 0; i < table->numAttributes; i++) {
      printf("%s\n", table->colNames[i]);
   }
   printf("Record debug\n");
   for (i = 0; i < record.numAttributes; i++) {
      printf("%s\n", record.value[i]);
   }

   // Add this point all fields have been iterated over, and the record should
   // be complete. Add record to the table.
   table_add_record(table, &record);
}

void parse(json_t *root, PrimaryKey* pk, Table *table) {

}


int main(int argc, char *argv[]) {
   // Table: create with filename as tablename
   // PrimaryKey: Initialize with 0 numKeys
   json_t *root = json_load_file(argv[1], 0, NULL);
   PrimaryKey pk;
   Table *table = add_table(argv[1]);

   pk.numKeys = 0;

   currentObject = 0;
   readObjects(json_array_get(root, 0), &pk, table);

   serialize();
   return 0;
}
