//#define __cplusplus
#include <GLES2/gl2.h>
//#include <GLES/glext.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#include <android/native_window.h> 
#include <android/native_window_jni.h>
#include "AndKKAudio.h"

#include "../IKKPlayUI.h"
#include "../KKPlayer.h"

#ifndef AndKKPlayerUI_H_
#define AndKKPlayerUI_H_
#include <jni.h>
class CAndKKPlayerUI :public  IKKPlayUI
{
   public:
             CAndKKPlayerUI(int RenderView);
             ~CAndKKPlayerUI();
             int  Init();
			  
			 int  OnSize(int w,int h);
			 void SetKeepRatio(int KeepRatio);
			 void SetDecoderMethod(int method);
			 void SetSurfaceTexture(JNIEnv *env);
			 jobject GetSurfaceTexture();
			 void OnSurfaceTextureFrameAailable();
             void renderFrame(ANativeWindow* surface);
             int  CloseMedia();
             MEDIA_INFO GetMediaInfo();
             int OpenMedia(char *str);
             int GetIsReady();
             /******是否是流媒体,该方法不是很准确*****/
             int GetRealtime();
             //得到延迟
             int GetRealtimeDelay();
             //
             int SetMinRealtimeDelay(int value);
             //强制刷新Que
             void ForceFlushQue();  
			 bool GetNeedReconnect();
             void Pause();
             void Seek(int value);
             int GetPlayerState();
    /***********UI调用***********/
   public:
		   /********** IKKPlayUI实现*************/
			virtual unsigned char* GetErrImage(int &length,int ErrType);
			virtual unsigned char* GetWaitImage(int &len,int curtime);
			virtual unsigned char* GetBkImage(int &len);
			virtual unsigned char* GetCenterLogoImage(int &length);
			virtual void OpenMediaStateNotify(char* strURL,EKKPlayerErr err);
			///读取线程打开文件前的回调函数
			virtual int PreOpenUrlCallForSeg(char *InOutUrl,int *AvIsSeg,int *Interrupt);
			 // PreOpenUrlCallForSeg(char *InOutUrl,int *Interrupt)
			/*******视频流结束调用*******/
			virtual void  GetNextAVSeg(void *playerIns,int Stata,int quesize,KKPlayerNextAVInfo &NextInfo);
			/***视频读取线程结束调用***/
			virtual void  AVReadOverThNotify(void *playerIns);
			virtual void  AVRender(); 
			virtual IkkRender* GetRender();
   private:
        void   GlViewRender();
		void   SurfaceViewRender(ANativeWindow* surface);
        
       
        IkkRender* m_pRender;
        int m_RenderWidth;         ///呈现区域宽度
        int m_RenderHeight;        ///呈现区域高度
        int m_Picwidth;
        int m_Picheight;
        bool m_bAdJust;
        /*************浏览器状态***************/
        bool m_bNeedReconnect;
        int m_playerState;
		
        KKPlayer m_player;///播放器
        CAndKKAudio m_Audio;
        
        int m_Screen_Width;
        int m_Screen_Height;
      

        CKKLock m_RenderLock;
		/**呈现类型，0 glviw 1 **/
		int  m_nRenderType;
		int  m_nRefreshPic;
};
#endif