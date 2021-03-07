#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"
#include "Api_Sqlite3.h"

static sqlite3 *db;

int OpenDb(char *dbFileDir)
{
	int val=0;
	
	//printf("sqlite ver[%s]\r\n", sqlite3_libversion());

	val = sqlite3_open(dbFileDir, &db);
	//printf("[%s] Sqlite DB Open : %s\r\n", val==0?"OK":"FAIL", dbFileDir);
	
	if(val != 0)
	{
		sqlite3_close(db);
	}

	return val;
}

int CloseDb(void)
{
	return sqlite3_close(db);
}

/*
	userArg :	sqlite3_exec에서 4번째 인자의 값
	argc :	열 개수
	argv :	행 필드 값
	colName :	열의 이름
	
*/
int callback(void * userArg, int argc, char **argv, char **colName)
{
		
	for (int i=0; i<argc; i++)
	{
	//	printf("[%s]: [%s]\n",colName[i], argv[i] ? argv[i]: "NULL");
		if(userArg != 0)
			memcpy(userArg, argv[i], strlen(argv[i]) );
	}
	
	//printf("\n");

	return 0;
}


int ProcSql(char *sql, char *get)
{
	char *err_msg=0;
	
	if( OpenDb("./db") != 0)
		return -1;	

	if(sqlite3_exec(db, sql, callback, get, &err_msg) != 0)
	{
		printf("[FAIL] Sql : %s, %s\r\n", sql, err_msg);
		sqlite3_free(err_msg);
	}

	//printf("get [%s]\r\n",get);
	CloseDb();

	return 1;
}

int CreateTable(char *(*table)[2])
{
	char sql[1024];
	
	int cnt=1;

	memset(sql,0, 1024);
	sprintf(sql,"%s","create table if not exists ");
	
	//table name  null 
	if(table[0][0] == NULL)
		return -1;	

	//table name
	strcat(sql,table[0][0]);
	
	strcat(sql, " ( ");	
	while(1)
	{
		//last chaeck
		if(table[cnt][0] == 0)
		{
			//first schema null
			if(cnt == 1)
				return -1;
		
			break;
		}
		
		//schema name		
		strcat(sql,table[cnt][0]);
		strcat(sql," ");
		//schema type
		if(table[cnt][1] == 0)
		{	
			//default type
			strcat(sql,SCHEMA_TYPE_STR);
		}	
		else
		{	
			strcat(sql, table[cnt][1]);
		}	

		
		cnt++;
		if(table[cnt][1] != 0)
			strcat(sql,",");
		
	}
		
	strcat(sql,");");

	//printf("sql: [%s]\r\n", sql);

 	return ProcSql(sql,0);
}

int MakeColumn(char *table, char *schema, char *data)
{
	char sql[1024];
	memset(sql, 0, 1024);

	sprintf(sql,"insert into %s(%s) \
				select  '%s' \
				where not exists (select type from %s where type='%s');"
				, table, schema, data, table, data);

	return ProcSql(sql,0);
}

int PutData(char *table, char *schema, char *schemaVal, char *data, char *dataVal)
{
	char sql[1024];
	memset(sql, 0, 1024);
	
	sprintf(sql,"UPDATE %s SET %s=%s WHERE %s=%s;"
				, table, data, dataVal, schema, schemaVal);	
	
	return ProcSql(sql, NULL);
}

int GetData(char *table, char *schema, char *schemaVal, char *kind ,char *get)
{
	char sql[1024];
	memset(sql, 0, 1024);

	sprintf(sql,"select %s from %s where %s=%s", kind, table, schema, schemaVal);
		
	
//	printf("sql: [%s]\r\n", sql);
	return ProcSql(sql, get);
}
