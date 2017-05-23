typedef unsigned char      uint8_t;
typedef long long          int64_t;
#ifndef KKPlugin_H_
#define KKPlugin_H_

#define KK_MKTAG(a,b,c,d)                 ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define KK_PERRTAG(a, b, c, d)            (-(int)KK_MKTAG(a, b, c, d))
#define KK_AVERROR_EOF                       KK_PERRTAG( 'E','O','F',' ') ///< End of file
#define KK_AVERROR(e)                       (-(e))
#define KK_ERROR_HTTP_NOT_FOUND           FFERRTAG(0xF8,'4','0','4')
extern "C"
{  
	
	//得到协议头
	typedef char (*fGetPtlHeader)(char *buf,char len);
	/***********用于释放内存**************/
	typedef void (*fFree)(void*);
    /****下载函数****/
    typedef char (*fKKDownAVFile)(char *strUrl);
	/********暂停下载或者重新下载下载函数**********/
	typedef void (*fKKPauseDownAVFile)(char *strUrl,bool Pause);
	
	
	/************得到下载速度信息*********/
    typedef bool (*fKKDownAVFileSpeedInfo)(const char *strurl,char *jsonBuf,int len);
    //刷新队列
	typedef void (*fFlushPlayerQue)(void *opaque);
	//计算延迟回调函数AVType 0,音频，1视频
	typedef void (*fCalPlayerDelay)(void *opaque,int64_t Pts,int AVType);

	typedef int  (*fIo_read_packet)(void *opaque, uint8_t *buf, int buf_size);
    //文件seek
	typedef int64_t  (*fIo_seek)(void *opaque, int64_t offset, int whence);
	
	//得到缓存时间
	typedef unsigned int     (*fGetCacheTime)(void *opaque);

	//url分析函数
	typedef char* (*fKKUrlParser)(const char *strurl,int *abort_request);
	//
	typedef int  (*fKKIRQ)(void *opaque);  //强制中断函数
    typedef void (*fSetNeedWait)(void* PlayerOpaque,bool wait);
	typedef struct __KKPlugin
	{
        void *opaque;                               //用户数据，      插件设置
		fIo_read_packet kkread;                     //读函数，        插件设置
		fIo_seek        kkseek;                     //seek函数        插件设置
		fFlushPlayerQue FlushQue;                   //参数填入 kkirqOpaque,刷新队列函数
		fCalPlayerDelay CalPlayerDelay;             //外部回调函数，Player填入
		fGetCacheTime   GetCacheTime;
		fKKIRQ          kkirq;                      //外部填入,函数。在插件函数内应调用，返回1中断，参数填入kkirqOpaque
		fSetNeedWait    SetNeedWait;                //播放器填入
        void  *PlayerOpaque;                        //播放器环境
		char  *URL;                                 //去掉协议头的地址
		int   RealTime;                             //1为实时，否则应为0
		int   FirstRead;                            //第一次读，1,读后为0，需手动
	}KKPlugin;
	
	//创建一个插件实例
	typedef KKPlugin*  (*fCreateKKPlugin)();
	typedef void (*fDeleteKKPlugin)(KKPlugin*p);
	typedef struct __KKPluginInfo
	{
		/***插件名称***/
		char ptl[32];
		/******创建一个插件******/
		fCreateKKPlugin        CreKKP;
		/**********删除一个插件**************/
		fDeleteKKPlugin        DelKKp;
		/***********下载文件*****************/
		fKKDownAVFile          KKDownAVFile;
		/************暂停或者重新下载*****************/
		fKKPauseDownAVFile     KKPauseDownAVFile;
		/*************得到下载速度信息************/
		fKKDownAVFileSpeedInfo KKDownAVFileSpeedInfo;
		/*****************释放内存*************/
		fFree KKFree;
		/***********分析url***************/
		fKKUrlParser           KKUrlParser;
#ifdef WIN32
		HMODULE	Handle;
#else
		void*  Handle;/*****************/
#endif
	}KKPluginInfo;

	
};
#endif