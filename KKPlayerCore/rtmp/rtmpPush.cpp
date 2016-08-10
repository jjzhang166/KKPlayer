#include "../librtmp/rtmp.h"
#include <string.h>
RTMP *m_pRtmp=NULL;
bool RTMPConnect(const char* url)
{
	m_pRtmp = RTMP_Alloc();  
	RTMP_Init(m_pRtmp);

	if(RTMP_SetupURL(m_pRtmp, (char*)url)<0)
	{
		return false;
	}
	RTMP_EnableWrite(m_pRtmp);
	if(RTMP_Connect(m_pRtmp, NULL)<0)
	{
		return false;
	}
	if(RTMP_ConnectStream(m_pRtmp,0)<0)
	{
		return false;
	}
	return true;
}

// 断开连接
void RTMPClose()
{
	if(m_pRtmp)
	{

		int x=RTMP_IsConnected(m_pRtmp);

		RTMP_Close(m_pRtmp);
		RTMP_Free(m_pRtmp);

		m_pRtmp = NULL;

	}
}

// 发送数据
int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)
{
	if(m_pRtmp == NULL)
		return FALSE;

	//视频
	if( nPacketType==RTMP_PACKET_TYPE_VIDEO)
	{
             
	}

	RTMPPacket packet;
	RTMPPacket_Reset(&packet);
	RTMPPacket_Alloc(&packet,size);

	packet.m_packetType = nPacketType;
	packet.m_nChannel = 0x04;  
	packet.m_headerType = /*RTMP_PACKET_SIZE_MEDIUM*/RTMP_PACKET_SIZE_LARGE;  
	packet.m_nTimeStamp = nTimestamp;  
	packet.m_nInfoField2 = m_pRtmp->m_stream_id;
	packet.m_nBodySize = size;
	packet.m_hasAbsTimestamp = 0;
	memcpy(packet.m_body,data,size);

	//将数据包放到发送队列
	int nRet = RTMP_SendPacket(m_pRtmp,&packet,TRUE);

	RTMPPacket_Free(&packet);

	return nRet;
}
