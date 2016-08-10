#include <stdlib.h>
#include <string.h>
#include "FlvEncode.h"
#include "stdint.h"

//<stdint.h>
namespace KKMEDIA
{
	#define NALU_TYPE_SLICE 1
	#define NALU_TYPE_DPA 2
	#define NALU_TYPE_DPB 3
	#define NALU_TYPE_DPC 4
	#define NALU_TYPE_IDR 5
	#define NALU_TYPE_SEI 6          
	#define NALU_TYPE_SPS 7
	#define NALU_TYPE_PPS 8
	#define NALU_TYPE_AUD 9
	#define NALU_TYPE_EOSEQ 10
	#define NALU_TYPE_EOSTREAM 11
	#define NALU_TYPE_FILL 12

	char *put_byte( char *output, uint8_t nVal )  
	{  
		output[0] = nVal;  
		return output+1;  
	}  
	char *put_be16(char *output, uint16_t nVal )  
	{  
		output[1] = nVal & 0xff;  
		output[0] = nVal >> 8;  
		return output+2;  
	}  
	char *put_be24(char *output,uint32_t nVal )  
	{  
		output[2] = nVal & 0xff;  
		output[1] = nVal >> 8;  
		output[0] = nVal >> 16;  
		return output+3;  
	}  
	char *put_be32(char *output, uint32_t nVal )  
	{  
		output[3] = nVal & 0xff;  
		output[2] = nVal >> 8;  
		output[1] = nVal >> 16;  
		output[0] = nVal >> 24;  
		return output+4;  
	}  
	char *put_be64( char *output, uint64_t nVal )  
	{  
		output=put_be32( output, nVal >> 32 );  
		output=put_be32( output, nVal );  
		return output;  
	}  
	char *put_amf_bool(char *output, int b)
	{
		*output=0x01;
		*(++output)= !!b;
		return ++output;
	}
	char *put_amf_string( char *c, const char *str )  
	{  
		uint16_t len = strlen( str );  
		c=put_be16( c, len );  
		memcpy(c,str,len);  
		return c+len;  
	} 
	
	char *put_amf_double( char *c, double d )  
	{  
		*c++ = 0;  /* type: Number */  
		{  
			unsigned char *ci, *co;  
			ci = (unsigned char *)&d;  
			co = (unsigned char *)c;  
			co[0] = ci[7];  
			co[1] = ci[6];  
			co[2] = ci[5];  
			co[3] = ci[4];  
			co[4] = ci[3];  
			co[5] = ci[2];  
			co[6] = ci[1];  
			co[7] = ci[0];  
		}  
		return c+8;  
	}

	FlvEncode::FlvEncode(void)
	{
	}

	FlvEncode::~FlvEncode(void)
	{
	}
	FLV_HEADER FlvEncode::CreateFLVHeader(char StreamInfo)
	{
		FLV_HEADER header;
		memset(&header,0,sizeof(FLV_HEADER));
		header.FLV[0]=0x46;
		header.FLV[1]=0x4c;
		header.FLV[2]=0x56;
		header.Ver=0x01;
		header.StreamInfo=StreamInfo;
		header.HeaderLen=0;
		char *pp=(char *)&header.HeaderLen;
		*(pp+3)=0x09;
		return header;
	}
	//http://blog.csdn.net/zengraoli/article/details/7742278
	void *FlvEncode::CreateFLVMetaData(METADATA *lpMetaData,int &OutLen)
	{
		char *body =(char *)::malloc(1024);

		char * p = (char *)body;
	
		p = put_byte( p, 2 );
		p = put_amf_string( p, "onMetaData" );

		//amf 2 type 1bytes：固定为0x08
		p = put_byte(p, 0x08);  
	    //array size 4bytes：表示接下来的metadata array data 中有多少组数据
		{
			p=put_be32(p,11);
			/*p = put_byte(p, 0x00);
			p = put_byte(p, 0x00);
			p = put_byte(p, 0x00);
			p = put_byte(p, 0x08);*/
			/**p++ =0x00 ;
			*p++ =0x00 ;
			*p++ =0x00 ;
			*p++ =0x08 ;*/
		}
		p =put_amf_string( p, "duration" );
		p =put_amf_double( p, lpMetaData->duration); //lpMetaData->nAudioFmt

		
		//①前面2bytes表示，第N个数组的名字所占的bytes
		//视频m_nCurPos
		p =put_amf_string( p, "width");
		p =put_amf_double( p, lpMetaData->nWidth);

		p =put_amf_string( p, "height");
		p =put_amf_double( p, lpMetaData->nHeight);

		p =put_amf_string( p, "framerate" );    //视频帧率
		p =put_amf_double( p, lpMetaData->nFrameRate); 

		//p =put_amf_string( p, "videodatarate" );//视频数据速率
		//p =put_amf_double( p, lpMetaData->nVideoDataRate); 

		p =put_amf_string( p, "videocodecid" );
		p =put_amf_double( p,  7);//lpMetaData->nVideoFmt
		
		//音频
		p =put_amf_string( p, "audiodatarate");//音频数据速率
		p =put_amf_double( p, lpMetaData->nAudioDatarate);

		p =put_amf_string( p, "audiosamplerate");
		p =put_amf_double( p, lpMetaData->nAudioSampleRate); //音频采样率

		p =put_amf_string( p, "audiosamplesize" );
		p =put_amf_double( p, lpMetaData->nAudioSampleSize); //音频采样精度

		p =put_amf_string( p, "stereo" );//是否为立体声
		//p =put_be32( p,1 ); //lpMetaData->nAudioChannels
		p=put_amf_bool(p, 1);

		p =put_amf_string( p, "audiocodecid" ); //音频编码方式
		p =put_amf_double( p, 2); //lpMetaData->nAudioFmt

		p =put_amf_string( p, "filesize" );
		p =put_amf_double( p, lpMetaData->filesize); //lpMetaData->nAudioFmt

		//p =put_amf_string( p, "" );
		//p =put_byte( p, 9);
		OutLen=0;
		for(int i=0;i<1024;i++)
		{
			OutLen++;
			if(p==body+i)
			{
				break;
			}
		}
		return body;
	}

	void *FlvEncode::FlvMemcpy(void* dest,size_t destLen, const void* src, size_t n)
	{
		char*     d= (char*) dest;
		const char*  s= (const char*)src;
		s=s+n-1;
		while(n--&&destLen--)
		{
			*d++=*s--;
		}
		return dest;
	}

	int FlvEncode::GetNALULen(char *src,int srcLen)
	{
		int len=0;
		int Index=0;
		char* pdata=src;
		while(len<srcLen)
		{
			
			if(
				(
				*pdata==0x00&&
				*(1+pdata)==0x00&&
				*(2+pdata)==0x01
				)
				||
				(
					*pdata==0x00&&
					*(1+pdata)==0x00&&
					*(2+pdata)==0x00&&
					*(3+pdata)==0x01
				)
			)
			{
               break;
			}
			len++;
			pdata++;
		}
		return len;
	}

	int FlvEncode::GetH264SeparatorLen(char *src,int srcLen)
	{
		if(*src!=0x00)
			return 0;
		char* pdata=src;
		
		if(
			*pdata==0x00&&
			*(1+pdata)==0x00&&
			*(2+pdata)==0x01
			)
		{
			return 3;
		}
		if(
			*pdata==0x00&&
			*(1+pdata)==0x00&&
			*(2+pdata)==0x00&&
			*(3+pdata)==0x01
			)
		{
			return 4;
		}
		return 0;
	}
	
}