
#include "AVInfomanage.h"
#include "SqliteOp.h"
#include "../stdafx.h"
#include "../Includeffmpeg.h"
CAVInfoManage::CAVInfoManage()
{
	m_Destbuffer=NULL;
	pimg_convert_ctx=NULL;
	m_ThStop=false;
    m_pDbOp = new CSqliteOp();
	memset(m_strDb,0,256);
#ifdef WIN32_KK
	m_SKK_ThreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, SqlOp_Thread, (LPVOID)this, 0,&m_SKK_ThreadInfo.Addr);
#else
	m_SKK_ThreadInfo.Addr = pthread_create(&m_SKK_ThreadInfo.Tid_task, NULL, (void* (*)(void*))SqlOp_Thread, (LPVOID)this);
#endif
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
		
	}
    CSqliteOp *DbOp =(CSqliteOp *)m_pDbOp;
    delete DbOp;

}

CAVInfoManage *CAVInfoManage::GetInance()
{
	if(m_pInance==NULL)
	{
		m_pInance= new CAVInfoManage();


	}
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
}

void CAVInfoManage::UpDataAVinfo(char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height)
{
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

	pimg_convert_ctx = sws_getCachedContext(pimg_convert_ctx,
		sl->width,sl->height , PixelFormat::AV_PIX_FMT_BGRA,
		w,             h,     PixelFormat::AV_PIX_FMT_BGRA,                
		SWS_FAST_BILINEAR,
		NULL, NULL, NULL);
	if (pimg_convert_ctx == NULL) 
	{
		fprintf(stderr, "Cannot initialize the conversion context\n");
	}

	AVPicture Srcpict = { { 0 } };
	int SrcNumBytes=avpicture_get_size(PixelFormat::AV_PIX_FMT_BGRA, sl->width,sl->height);
	avpicture_fill((AVPicture *)&Srcpict, sl->pBuffer,PixelFormat::AV_PIX_FMT_BGRA,  sl->width,sl->height);


	AVPicture Destpict = { { 0 } };
	int DestNumBytes=avpicture_get_size(PixelFormat::AV_PIX_FMT_BGRA, w,h);

	if(m_Destbuffer==NULL)
	    m_Destbuffer=(uint8_t *)::malloc(DestNumBytes);
	avpicture_fill((AVPicture *)&Destpict, m_Destbuffer,PixelFormat::AV_PIX_FMT_BGRA,  w, h);


	sws_scale(pimg_convert_ctx, Srcpict.data, Srcpict.linesize,
		0,sl->height, Destpict.data,Destpict.linesize);
	
	
	int ret=sqlite3_prepare(pDb, strsql, -1, &pStmt, 0);
	ret= sqlite3_bind_blob(pStmt, 1,m_Destbuffer, DestNumBytes, NULL);
	ret=sqlite3_step(pStmt);;
    sqlite3_finalize(pStmt);
	
	
	::free(sl->pBuffer);
	::free(sl);
    
	
	
	
}
//获取历史信息
void CAVInfoManage::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
	SelectImgs(slQue);
 /*  SQL_LITE__ *sl =(SQL_LITE__ *)::malloc(sizeof(SQL_LITE__));
   memset(sl,0,sizeof(SQL_LITE__));
   sl->SqlType=1;
   m_LockQue.Lock();
   m_sqlQue.push(sl);
   m_LockQue.Unlock();*/
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
		   // SelectImgs();
		}
		m_LockDb.Unlock();
		if(!sleepx)
		{
			Sleep(100);
		}
	}
	m_SKK_ThreadInfo.ThOver=true;
}