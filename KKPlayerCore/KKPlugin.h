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

	typedef void (*fFlushPlayerQue)(void *opaque);
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
		fFlushPlayerQue FlushQue;    //参数填入 kkirqOpaque
		fKKIRQ kkirq;                //外部填入,函数。在插件函数内应调用，返回1中断，参数填入kkirqOpaque
        void *kkirqOpaque;           //外部调用
		char *URL;
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