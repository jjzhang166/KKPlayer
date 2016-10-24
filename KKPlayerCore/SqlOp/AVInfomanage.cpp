
#include "AVInfomanage.h"
#include "SqliteOp.h"
#include "../stdafx.h"
#include "../Includeffmpeg.h"
extern AVPixelFormat DstAVff;
CAVInfoManage::CAVInfoManage()
{
	m_Destbuffer=NULL;
	pimg_convert_ctx=NULL;
	m_ThStop=false;
    m_pDbOp = new CSqliteOp();
	memset(m_strDb,0,256);
	m_SKK_ThreadInfo.ThOver=true;
#ifdef WIN32_KK
	m_SKK_ThreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, SqlOp_Thread, (LPVOID)this, 0,&m_SKK_ThreadInfo.Addr);
#else
	m_SKK_ThreadInfo.Addr = pthread_create(&m_SKK_ThreadInfo.Tid_task, NULL, (void* (*)(void*))SqlOp_Thread, (LPVOID)this);
#endif
	if(m_pInance!=NULL){
        delete m_pInance;
	}
	
	m_pInance=this;
}
void CAVInfoManage::SetPath(char *Path)
{
	strcpy(m_strDb,Path);
}
CAVInfoManage::~CAVInfoManage()
{
	m_ThStop=true;
    ::av_free(m_Destbuffer);
	while(!m_SKK_ThreadInfo.ThOver)
	{
		Sleep(10);
	}
    CSqliteOp *DbOp =(CSqliteOp *)m_pDbOp;
    delete DbOp;
	m_pInance=NULL;

}

CAVInfoManage *CAVInfoManage::GetInance()
{
	return m_pInance;
}
CAVInfoManage *CAVInfoManage::m_pInance=NULL;

void CAVInfoManage::InitDb()
{
	  sqlite3* pDb;
	  CSqliteOp *pDbOp =(CSqliteOp *)m_pDbOp;
	  pDbOp->OpenDB(m_strDb,&pDb);
	  m_pDb=pDb;
	  if(pDbOp->IsTableExt(pDb,"AVinfoMap")!=1)
	  {
		  char *str= "CREATE table AVinfoMap(FilePath TEXT NOT NULL, FileMd5  TEXT,Img BLOB,Width INTEGER,Height INTEGER,lstTime INTEGER,TotalTime INTEGER,primary key(FilePath));";
		  pDbOp->CreateTable(pDb,str);
	  }

	  if(pDbOp->IsTableExt(pDb,"AVDownInfoMap")!=1)
	  {
		  char *str= "CREATE table AVDownInfoMap(FileInfo TEXT NOT NULL,Alias TEXT,Category TEXT, FileSize INTEGER,AcSize INTEGER,DownOK INTEGER,primary key(FileInfo));";
		  pDbOp->CreateTable(pDb,str);
	  }
}

void CAVInfoManage::UpDataAVinfo(char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height)
{
	return ;
	SQL_LITE__ *sl =(SQL_LITE__ *)::malloc(sizeof(SQL_LITE__));
	memset(sl,0,sizeof(SQL_LITE__));

	sl->pBuffer=(unsigned char*)::malloc(buflen);
	memcpy(sl->pBuffer,Imgbuf,buflen);
    sl->bufLen=buflen;
	sl->width=width;
	sl->height=height;
	sl->color=AV_COLOR;
	sl->SqlType=0;

    char *str="replace into AVinfoMap(FilePath,FileMd5,Img,Width,Height,lstTime,TotalTime) values (";
	memcpy(sl->strSql,str,strlen(str));

	char temp[512]="";
	snprintf(temp,512,"\"%s\",",strpath);
	strcat(sl->strSql,temp);

	strcat(sl->strSql,"\"\",");
	strcat(sl->strSql,"?,");

	snprintf(temp,512,"%d,",width);
	strcat(sl->strSql,"%d,");

	snprintf(temp,512,"%d,",height);
	strcat(sl->strSql,"%d,");

	snprintf(temp,512,"%d,",curtime);
	strcat(sl->strSql,temp);

	snprintf(temp,512,"%d)",totaltime);
	strcat(sl->strSql,temp);

	m_LockQue.Lock();
	m_sqlQue.push(sl);
	m_LockQue.Unlock();
		
}



void CAVInfoManage::UpdateDownAVinfo(char *strInfo,char* alias,char *category,unsigned int FileSize,unsigned int AcSize,int DownOk)
{
	SQL_LITE__ *sl =(SQL_LITE__ *)::malloc(sizeof(SQL_LITE__));
	memset(sl,0,sizeof(SQL_LITE__));
	sl->SqlType=1;

	char *str="replace into AVDownInfoMap(FileInfo,Alias,Category,FileSize,AcSize,DownOK) values (";
	memcpy(sl->strSql,str,strlen(str));

	char temp[512]="";
	snprintf(temp,512,"\"%s\",",strInfo);
	strcat(sl->strSql,temp);

	snprintf(temp,512,"\"%s\",",alias);
	strcat(sl->strSql,temp);

	snprintf(temp,512,"\"%s\",",category);
    strcat(sl->strSql,temp);

	snprintf(temp,512,"%u,",FileSize);
	strcat(sl->strSql,temp);

	snprintf(temp,512,"%u,",AcSize);
	strcat(sl->strSql,temp);

	snprintf(temp,512,"%d)", DownOk);
	strcat(sl->strSql,temp);

	m_LockQue.Lock();
	m_sqlQue.push(sl);
	m_LockQue.Unlock();
}
unsigned __stdcall CAVInfoManage::SqlOp_Thread(LPVOID lpParameter)
{

	CAVInfoManage * pIn= (CAVInfoManage *)lpParameter;
	pIn->SqlOpFun();
	return 1;
}
void  CAVInfoManage::UpDataAVinfo(_SQL_LITE *sl)
{
	/*::free(sl->pBuffer);
	::free(sl);
	return;*/
	sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt *pStmt = 0;  

    int w=100,h=100;

	char strsql[512]="";
	snprintf(strsql,512,sl->strSql,w,h);

#ifdef  WIN32
	AVPixelFormat srcFF=DstAVff;
	AVPixelFormat DestFF=AV_PIX_FMT_BGRA;
#else
	AVPixelFormat srcFF=DstAVff;
	AVPixelFormat DestFF=AV_PIX_FMT_BGRA;
#endif
	pimg_convert_ctx = sws_getCachedContext(pimg_convert_ctx,
		sl->width,sl->height ,srcFF,
		w,             h,     DestFF,                
		SWS_FAST_BILINEAR,
		NULL, NULL, NULL);
	if (pimg_convert_ctx == NULL) 
	{
		fprintf(stderr, "Cannot initialize the conversion context\n");
	}

	AVPicture Srcpict = { { 0 } };
	int SrcNumBytes=avpicture_get_size(srcFF, sl->width,sl->height);
	avpicture_fill((AVPicture *)&Srcpict, sl->pBuffer,srcFF,  sl->width,sl->height);


	AVPicture Destpict = { { 0 } };
	int DestNumBytes=avpicture_get_size(DestFF, w,h);

	if(m_Destbuffer==NULL)
	    m_Destbuffer=(uint8_t *)::malloc(DestNumBytes);
	avpicture_fill((AVPicture *)&Destpict, m_Destbuffer,DestFF,  w, h);


	sws_scale(pimg_convert_ctx, Srcpict.data, Srcpict.linesize,
		0,sl->height, Destpict.data,Destpict.linesize);
	
	
	int ret=sqlite3_prepare(pDb, strsql, -1, &pStmt, 0);
	ret= sqlite3_bind_blob(pStmt, 1,m_Destbuffer, DestNumBytes, NULL);
	ret=sqlite3_step(pStmt);;
    sqlite3_finalize(pStmt);
	
	
	::free(sl->pBuffer);
	::free(sl);
}

int CAVInfoManage::NoSelectSql(char *sqltxt)
{
	sqlite3* pDb=( sqlite3* )m_pDb;
	char** pResult;
	int nRow=-1;
	int nColumn=-1;
	int nIndex = nColumn;

	//sqlite3_free_table()
	int result= sqlite3_get_table(
		pDb,               /* An open database */
		sqltxt,     /* SQL to be evaluated */
		&pResult,    /* Results of the query */
		&nRow,           /* Number of result rows written here */
		&nColumn,        /* Number of result columns written here */
		NULL       /* Error msg written here */
		);
	sqlite3_free_table(pResult);
	//result != SQLITE_OK
	return result;
}
//获取历史信息
void CAVInfoManage::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
	SelectImgs(slQue);
}

void CAVInfoManage::GetDownAVInfo(std::vector<AV_Down_Info *> &slQue,int DownOk)
{
	//分析sql语句
	sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt* stmt2 = NULL;

	char** pResult;
	int nRow=-1;
	int nColumn=-1;
	std::string strOut="";
	int nIndex = nColumn;
	char sqlstr[256]="";
	snprintf(sqlstr,256,"select * from AVDownInfoMap where DownOK=%d",DownOk);

	m_LockDb.Lock();
	if (sqlite3_prepare_v2(pDb,sqlstr,strlen(sqlstr),&stmt2,NULL) != SQLITE_OK) {
		if (stmt2)
			sqlite3_finalize(stmt2);
		m_LockDb.Unlock();
		return;
	}

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
	//FileInfo TEXT NOT NULL,Alias TEXT,Category TEXT, FileSize INTEGER,AcSize INTEGER,DownOK
	for(int i=0;i<nRow;i++)
	{
		AV_Down_Info *Item =(AV_Down_Info *)::malloc(sizeof(AV_Down_Info));
		for(int j=0;j<nColumn;j++)
		{
		    

			char *val=pResult[nIndex];
			if(j==0){
				memset(Item->FileInfo,0,sizeof(Item->FileInfo));
				strcpy(Item->FileInfo,val);
			}else if(j==1){
                       memset(Item->Alias,0,32);
					   strcpy(Item->Alias,val);
			}else if(j==2){
                  memset(Item->Category,0,32);
				  strcpy(Item->Category,val);
			}else if(j==3){
				Item->FileSize=atoi(val);
			}else if(j==4){
				Item->AcSize=atoi(val);
			}
			nIndex++;
		}
		slQue.push_back(Item);
	}
	sqlite3_free_table(pResult);
	m_LockDb.Unlock();
	return;
}
void CAVInfoManage::SelectImgs(std::vector<_AV_Hos_Info *> &slQue)
{
	sqlite3* pDb=( sqlite3* )m_pDb;
	sqlite3_stmt *pStmt = 0;
	m_LockDb.Lock();
	int ret=sqlite3_prepare(pDb, "select * from AVinfoMap", -1, &pStmt, 0);

	int size=0;
	int SlLen=sizeof(AV_Hos_Info);
    //int ret=sqlite3_bind_blob(pStmt, 2, buf, 100000, NULL);
	while(SQLITE_ROW ==sqlite3_step(pStmt))
	{
		AV_Hos_Info *sl=(AV_Hos_Info *)::malloc(SlLen);
		memset(sl,0,SlLen);

		const unsigned char *pPath= sqlite3_column_text(pStmt, 0);
		strcpy(sl->path,(char*)pPath);

		const unsigned char *pMd5= sqlite3_column_text(pStmt, 1);
		strcpy(sl->md5,(char*)pMd5);
		
		//img
		const void *test = sqlite3_column_blob(pStmt, 2);
		size = sqlite3_column_bytes(pStmt, 2);
		sl->pBuffer=(unsigned char *)::malloc(size);
		memcpy(sl->pBuffer,test,size);
		sl->bufLen=size;

        sl->width=sqlite3_column_int(pStmt,3);
		sl->height=sqlite3_column_int(pStmt,4);

		sl->CurTime=sqlite3_column_int(pStmt,5);
		sl->TotalTime=sqlite3_column_int(pStmt,6);

		slQue.push_back(sl);

	}
	sqlite3_finalize(pStmt);
	m_LockDb.Unlock();

}
void  CAVInfoManage::SqlOpFun()
{
	m_SKK_ThreadInfo.ThOver=false;
	while(!m_ThStop)
	{

		bool sleepx=false;
		SQL_LITE__ *sl =NULL;
		m_LockQue.Lock();

		if(m_sqlQue.size()>0)
		{
			sl =m_sqlQue.front();
			m_sqlQue.pop();
			sleepx=true;
		}
		m_LockQue.Unlock();

		m_LockDb.Lock();
		if(sl!=NULL&&sl->SqlType==0)
		{
            UpDataAVinfo(sl);
		}else if(sl!=NULL&&sl->SqlType==1)
		{
		     NoSelectSql(sl->strSql);
			 ::free(sl);
		}
		m_LockDb.Unlock();
		if(!sleepx)
		{
			Sleep(100);
		}
	}
	m_SKK_ThreadInfo.ThOver=true;
}