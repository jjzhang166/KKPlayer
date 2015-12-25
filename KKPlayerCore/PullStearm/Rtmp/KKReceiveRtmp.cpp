#include "KKReceiveRtmp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
int  InitSockets()  
{  
#ifdef WIN32  
	WORD version;  
	WSADATA wsaData;  

	version = MAKEWORD(1, 1);  
	return (WSAStartup(version, &wsaData) == 0);  
#else  
	return TRUE;  
#endif  
}  
FILE *netstackdump=NULL;
FILE *netstackdump_read=NULL;
CKKRtmp::CKKRtmp(void)
{
	InitSockets();
	m_pRtmpInstance=NULL;
	m_pRtmpInstance=RTMP_Alloc();
	RTMP_Init(m_pRtmpInstance);
	m_dSeek=0;
	m_bufferSize=1024*1024*8;
}
CKKRtmp::~CKKRtmp(void)
{
}
int CKKRtmp::SetupURL(const char* url)
{
   int ll=RTMP_SetupURL(m_pRtmpInstance,(char*)url);//"rtmp://192.9.8.229:1935/live/b450d2f2-7af2-bf80-b71a-471f90465730");
   m_pRtmpInstance->Link.lFlags|=RTMP_LF_LIVE;
   return ll;
}
int CKKRtmp::RtmpConnect()
{
     int ll=RTMP_Connect(m_pRtmpInstance,NULL);
	 return ll;
}
int CKKRtmp::RtmpConnectStream(uint32_t dSeek)
{
	 RTMP_SetBufferMS(m_pRtmpInstance,1000);
	int ll=RTMP_ConnectStream(m_pRtmpInstance,dSeek);
	return ll;
}
void CKKRtmp::ReadRtmpStream()
{
	char *pBuffer =(char*)::malloc(m_bufferSize);
	int nRead=0;
	//rtmp->m_read.initialFrameType = initialFrameType;  
	do 
	{
		memset(pBuffer,0, m_bufferSize);
		 nRead = RTMP_Read(m_pRtmpInstance, pBuffer, m_bufferSize);  
		//RTMP_LogPrintf("nRead: %d\n", nRead);  
		if (nRead > 0)  
		{  
             int ll=0;
			 ll++;
		}
	} while (1);
}
