#ifndef KkPlayerDui_H_
#define KkPlayerDui_H_
#include "../KKPlayerCore/IKKPlayUI.h"
#include "KKPlayer.h"
class CKKPlayerDui:public  IKKPlayUI
{
public:
        CKKPlayerDui();
		~CKKPlayerDui();
public:
         unsigned char* GetWaitImage(int &length,int curtime);
		 unsigned char* GetErrImage(int &length,int ErrType);
	     unsigned char* GetBkImage(int &length);
		 unsigned char* GetCenterLogoImage(int &length);
		 //打开失败
		 void OpenMediaStateNotify(char* strURL,EKKPlayerErr err);
		 ///读取线程打开文件前的回调函数,用于分段，返回1，代表有分段，此时InOutUrl已经被修改。
		 int PreOpenUrlCallForSeg(char *InOutUrl,int *AvIsSeg,int *Interrupt);
		 /*******视频流结束调用.用于加载下一个分片*******/
		 void GetNextAVSeg(void *playerIns,int Err,int quesize,KKPlayerNextAVInfo &NextInfo);

		 /***视频读取线程结束调用，只能使用其地址值****/
		 void  AVReadOverThNotify(void *playerIns);
		 /***视频读取线程结束调用***/
		 void  AVRender();
};
#endif