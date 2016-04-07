#include "jansson-2.7/build/include/jansson.h"
#define MAX_COLUMNS 20
#define MAX_RECORDS 100
#define MAX_ARRAY_DEPTH 5
#define MAX_TABLES 15
#define MAX_TABLE_NAME 100


typedef struct Record {
	char *value[MAX_COLUMNS];
	int numAttributes;
} Record;

typedef struct Table {
	char *tableName;
	char *colNames[MAX_COLUMNS];
	Record records[MAX_RECORDS];
	int numAttributes;
   int numRecords;
} Table;


typedef struct PrimaryKey{
	int key[MAX_ARRAY_DEPTH];
	int numKeys;
} PrimaryKey;

void add_to_key(PrimaryKey *pk, PrimaryKey *newPk, int key);

// Adding and getting values from a Record
//void record_add(Record *record, char *key, char *value);
void record_add(Record *r, int idx, char *value);
//char *record_get(Record *record, char *key);
char *record_get(Record *r, int idx);
void initialize_record(Record *r);

// Table methods

// This should be called when parse recurses into a new embedded
// object/array and needs a new table created.
// Should this have a primary key parameter?
// Or should that just be left for table_add_key?
// That’s probably fine.
Table *add_table(char *table_name);
void initialize_table(Table *table, char *table_name);

// Add a key to the table, should only happen on the first object
void table_add_key(Table *table, char *key);
// Add a record to the table
void table_add_record(Table *table, Record *record);
// Given a table name, return a Table * to the table with that name.
Table *get_table(char *name);

// Given a key, return its index in the table->columns
int table_lookup_index_for_key(Table *table, char *key);

void serialize();

void parse(json_t *root, PrimaryKey *pk, Table *table);

void readObjects(json_t *root, PrimaryKey *pk, Table *table);
void readArrays(json_t *root, PrimaryKey *pk, Table *table);
