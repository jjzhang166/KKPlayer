#include "HistoryInfoMgr.h"
CHistoryInfoMgr* CHistoryInfoMgr::m_pInance=NULL;
CHistoryInfoMgr::CHistoryInfoMgr()
{

}
CHistoryInfoMgr::~CHistoryInfoMgr()
{

}
void CHistoryInfoMgr::SetPath(const char *Path)
{
	strcpy(m_strDbPath,Path);
}
	
void CHistoryInfoMgr::InitDb()
{
      sqlite3* pDb;
	  m_Lock.Lock();
	  SqliteOp.OpenDB(m_strDbPath,&pDb);
	  if(SqliteOp.IsTableExt(pDb,"AVHisinfo")!=1){
		  char *str= "CREATE table AVHisinfo(url TEXT NOT NULL,Img BLOB,Width INTEGER,Height INTEGER,lstTime INTEGER,TotalTime INTEGER,primary key(url));";
		  SqliteOp.CreateTable(pDb,str);
	  }
	  m_pDb=pDb;
	  m_Lock.Unlock();
}
/*******播放进度更新信息***********/
void CHistoryInfoMgr::UpDataAVinfo(char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height)
{
    sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt *pStmt = 0;  

	m_Lock.Lock();
    char *str="replace into AVHisinfo(url,Img,Width,Height,lstTime,TotalTime) values (" \
               "\"%s\"," \
				"?," \
				"%d," \
				"%d," \
				"%d," \
				"%d)";
	

	char strsql[512]="";
	sprintf(strsql,str,strpath,width,height,curtime,totaltime);

	
	int ret=sqlite3_prepare(pDb, strsql, -1, &pStmt, 0);
	ret= sqlite3_bind_blob(pStmt, 1,Imgbuf,buflen, NULL);
	ret=sqlite3_step(pStmt);;
    sqlite3_finalize(pStmt);
	m_Lock.Unlock();

}    
//获取放播的历史信息
void CHistoryInfoMgr::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
    sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt *pStmt = 0;
	m_Lock.Lock();
	int ret=sqlite3_prepare(pDb, "select * from AVHisinfo", -1, &pStmt, 0);

	int size=0;
	int SlLen=sizeof(AV_Hos_Info);
    //int ret=sqlite3_bind_blob(pStmt, 2, buf, 100000, NULL);
	while(SQLITE_ROW ==sqlite3_step(pStmt))
	{
		AV_Hos_Info *sl=(AV_Hos_Info *)::malloc(SlLen);
		memset(sl,0,SlLen);

		int index=0;
		const unsigned char *pPath= sqlite3_column_text(pStmt, index++);
		strcpy(sl->url,(char*)pPath);

		
		//img
		const void *test = sqlite3_column_blob(pStmt, index);
		size = sqlite3_column_bytes(pStmt, index++);
		sl->pBuffer=(unsigned char *)::malloc(size);
		memcpy(sl->pBuffer,test,size);
		sl->bufLen=size;

        sl->width=sqlite3_column_int(pStmt,index++);
		sl->height=sqlite3_column_int(pStmt,index++);

		sl->CurTime=sqlite3_column_int(pStmt,index++);
		sl->TotalTime=sqlite3_column_int(pStmt,index++);

		slQue.push_back(sl);

	}
	sqlite3_finalize(pStmt);
	m_Lock.Unlock();
}
CHistoryInfoMgr *CHistoryInfoMgr::GetInance()
{
   
   if(m_pInance==NULL){
       m_pInance=new CHistoryInfoMgr();
   }
 
   return m_pInance;
}