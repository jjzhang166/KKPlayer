/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include <queue>
#include <list>
#include "stdafx.h"

#include "KKPlayerInfo.h"
#include "IKKAudio.h"
#include "render/render.h"
#include "IKKPlayUI.h"


#include "KKLock.h"
#include "KKVideoInfo.h"
#include "KKPlugin.h"

#ifndef KKPlayer_H_
#define KKPlayer_H_

class KKPlayer
{
    public:
	        KKPlayer(IKKPlayUI* pPlayUI,IKKAudio* pSound);
	        ~KKPlayer(void);
			/******Windows平台调用**********/
			void          SetWindowHwnd(HWND hwnd);

			
			///强制中断。
			void          ForceAbort();

			///设置最后打开音频
			void          SetLastOpenAudio(bool bLastOpenAudio);
			///设置是否呈现
			void          SetRender(bool bRender);
			/*********打开媒体.成功返回0，失败返回-1.************/
			int           OpenMedia(char* URL,char* Other=""); 
			
			/*********关闭播放器*********/
			void          CloseMedia(); 
			void          RenderImage(IkkRender *pRender,bool Force);
			
			void          OnDecelerate();
			void          OnAccelerate();
			int           GetAVRate();
#ifdef WIN32_KK
			/*****Gdi*****/
			void          OnDrawImageByDc(HDC memdc);
			void          VideoDisplay(void *buf,int w,int h,void *usadata,double last_duration,double pts,double duration,long long pos,double diff);
#endif           
			void          SetVolume(long value);
			long          GetVolume();
			//暂停
			void          Pause();
			//快进快退，相对
			int           KKSeek( SeekEnum en,int value);
			//单位时间秒
			int           AVSeek(int value,short segid=-1);
			
			
			//获取播放信息
			bool          GetMediaInfo(MEDIA_INFO &info);
			
			//得到包序列号
			int           GetPktSerial();	
			short         GetSegId();
			short         GetCurSegId();
		
		

			//1为流媒体,没有open返回-1/******是否是流媒体,该方法不是很准确*****/
			int GetRealtime();

			//解码成BGRA格式
			void SetBGRA();
			///抓取视频图片
			bool GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale=true);
			/******是否准备好了,准备好返回1，否则返回0，没有open返回-1*******/
			int GetIsReady();
			
			//得到延迟
			int GetRealtimeDelay();
			//强制刷新播放器Que
			void ForceFlushQue();
		
			/******设置实时流媒体最小延迟,最小值2，单位秒**********/
			int SetMaxRealtimeDelay(int Delay);
            
			//显示视频追踪信息,返回1成功
			int ShowTraceAV(bool Show);

			//获取播放的时间
			int GetPlayTime();
			int GetTotalTime();

			
			/*******************插件分析,返回1有对应的插件*********************/
			int KKProtocolAnalyze(char *StrfileName,KKPluginInfo &KKPl);
			/**********添加插件**********/
			static void AddKKPluginInfo(KKPluginInfo &info);
			static std::list<KKPluginInfo>&  GetKKPluginInfoList();
#ifdef Android_Plat
			void *  GetVideoRefreshJNIEnv()
			{
			    return m_pVideoRefreshJNIEnv;
			}
			void SetSurfaceTexture(void* surface) ;
#endif

private:
	       
	        /*********视频刷新线程********/
	        static unsigned __stdcall VideoRefreshthread(LPVOID lpParameter);
			//音频回调线程
			static unsigned __stdcall Audio_Thread(LPVOID lpParameter);
			//文件读取线程
	        static unsigned __stdcall ReadAV_thread(LPVOID lpParameter);
			
			void        OpenAudioDev();
			bool        OpenInputSegAV(const char *url,short segid,bool flush);
			bool        loadSeg(AVFormatContext**  pAVForCtx,int AVQueSize,short segid=-1,bool flush=false);
			void        InterSeek(AVFormatContext*  pAVForCtx);
			//数据读取
	        void        ReadAV();
			
			///延迟分析
			void        AvDelayParser();
	        //视频刷线程
			void        VideoRefresh();
	        
			/*******显示视频**********/
		    void        video_image_refresh(SKK_VideoState *is);
			/******绘制音频图谱******/
            void        video_audio_display(IkkRender *pRender,SKK_VideoState *s);

			/********流媒体这是刷新函数**********/
			void        Avflush(int64_t seek_target); 
			void        AvflushRealTime(int Avtype);
			//读音频回调
			void        ReadAudioCall();
			void        PacketQueuefree();

			
			KKPlayer(const KKPlayer& cs);
			KKPlayer operator = (const KKPlayer& cs);
private:
	        //插件信息
	        static std::list<KKPluginInfo>  KKPluginInfoList;

			
			//缓存信息
			AVCACHE_INFO                    m_AVCacheInfo;

	        //视频信息
	        SKK_VideoState*                 pVideoInfo; 

			
			///AV视频信息
			KKPlayerNextAVInfo              m_AVNextInfo;
			
            //播放信息
			MEDIA_INFO                      m_AVPlayInfo;
			//播放器锁
	        CKKLock                         m_PlayerLock;
            
			CKKLock                         m_AVInfoLock;
			///指示是否有分片
			int                             m_AvIsSeg;
			///缓存计数
			int                             m_CacheAvCounter;
			///是否显示
			bool                            m_bRender;
			bool                            m_bLastOpenAudio;
            volatile int                    m_nPreFile;

	        volatile  bool                  m_bOpen;
	        IKKPlayUI*                      m_pPlayUI;
	       
			//当前包序列号
			volatile int                    m_PktSerial;
	        IKKAudio*                       m_pSound;
			HWND                            m_hwnd;
			int64_t                         start_time;
			
			///seektime
			int                             m_nSeekTime;
			short                           m_nSeekSegId;
			//当前时间
			int                             m_CurTime;
			int                             m_TotalTime;
			/// 
			//0x0001   视频;
			//0x0010    音频， 
			//0x0100    字幕， 
			int                             m_nhasVideoAudio;
			//视频读取线程
			SKK_ThreadInfo                  m_ReadThreadInfo;
			//视频刷新线程
			SKK_ThreadInfo                  m_VideoRefreshthreadInfo;
            //音频数据回调线程
			SKK_ThreadInfo                  m_AudioCallthreadInfo;
			int64_t                         m_lstPts;
			
            //用于渲染音频波形的缓存
			void*                           m_pAudioPicBuf;
			int                             m_AudioPicBufLen;
			//解码后图形的格式
			AVPixelFormat                   m_DstAVff;
			
#ifdef Android_Plat
			void *  m_pVideoRefreshJNIEnv;
			void*   m_pSurfaceTexture;
#endif
			
};

#endif
