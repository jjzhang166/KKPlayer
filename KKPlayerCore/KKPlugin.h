typedef unsigned char      uint8_t;
typedef long long          int64_t;
#ifndef KKPlugin_H_
#define KKPlugin_H_
extern "C"
{   typedef char (*fGetPtlHeader)(char *buf,char len);
    /****下载函数****/
    typedef char (*fKKDownAVFile)(char *strUrl);
	/********停止下载函数**********/
	typedef char (*fKKStopDownAVFile)(char *str);
	
	
	/************下载速度回调函数*********/
    //typedef char (*fKKDownAVFileSpeed)(char *str,int DownSpeed);
    //刷新队列
	typedef void (*fFlushPlayerQue)(void *opaque);
	//计算延迟回调函数AVType 0,音频，1视频
	typedef void (*fCalPlayerDelay)(void *opaque,int64_t Pts,int AVType);
	typedef int  (*fIo_read_packet)(void *opaque, uint8_t *buf, int buf_size);
    //文件seek
	typedef int64_t  (*fIo_seek)(void *opaque, int64_t offset, int whence);
	//
	typedef int  (*fKKIRQ)(void *opaque);  //强制中断函数

	typedef struct __KKPlugin
	{
        void *opaque;                 //用户数据
		fIo_read_packet kkread;
		fIo_seek kkseek;
		fFlushPlayerQue FlushQue;    //参数填入 kkirqOpaque,刷新队列
		fCalPlayerDelay CalPlayerDelay;
		fKKIRQ kkirq;                //外部填入,函数。在插件函数内应调用，返回1中断，参数填入kkirqOpaque
        void *PlayerOpaque;          //播放器环境
		char *URL;                   //去掉协议头的地址
		int RealTime;                //1 为实时，否则因为0
	}KKPlugin;
	
	//创建一个插件实例
	typedef KKPlugin*  (*fCreateKKPlugin)();
	typedef void (*fDeleteKKPlugin)(KKPlugin*p);
	typedef struct __KKPluginInfo
	{
		char ptl[32];
		fCreateKKPlugin CreKKP;
		fDeleteKKPlugin DelKKp;
		fKKDownAVFile   KKDownAVFile;
		fKKStopDownAVFile KKStopDownAVFile;
	//	fKKDownAVFileSpeed KKDownAVFileSpeed;
#ifdef WIN32
		HMODULE	Handle;
#else
		void*  Handle;/*****************/
#endif
	}KKPluginInfo;

	
};
#endif