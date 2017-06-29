#ifndef  ICallKKplayer_H_
#define  ICallKKplayer_H_
#include <string>
//errcode 参考EKKPlayerErr
typedef void (*fpKKPlayerErrNotify)(void *UserData,int errcode);
class  ICallKKplayer
{
public:
	    virtual ~ICallKKplayer(){}
	    virtual int          Pause()=0;
		virtual int          PktSerial()=0;
		virtual void         OnDecelerate()=0;
		virtual void         OnAccelerate()=0;
		/******设置实时流媒体最小延迟,最小值2，单位秒**********/
		virtual int          SetMaxRealtimeDelay(int Delay)=0;
		virtual void         SetErrNotify(void *UserData,fpKKPlayerErrNotify m_ErrNotify)=0;
		///抓取视频图片
		virtual bool         GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale=true)=0;

		//获取播放的时间
		virtual int          GetPlayTime()=0;
		virtual int          GetTotalTime()=0;
		virtual int          GetRealtime()=0;
		virtual bool         GetMediaInfo(MEDIA_INFO& info)=0;
		virtual void         SetVolume(long value,bool tip=true)=0;
		virtual long         GetVolume()=0;
		virtual void         AvSeek(int value)=0;
		virtual int          OpenMedia(std::string url)=0;
		virtual int          DownMedia(char *KKVURL,bool Down=true)=0;
		virtual void         CloseMedia()=0;
		//全屏
		virtual void         FullScreen()=0;
};
#endif