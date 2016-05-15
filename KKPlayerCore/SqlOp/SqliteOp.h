#include "../sqlite/sqlite3.h"
#ifndef SqliteOp_H_
#define VqlOp_H_
class CSqliteOp 
{
   public:
        	CSqliteOp  ();
			~CSqliteOp  (void);
			//初始化一个DB
			int OpenDB(char* path,sqlite3** pDb);
			//创建表
			int CreateTable(sqlite3 *db,char* sqltxt);

			//判断表是否存在；
			int IsTableExt(sqlite3 *db,char* name);
			int NoSelectSql(sqlite3 *db,char* sqltxt);
			int SelectSql(sqlite3 *db,char *sqltxt,char** pResult);
			void FreeSelectSql(char** pResult);
   private:  
	        char m_Path[256];
};
#endif