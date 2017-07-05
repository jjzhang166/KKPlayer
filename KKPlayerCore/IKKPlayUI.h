/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#ifndef KKPlayUI_H_
#define KKPlayUI_H_
#include "render/render.h"
enum EKKPlayerErr
{
     KKOpenUrlOk=0,          /***播发器打开成功**/
	 KKOpenUrlOkFailure=1,   /**播发器打开失败***/
	 KKAVNotStream=2,
	 KKAVReady=3,            ///缓冲已经准备就绪
	 KKAVWait=4,             ///需要缓冲
	 KKRealTimeOver=5,
	 KKEOF=6,                 ///文件结束了。
     KKAVOver=7,              ///视频播放结束。
	 KKSeekOk=8,              ///Seek成功
	 KKSeekErr=9,             ///seek失败
};
typedef struct KKPlayerNextAVInfo
{
     char url[1024];///填入要打开的url地址。
	 bool NeedRead; ///是否需要打开。
	 bool lstOpSt;  ///上次打开是否成功
	 short SegId;   ///需要打开的分段Id
}KKPlayerNextAVInfo;
/*******UI接口********/
class IKKPlayUI
{
   public:
	         virtual unsigned char* GetWaitImage(int &length,int curtime)=0;
			 virtual unsigned char* GetErrImage(int &length,int ErrType)=0;
		     virtual unsigned char* GetBkImage(int &length)=0;
			 virtual unsigned char* GetCenterLogoImage(int &length)=0;
			 //打开失败
			 virtual void OpenMediaStateNotify(char* strURL,EKKPlayerErr err)=0;
			 ///读取线程打开文件前的回调函数,用于分段，返回1，代表有分段，此时InOutUrl已经被修改。
			 virtual int PreOpenUrlCallForSeg(char *InOutUrl,int *AvIsSeg,int *Interrupt)=0;
			 /*******视频流结束调用.用于加载下一个分片*******/
			 virtual void GetNextAVSeg(void *playerIns,int Err,int quesize,KKPlayerNextAVInfo &NextInfo)=0;

			 /***视频读取线程结束调用，只能使用其地址值****/
			 virtual void  AVReadOverThNotify(void *playerIns)=0;
			 /***视频读取线程结束调用***/
			 virtual void  AVRender()=0;

			/* virtual void RenderLock()=0;
			 virtual void RenderUnLock()=0;*/
			 virtual IkkRender* GetRender()=0;
   protected:
	         unsigned char* m_pBkImage;
			 int m_pBkImageLen;
			 int m_pCenterLogoImageLen;

};
#endif
