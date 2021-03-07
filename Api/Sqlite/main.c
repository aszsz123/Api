#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Api_Sqlite3.h"


char *table_schema[][2] = {
	{"config", 0}, //테이블 이름

	{"id"	, SCHEMA_TYPE_PRIMARY_KEY},
	{"day"	, SCHEMA_TYPE_STR},
	{"time"	, SCHEMA_TYPE_STR},
	{"type"	, SCHEMA_TYPE_STR},
	{"data"	, SCHEMA_TYPE_STR},
	{0,0},
};


int main (void)
{	
	char data[256]={0,};
	CreateTable(table_schema);
	MakeColumn("config","type","test1");
	PutData("config", "type","'test1'","day",DATA_DAY);
	PutData("config", "type","'test1'","time",DATA_TIME);
	PutData("config", "type","'test1'","data","'123'");
	ProcSql("select * from config;",0);
	
	GetData("config", "type", "'test1'", "data", data);
	printf("get [%s]\r\n", data );
	return 1;
}
