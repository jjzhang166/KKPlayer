///一些对外的数据定义
#ifndef KKPlayerInfo_H_
#define KKPlayerInfo_H_
typedef struct AVCACHE_INFO
{
	int VideoSize;
	int AudioSize;
	int MaxTime;
}AVCACHE_INFO;
//获取播放器信息
typedef struct MEDIA_INFO 
{
	//分辨率
	char              AVRes[32];
	char              AVinfo[1024];
	const char*       AvFile;
	int               FileSize;
	int               CurTime;
	int               TotalTime;//总时长
	int               serial;
	bool              Open;
	int               KKState;
    short             SegId;
	char              SpeedInfo[1024];     ///下载速度
	AVCACHE_INFO      CacheInfo;
}MEDIA_INFO ;
#endif