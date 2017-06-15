#include "HistoryInfoMgr.h"
CHistoryInfoMgr* CHistoryInfoMgr::m_pInance=NULL;
CHistoryInfoMgr::CHistoryInfoMgr():m_nH264Codec(0),m_nH265Codec(0)
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

	  if(SqliteOp.IsTableExt(pDb,"ConfigInfo")!=1){
		  char *str= "CREATE table ConfigInfo(Key TEXT NOT NULL,Value TEXT,primary key(Key));";
		  SqliteOp.CreateTable(pDb,str);
	  }

	  m_pDb=pDb;
	  m_Lock.Unlock();
}
/*******播放进度更新信息***********/
void CHistoryInfoMgr::UpDataAVinfo(const char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height)
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
		sl->url=(char*)::malloc(2048);
		memset(sl->url,0,2048);
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
void CHistoryInfoMgr::UpdataConfig(const char* StrKey,const char* StrValue)
{
    sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt *pStmt = 0;  

	
    char *str="replace into ConfigInfo(Key,Value) values (" \
               "\"%s\"," \
				"\"%s\");";
	
    char strsql[512]="";
	sprintf(strsql,str,StrKey,StrValue);
	m_Lock.Lock();
	
	SqliteOp.NoSelectSql(pDb,strsql);
	m_Lock.Unlock();
}
bool CHistoryInfoMgr::GetConfig(const char* StrKey,std::string &OutValue)
{
    char sqlstr[1024]="select * from ConfigInfo where Key=\"";
    bool Ok =false;
	
	strcat(sqlstr,StrKey);
	strcat(sqlstr,"\";");
	int nRow=-1,nColumn=-1,nIndex=-1;
	char** pResult=NULL;
	m_Lock.Lock();
	sqlite3* pDb=( sqlite3* )m_pDb;
	int result= sqlite3_get_table(
		pDb,               /* An open database */
		sqlstr,     /* SQL to be evaluated */
		&pResult,    /* Results of the query */
		&nRow,           /* Number of result rows written here */
		&nColumn,        /* Number of result columns written here */
		NULL       /* Error msg written here */
		);


	//读出sqlite数据
	nIndex = nColumn;
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nColumn;j++)
		{
			char *val=pResult[nIndex];
			if(j==0){
				
			}else if(j==1){
				OutValue=val;
			}
			nIndex++;
		}
		Ok=true;
	}
	sqlite3_free_table(pResult);
	m_Lock.Unlock();

	return Ok;
}
CHistoryInfoMgr *CHistoryInfoMgr::GetInance()
{
   
   if(m_pInance==NULL){
       m_pInance=new CHistoryInfoMgr();
   }
 
   return m_pInance;
}
void CHistoryInfoMgr::UpdataH264Codec(int value)
{
       char hardM[512]="";
		sprintf(hardM,"%d",value);
		UpdataConfig("H264Codec",hardM);
		m_nH264Codec=value;
}
int CHistoryInfoMgr::GetH264Codec()
{
	if(m_nH264Codec==-1)
	{
		std::string selectIndex="";
		InfoMgr->GetConfig("H264Codec",selectIndex);
		m_nH264Codec=atoi(selectIndex.c_str());
	}
    return m_nH264Codec;
		 
}
int CHistoryInfoMgr::GetH265Codec()
{
	if(m_nH264Codec==-1)
	{
		std::string selectIndex="";
		InfoMgr->GetConfig("H265Codec",selectIndex);
		m_nH265Codec=atoi(selectIndex.c_str());
	}
    return  m_nH265Codec;
}
void CHistoryInfoMgr::UpdataH265Codec(int value)
{
        char hardM[512]="";
		sprintf(hardM,"%d",value);
		UpdataConfig("H265Codec",hardM);
		m_nH265Codec=value;
}