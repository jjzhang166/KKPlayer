
#ifndef __LIBRTMP_STREAM_INTERFACE__
#define __LIBRTMP_STREAM_INTERFACE__

#include "LibtrmpData.h"

class LibRtmpStream
{
public:
	// 连接到RTMP Server    
	virtual bool Connect(const char* url) = 0;    
	// 断开连接    
	virtual void Close() = 0;    
	// 发送MetaData    
	virtual bool SendMetadata(LPRTMPMetadata lpMetaData) = 0;    
	// 发送H264数据帧    
	virtual bool SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp) = 0; 
	//发送AAC数据
	virtual bool SendAACPacket(unsigned char* data,unsigned int size,unsigned int nTimeStamp ) =0;
	
};

LibRtmpStream *LibrtmpGetInstance();
#endif