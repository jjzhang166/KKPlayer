#include "rtmp.h"
#include "rtmp_sys.h"
#include "amf.h"
#include <stdio.h>
#include "LibrtmpStreamInterface.h"
#include "LibtrmpData.h"

#ifndef __RELIBRTMP_STREAMDATA__
#define __RELIBRTMP_STREAMDATA__


#define FILEBUFSIZE (1024 * 1024 * 10) //  10M
enum
{
	FLV_CODECID_H264 = 7,
};

class RTMPStream :public LibRtmpStream
{
public:
	RTMPStream(void);
	~RTMPStream(void);
public:
	// 连接到RTMP Server
	bool Connect(const char* url);
	// 断开连接
	void Close();
	// 发送MetaData
	bool SendMetadata(LPRTMPMetadata lpMetaData);
	// 发送H264数据帧
	bool SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp);
	//发送AAC数据
	bool SendAACPacket(unsigned char* data,unsigned int size,unsigned int nTimeStamp );


private:
	 int InitSockets();
	 void CleanupSockets();
	 char * put_byte( char *output, uint8_t nVal ) ;
	 char * put_be16(char *output, uint16_t nVal ) ;
	 char * put_be24(char *output,uint32_t nVal )  ;
	 char * put_be32(char *output, uint32_t nVal ) ;
	 char * put_be64( char *output, uint64_t nVal );
	 char * put_amf_string( char *c, const char *str );
	 char * put_amf_double( char *c, double d );
	 // 发送数据
	 int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp);
	 RTMP* m_pRtmp;

};

#endif