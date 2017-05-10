#include "../stdafx.h"
#include <stdlib.h>
#include "SqliteOp.h"

#ifdef _DEBUG
#pragma comment (lib,"../Debug/kksqlite.lib")
#else
#pragma comment (lib,"../Release/kkSqlite.lib")
#endif
CSqliteOp ::CSqliteOp()
{

}
CSqliteOp ::~CSqliteOp  (void)
{

}
//初始化一个DB
int CSqliteOp::OpenDB(const char* path,sqlite3** pDb)
{

	sqlite3_open(path,pDb);
	if(pDb==NULL)
      return 0;
	return 1;
}

int CSqliteOp::CreateTable(sqlite3 *db,char* sqltxt)
{
	
	char errstr[256]={};
	char *perrstr=errstr;

	int result= sqlite3_exec(
		db,               /* An open database */
		sqltxt,
		NULL,    
		NULL,     
		&perrstr     
		);
	if(SQLITE_OK!=result)
	{
		//输出sqlite3错误信息
		//TRACE0(perrstr);
	}
	return result;
}

int CSqliteOp::IsTableExt(sqlite3 *db,char* name)
{
	
	// 打开或创建数据库
	if(db==NULL)
	{
		return -1;
	}

	char sqltxt[256]="";
	strcpy(sqltxt,"SELECT count(*) as num FROM sqlite_master WHERE type='table' AND name='");
	strcat(sqltxt,name);
	strcat(sqltxt,"' ");
	char** pResult=NULL;
	int nRow=-1;
	int nColumn=-1;
	sqlite3_get_table(
		db,               /* An open database */
		sqltxt ,     /* SQL to be evaluated */
		&pResult,    /* Results of the query */
		&nRow,           /* Number of result rows written here */
		&nColumn,        /* Number of result columns written here */
		NULL       /* Error msg written here */
		);
	char strOut[512]="";
	int nIndex = nColumn;
	if(nRow>-1&&nColumn>-1&&pResult!=NULL)
	{
		strcat(strOut,pResult[nColumn]);
	}
	sqlite3_free_table(pResult);
	pResult=NULL;
	int Ok=atoi(strOut);
	//如果表不存在则创建
	if(Ok<1)
	{
		return 0;
	}
	return 1;
}

int CSqliteOp::NoSelectSql(sqlite3 *db,char* sqltxt)
{
	char** pResult;
	int nRow=-1;
	int nColumn=-1;
	int nIndex = nColumn;

	//sqlite3_free_table()
	int result= sqlite3_get_table(
		db,               /* An open database */
		sqltxt,     /* SQL to be evaluated */
		&pResult,    /* Results of the query */
		&nRow,           /* Number of result rows written here */
		&nColumn,        /* Number of result columns written here */
		NULL       /* Error msg written here */
		);
	//result != SQLITE_OK
	return result;
}

//执行sql操作
int CSqliteOp::SelectSql(sqlite3 *db,char *sqltxt,char** pResult)
{
	
	int nRow=-1;
	int nColumn=-1;
	int nIndex = nColumn;

	//分析sql语句
	sqlite3_stmt* stmt2 = NULL;
	if (sqlite3_prepare_v2(db,sqltxt,strlen(sqltxt),&stmt2,NULL) != SQLITE_OK) {
		if (stmt2)
			sqlite3_finalize(stmt2);
		return -1;
	}

	

	int result= sqlite3_get_table(
		db,               /* An open database */
		sqltxt,     /* SQL to be evaluated */
		&pResult,    /* Results of the query */
		&nRow,           /* Number of result rows written here */
		&nColumn,        /* Number of result columns written here */
		NULL       /* Error msg written here */
		);
	//sqlite3_free_table(pResult);
	return 0;
}

void CSqliteOp::FreeSelectSql(char** pResult)
{
       sqlite3_free_table(pResult);
}