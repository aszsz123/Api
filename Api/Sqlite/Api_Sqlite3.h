#ifndef _API_SQLITE3_
#define _API_SQLITE3_

int ProcSql(char *sql, char *get);
int GetData(char *table, char *schema, char *schemaVal, char *kind ,char *get);
int PutData(char *table, char *schema, char *schemaVal, char *data, char *dataVal);
int MakeColumn(char *table, char *schema, char *data);
int CreateTable(char *(*table)[2]);

#define SCHEMA_TYPE_PRIMARY_KEY "INTEGER PRIMARY KEY AUTOINCREMENT not null"
#define SCHEMA_TYPE_STR "text not null default 'NULL'"
#define SCHEMA_TYPE_INT "integer"
#define SCHEMA_TYPE_BLOB "blob"

#define DATA_DAY "strftime('\%Y-\%m-\%d', 'now', 'localtime')" 
#define DATA_TIME "strftime('\%H:\%M:\%S', 'now', 'localtime')"  


#endif
