#include "HistoryInfoMgr.h"
#include "../KKPlayerCore/KKPlayer.h"
#include "json/json.h"
#include <process.h>
#include <queue>
#include <list>
typedef struct SQL_LITE__
{
	char strSql[512];
}SQL_LITE__;

void SetKKplayerH264HardCodec(int value);
void SetKKplayerH265HardCodec(int value);
static std::queue<SQL_LITE__ *> m_sqlQue;
CHistoryInfoMgr* CHistoryInfoMgr::m_pInance=NULL;
CHistoryInfoMgr::CHistoryInfoMgr():m_nH264Codec(-1),m_nH265Codec(-1),m_nUselibRtmp(-1), m_nlibRtmpDelay(-1)
,m_hThread(0)
,m_bThOver(1)
,m_iThaddr(0)
,m_nRenderer(-1)
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

	  if(SqliteOp.IsTableExt(pDb,"AVTransferInfo")!=1)
	  {
		  char *str= "CREATE table AVTransferInfo(UrlInfo TEXT NOT NULL,Alias TEXT,Category TEXT, FileSize INTEGER,AcSize INTEGER,Speed INTEGER,primary key(UrlInfo));";
		  SqliteOp.CreateTable(pDb,str);
	  }
	  m_pDb=pDb;
	  if(!m_hThread)
	  {
		  m_bThOver=0;
	     m_hThread=(HANDLE)_beginthreadex(NULL, NULL, SqlOp_Thread, (LPVOID)this, 0,&m_iThaddr);
	  }
	  m_Lock.Unlock(); 

}

void CHistoryInfoMgr::UpdateTransferInfo(char *urlInfo,char* alias,char *category,unsigned int FileSize,unsigned int AcSize,int Speed)
{
	
	char *strformt="replace into AVTransferInfo(UrlInfo,Alias,Category,FileSize,AcSize,Speed) values (" \
	                                       "\"%s\",\"%s\",\"%s\", %d,      %d,    %d)";

	int len=1024*10;
	char* sqltxt =(char*)::malloc(len);
	memset(sqltxt,0,len);
	sprintf(sqltxt,strformt,urlInfo,alias,category,FileSize,AcSize,Speed);
	
	sqlite3* pDb=( sqlite3* )m_pDb;
	{
	  m_Lock.Lock();
	  SqliteOp.NoSelectSql(pDb,sqltxt);
	  m_Lock.Unlock();
	}
	free(sqltxt);
}

void CHistoryInfoMgr::GetAVTransferInfo(std::vector<AV_Transfer_Info *> &slQue,int Finish)
{

	char sqlstr[1024]="select * from AVTransferInfo";

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
    m_Lock.Unlock();

	//读出sqlite数据
	nIndex = nColumn;
	for(int i=0;i<nRow;i++)
	{
		AV_Transfer_Info * Item = (AV_Transfer_Info*)malloc(sizeof(AV_Transfer_Info));
		for(int j=0;j<nColumn;j++)
		{
			char *val=pResult[nIndex];
			if(j==0)
			{
				strcpy(Item->UrlInfo,val);
			}else if(j==1){
				strcpy(Item->Alias,val);
			}else if(j==2){
				strcpy(Item->Category,val);
			}else if(j==3){
				Item->FileSize=atoi(val);
			}else if(j==4){
				Item->AcSize=atoi(val);
			}else if(j==5){
				Item->Speed=atoi(val);
			}
			nIndex++;
		}
		slQue.push_back(Item);
	}
	sqlite3_free_table(pResult);
	
	return;
}


unsigned __stdcall  CHistoryInfoMgr::SqlOp_Thread(LPVOID lpParameter)
{
	CHistoryInfoMgr * pIn= (CHistoryInfoMgr *)lpParameter;
	pIn->SqlOpFun();
	return 1;
}
void CHistoryInfoMgr::SqlOpFun()
{


	 
	 m_bThOver=true;
}
/*******播放进度更新信息***********/
void CHistoryInfoMgr::UpDataAVinfo(const char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height)
{
    sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt *pStmt = 0;  

	
    char *str="replace into AVHisinfo(url,Img,Width,Height,lstTime,TotalTime) values (" \
               "\"%s\"," \
				"?," \
				"%d," \
				"%d," \
				"%d," \
				"%d)";
	

	char strsql[512]="";
	sprintf(strsql,str,strpath,width,height,curtime,totaltime);

	
	m_Lock.Lock();
	int ret=sqlite3_prepare(pDb, strsql, -1, &pStmt, 0);
	ret= sqlite3_bind_blob(pStmt, 1,Imgbuf,buflen, NULL);
	ret=sqlite3_step(pStmt);
	m_Lock.Unlock();
    sqlite3_finalize(pStmt);
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
	m_Lock.Unlock();
	sqlite3_finalize(pStmt);
	
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
		SetKKplayerH264HardCodec(value);
}
int CHistoryInfoMgr::GetH264Codec()
{
	if(m_nH264Codec==-1)
	{

		std::string selectIndex="";
		GetConfig("H264Codec",selectIndex);
		m_nH264Codec=atoi(selectIndex.c_str());
		SetKKplayerH264HardCodec(m_nH264Codec);
	}
    return m_nH264Codec;
		 
}
int CHistoryInfoMgr::GetH265Codec()
{
	if(m_nH265Codec==-1)
	{
		std::string selectIndex="";
		GetConfig("H265Codec",selectIndex);
		m_nH265Codec=atoi(selectIndex.c_str());
	
        SetKKplayerH265HardCodec(m_nH265Codec);
	}
    return  m_nH265Codec;
}
void CHistoryInfoMgr::UpdataH265Codec(int value)
{
        char hardM[512]="";
		sprintf(hardM,"%d",value);
		UpdataConfig("H265Codec",hardM);
		m_nH265Codec=value;
		
        SetKKplayerH265HardCodec(value);
}

int  CHistoryInfoMgr::GetUseLibRtmp()
{
	if(m_nUselibRtmp==-1){
	      std::string tempStr="";
		 GetConfig("UseLibRtmp",tempStr);
	     m_nUselibRtmp=atoi( tempStr.c_str());
	}
	return m_nUselibRtmp;
}

 void CHistoryInfoMgr::UpdataUseLibRtmp(int value)
 {
        char hardM[512]="";
		sprintf(hardM,"%d",value);
		UpdataConfig("UseLibRtmp",hardM);
		m_nUselibRtmp=value;
		
 }

int CHistoryInfoMgr::GetRtmpDelay()
{
    if(m_nlibRtmpDelay==-1){
	    std::string tempStr="";
		GetConfig("Rtmpdelay",tempStr);
	    m_nlibRtmpDelay=atoi( tempStr.c_str());
		if(m_nlibRtmpDelay<1)
			m_nlibRtmpDelay=1;
	}
return  m_nlibRtmpDelay;
}
void CHistoryInfoMgr::UpdataRtmpDelay(int value)
{
        char hardM[512]="";
		sprintf(hardM,"%d",value);
		UpdataConfig("Rtmpdelay",hardM);
		 m_nlibRtmpDelay=value;
}
int  CHistoryInfoMgr::GetRendererMethod()
{
	if(m_nRenderer==-1){
	    std::string tempStr="";
		GetConfig("Renderer",tempStr);
	    m_nRenderer=atoi( tempStr.c_str());
	}
    return m_nRenderer;
}
void CHistoryInfoMgr::UpdataRendererMethod(int value)
{
        char hardM[512]="";
		sprintf(hardM,"%d",value);
		UpdataConfig("Renderer",hardM);
		m_nRenderer=value;
}