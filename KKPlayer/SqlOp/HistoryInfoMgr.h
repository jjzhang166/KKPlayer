
#ifndef AVInfomanage_H_
#define AVInfomanage_H_
#include "../stdafx.h"
#include "SqliteOp.h"
#include <vector>
#include <helper/SCriticalSection.h>
typedef struct _AV_Hos_Info{
     char *url;
	 unsigned char* pBuffer;
	 int bufLen;
	 int width;
	 int height;
	 int CurTime;
	 int TotalTime;
}AV_Hos_Info;

class CHistoryInfoMgr{
public:
	     CHistoryInfoMgr(); 
		 ~CHistoryInfoMgr();
         void SetPath(const char *Path) ;
		 void InitDb();
		 /*******播放进度更新信息***********/
		 void UpDataAVinfo(const char *strpath,int curtime,int totaltime,unsigned char* Imgbuf,int buflen,int width,int height);
       
		 //获取放播的历史信息
		 void GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue);

		 void UpdataConfig(const char* StrKey,const char* StrValue);

		 bool GetConfig(const char* StrKey,std::string &OutValue);

		 int  GetH264Codec();
		 void UpdataH264Codec(int value);

		 int  GetH265Codec();
		 void UpdataH265Codec(int value);

		 int  GetUseLibRtmp();
		 void UpdataUseLibRtmp(int value);

		 int  GetRtmpDelay();
		 void UpdataRtmpDelay(int value);

  public:
	     static CHistoryInfoMgr *GetInance();
  private:
	     static CHistoryInfoMgr        *m_pInance;
	     CSqliteOp                     SqliteOp;
		 char                          m_strDbPath[1024];
		 void*                         m_pDb;
		 SOUI::SCriticalSection        m_Lock;
		 int        m_nH264Codec;
		 int        m_nH265Codec;
		 int        m_nUselibRtmp;
		 int        m_nlibRtmpDelay;
};
#endif