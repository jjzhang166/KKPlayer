//封装flv文件
#ifndef FlvEncode_H_
#define FlvEncode_H_
namespace KKMEDIA
{
#pragma pack(push,1)
typedef struct _FLV_HEADER
{
	char FLV[3];//={0x46,0x4c,0x56};
	char Ver;   //版本号
	
	char StreamInfo;// 有视频又有音频就是0x01 | 0x04（0x05）
	int HeaderLen; 
} FLV_HEADER;

typedef struct _FLV_TAG_HEADER
{
	int  PreTagLen;  /*******上一个Tag长度********/         //   4
    char TagType;   //音频（0x8），视频（0x9），脚本（0x12）  //  1
	char TagDtatLen[3];    /*******数据区长度*******/         // 3
    char Timestamp[3];//时间戳                               //  3
	char ExpandTimeStamp;//是扩展时间戳                      //  1
	char streamID[3];//streamID，但是总为0                   //  3
}FLV_TAG_HEADER;

typedef struct _METADATA
{

	double filesize;/**********市斤毫秒***********/
	double duration;/**********市斤毫秒***********/
	// video, must be h264 type
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nFrameRate;		// fps
	unsigned int	nVideoDataRate;	// bps
	unsigned int	nSpsLen;
	unsigned char	Sps[1024];
	unsigned int	nPpsLen;
	unsigned char	Pps[1024];
	unsigned int    nVideoCodecId;
	int nVideoFmt;

	// audio, must be aac type
	bool	        bHasAudio;
	unsigned int	nAudioDatarate;
	unsigned int	nAudioSampleRate;
	unsigned int	nAudioSampleSize;
	int				nAudioFmt;
	unsigned int	nAudioChannels;
	char		    pAudioSpecCfg;
	unsigned int	nAudioSpecCfgLen;
	unsigned int    nAudioCodecId;

}METADATA;
//AVCDecorderConfigurationRecord

//AVCVideoPacket Format： AVCPacketType(8)+ CompostionTime(24) +Data
//AVCPacketType为包的类型：
//	如果AVCPacketType=0x00，为AVCSequence Header；
//	如果AVCPacketType=0x01，为AVC NALU；
//	如果AVCPacketType=0x02，为AVC end ofsequence
//	CompositionTime为相对时间戳：
//	如果AVCPacketType=0x01， 为相对时间戳；
//	其它，均为0；
//	Data为负载数据：
//	如果AVCPacketType=0x00，为AVCDecorderConfigurationRecord；
//	如果AVCPacketType=0x01，为NALUs；
//	如果AVCPacketType=0x02，为空。
typedef struct _AVC_DEC_CON_REC
{
	char cfgVersion;//configurationVersion 版本号
	char avcProfile;//AVCProfileIndication sps[1]
	char profileCompatibility;//profile_Compatibility sps[2]
	char avcLevel;//AVCLevelIndication     sps[3]
	//lengthSizeMinusOne:indicates the length in bytes of the NALUnitLength field in an AVC video
	char reserved6_lengthSizeMinusOne2;
	char reserved3_numOfSPS5;//个数
	long spsLength;//sequenceParameterSetLength
	void *sps;
	char numOfPPS;//个数
	long ppsLength;
	void *pps;
}AVC_DEC_CON_REC;

#pragma pack(pop)
class FlvEncode
{
public:
	     FlvEncode(void);
	     ~FlvEncode(void);
		 //创建一个FLVHeader
		 FLV_HEADER CreateFLVHeader(char StreamInfo);
		
		 void *CreateFLVMetaData(METADATA *lpMetaData,int &OutLen);
		 void *FlvMemcpy(void* dest,size_t destLen, const void* src, size_t n);
		 int GetNALULen(char *src,int srcLen);
		 //获取h264分割符长度
		 int GetH264SeparatorLen(char *src,int srcLen);
};
}
#endif