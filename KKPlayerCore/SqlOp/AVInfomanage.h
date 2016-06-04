#include "../stdafx.h"
#include "../KKVideoInfo.h"
#include <vector>
#ifdef WIN32
   #define AV_COLOR "BGRA"
#else
#define AV_COLOR "RGBA"
#endif
#ifndef AVInfomanage_H_
#define AVInfomanage_H_
typedef struct _AV_Hos_Info
{
     char path[512];
	 char md5[256];
	 unsigned char* pBuffer;
	 int bufLen;
	 int width;
	 int height;
	 int CurTime;
	 int TotalTime;
}AV_Hos_Info;

typedef struct _SQL_LITE
{
	char strSql[512];
	unsigned char* pBuffer;
	int bufLen;
	int SqlType;
	int width;
	int height;	
	char* color;
}SQL_LITE__;

class CAVInfoManage
{
  public:
		 ~CAVInfoManage();
         void SetPath(char *Path) ;
		 static CAVInfoManage *GetInance();
		 void InitDb();
		 void UpDataAVinfo(char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height);

		 //获取放播的历史信息
		 void GetAVHistoryInfo(std::vector<_AV_Hos_Info *> &slQue);
  private:
	    
	  uint8_t * m_Destbuffer;
	     SKK_ThreadInfo m_SKK_ThreadInfo;
	     CAVInfoManage();
		 static unsigned __stdcall SqlOp_Thread(LPVOID lpParameter);
		 void SqlOpFun();

		 void UpDataAVinfo(_SQL_LITE *sl);
		 void SelectImgs(std::vector<_AV_Hos_Info *> &slQue);
	     void *m_pDb;
	     void *m_pDbOp;
		 char m_strDb[256];
		 static CAVInfoManage *m_pInance;
		 volatile bool m_ThStop;
		 SwsContext * pimg_convert_ctx;
		 std::queue<_SQL_LITE *> m_sqlQue;

		 CKKLock  m_LockQue;

		 CKKLock  m_LockDb;
};
#endif