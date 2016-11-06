typedef unsigned char      uint8_t;
typedef long long          int64_t;
#ifndef KKPlugin_H_
#define KKPlugin_H_
extern "C"
{  
	
	//得到协议头
	typedef char (*fGetPtlHeader)(char *buf,char len);
	/***********用于释放内存**************/
	typedef void (*fFree)(void*);
    /****下载函数****/
    typedef char (*fKKDownAVFile)(char *strUrl);
	/********停止下载函数**********/
	typedef char (*fKKStopDownAVFile)(char *str);
	
	/***********下载速度信息***************/
	typedef struct KK_DOWN_SPEED_INFO{
		char IpPort[126];//用|分割端口
        unsigned int Speed;
		unsigned int AcSize;
		unsigned int FileSize;
		int Tick;
		KK_DOWN_SPEED_INFO *Next;
	}KK_DOWN_SPEED_INFO;
	/************得到下载速度信息*********/
    typedef KK_DOWN_SPEED_INFO* (*fKKDownAVFileSpeedInfo)(char *strurl,int *TotalSpeed);
    //刷新队列
	typedef void (*fFlushPlayerQue)(void *opaque);
	//计算延迟回调函数AVType 0,音频，1视频
	typedef void (*fCalPlayerDelay)(void *opaque,int64_t Pts,int AVType);

	typedef int  (*fIo_read_packet)(void *opaque, uint8_t *buf, int buf_size);
    //文件seek
	typedef int64_t  (*fIo_seek)(void *opaque, int64_t offset, int whence);
	//得到缓存时间
	typedef unsigned int     (*fGetCacheTime)(void *opaque);
	//
	typedef int  (*fKKIRQ)(void *opaque);  //强制中断函数

	typedef struct __KKPlugin
	{
        void *opaque;                        //用户数据
		fIo_read_packet kkread;              
		fIo_seek kkseek;
		fFlushPlayerQue FlushQue;           //参数填入 kkirqOpaque,刷新队列函数
		fCalPlayerDelay CalPlayerDelay;     //外部回调函数，Player填入
		fGetCacheTime GetCacheTime;
		fKKIRQ kkirq;                       //外部填入,函数。在插件函数内应调用，返回1中断，参数填入kkirqOpaque
        void *PlayerOpaque;                 //播放器环境
		char *URL;                          //去掉协议头的地址
		int RealTime;                       //1为实时，否则应为0
		int FirstRead;                      //第一次读，1,读后为0，需手动
	}KKPlugin;
	
	//创建一个插件实例
	typedef KKPlugin*  (*fCreateKKPlugin)();
	typedef void (*fDeleteKKPlugin)(KKPlugin*p);
	typedef struct __KKPluginInfo
	{
		char ptl[32];
		/******创建一个插件******/
		fCreateKKPlugin CreKKP;
		/**********删除一个插件**************/
		fDeleteKKPlugin DelKKp;
		/***********下载文件*****************/
		fKKDownAVFile   KKDownAVFile;
		/************停止下载*****************/
		fKKStopDownAVFile KKStopDownAVFile;
		/*************得到下载速度信息************/
		fKKDownAVFileSpeedInfo KKDownAVFileSpeedInfo;
		/*****************释放内存*************/
		fFree KKFree;
#ifdef WIN32
		HMODULE	Handle;
#else
		void*  Handle;/*****************/
#endif
	}KKPluginInfo;

	
};
#endif