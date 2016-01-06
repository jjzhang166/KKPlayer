/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#pragma once
#include "IKKAudio.h"
#include "render/render.h"
#include "IKKPlayUI.h"
#include <queue>
#include <string>

#include "InterCriSec.h"
#include "KKVideoInfo.h"
#ifndef KKPlayer_H_
#define KKPlayer_H_
enum OpenMediaEnum
{
	No=0,
	Dump=1,//转存
};
enum SeekEnum
{
	Up,
	Down,
	Left,
	Right
};
class KKPlayer
{
    public:
	        KKPlayer(IKKPlayUI* pPlayUI,IKKAudio* pSound);
	        ~KKPlayer(void);
			void SetWindowHwnd(HWND hwnd);
			/*********打开媒体************/
			int OpenMedia(std::string fileName,OpenMediaEnum en=OpenMediaEnum::No,std::string FilePath="C:\\"); 
			void CloseMedia(); 
    		void ReadAV();
            int GetCurTime();
			/*****Gdi*****/
			void OnDrawImageByDc(HDC memdc);
			void RenderImage(CRender *pRender);
			void AdjustDisplay(int w,int h);
			/******显示函数*****/
			virtual void VideoDisplay(void *buf,int w,int h,void *usadata,double last_duration,double pts,double duration,long long pos,double diff);
            //视频刷线程
			static unsigned WINAPI VideoRefreshthread(LPVOID lpParameter);  
			//推流线程
			static unsigned WINAPI PushStream(LPVOID lpParameter);  
            
			//获取屏幕数据
			//static unsigned WINAPI PicGdiGrab(LPVOID lpParameter);
			void SetVolume(long value);
			long GetVolume();
			//暂停
			void Pause();
			//快进快退
			void KKSeek( SeekEnum en,int value);
			void InitSound();
private:
	        //文件打开后需要做什么
	        OpenMediaEnum m_OpenMediaEnum;
	        InterCriSec m_CloseLock;
	        bool m_bOpen;
	        IKKPlayUI* m_pPlayUI;
	        void VideoPushStream();
	        int WindowWidth;
			int WindowHeight;
	        SKK_Frame *m_DisplayVp;
	        IKKAudio* m_pSound;
			HWND m_hwnd;
	        void PacketQueuefree();
		    /*******显示视频**********/
		    void VideoRefresh();
		    void video_image_refresh(SKK_VideoState *is);
		    //视频信息
		    SKK_VideoState *pVideoInfo;
			int64_t start_time;
			
			//当前时间
			int m_CurTime;
			SKK_ThreadInfo m_ReadThreadInfo;
			SKK_ThreadInfo m_VideoRefreshthreadInfo;
			//等待事件
			HANDLE m_WaitEvent;

			//推流数据队列
			std::queue<AVPacket *> m_PushPktQue;
			InterCriSec m_PushStreamLock;
			std::string m_StrFilePath;
};
#endif
