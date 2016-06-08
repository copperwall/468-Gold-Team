/* Tables */

/* Create table prototypes */
int createPersistentTable(Buffer *buf, tableDescription table);
int createVolatileTable(Buffer *buf, tableDescription table);
int getFileDescriptorForTable(Buffer *buf, char *name);
tableDescription *getTableDescription(Buffer *buf, char *tableName, tableDescription *out);
