#include "../../../librtmp/rtmp.h"
#ifndef __LIBRTMP_METADATA__
#define __LIBRTMP_METADATA__
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
// typedef struct  
// {
// 	//h264
// 	 int    nWidth;  
// 	 int    nHeight;  
// 	 int    nFrameRate;     // 帧率  
// 	 int    nVideoDataRate; // 码率  
// 	 int    nSpsLen;  
// 	 char   Sps[1024];  
// 	 int    nPpsLen;  
// 	 char   Pps[1024];  
// 
// 	//aac 
// 	bool   bHasAudio;  
// 	int    nAudioSampleRate;  
// 	int    nAudioSampleSize;  
// 	int    nAudioChannels;  
// 	char            pAudioSpecCfg;  
// 	unsigned int    nAudioSpecCfgLen; 
// 
// } RTMPMetadata;
// 
// typedef struct   
// {  
// 	//类型  音频  视频  
// 	int type;  
// 	//大小
// 	int size;  
// 	//buf
// 	unsigned char *data;  
// } PackData;


// NALU单元
typedef struct _NaluUnit
{
	int type;
	int size;
	unsigned char *data;
}NaluUnit;

typedef struct _RTMPMetadata
{
	// video, must be h264 type
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nFrameRate;		// fps
	unsigned int	nVideoDataRate;	// bps
	unsigned int	nSpsLen;
	unsigned char	Sps[1024];
	unsigned int	nPpsLen;
	unsigned char	Pps[1024];

	// audio, must be aac type
	bool	        bHasAudio;
	unsigned int	nAudioDatarate;
	unsigned int	nAudioSampleRate;
	unsigned int	nAudioSampleSize;
	int				nAudioFmt;
	unsigned int	nAudioChannels;
	char		    pAudioSpecCfg;
	unsigned int	nAudioSpecCfgLen;

} RTMPMetadata,*LPRTMPMetadata;


#endif