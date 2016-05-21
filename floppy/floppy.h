/* FLOPPY */

#include <stdint.h>

#define E_TABLE_EXISTS -2

typedef enum {INT, FLOAT, VARCHAR, DATETIME, BOOLEAN} floppy_data_type;

/**
 * Table creation structs
 */

typedef struct Attribute {
   char *attName;             // Attribute name
   floppy_data_type attType;  // Attribute type
   uint8_t attSize;           // Attribute size (this is only used for varchars) 1 byte
   struct Attribute *next;    // Next Attribute in linked list
} Attribute;

typedef struct FK {
   char *tableName;           // The table the foreign key is referencing
   Attribute *key;            // The foreign key itself
   struct FK *next;           // The next foreign key in the linked list
} foreignKeys;

typedef struct Tables {
   char *tableName;           // The table name
   Attribute *attributeList;  // The pointer to the first Attribute in a list
   Attribute *pKey;           // The pointer to the first Attribute of the PK
   foreignKeys *fKeys;        // The pointer to the first FK
   int isVolatile;            // Boolean for whether or not the table is volatile
   int fd;                    // The file descriptor for this table.
   struct Tables *next;       // The next table in the linked list
} tableDescription;
