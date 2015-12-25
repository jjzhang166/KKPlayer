#include "../../../librtmp/rtmp.h"
#ifndef KKRtmp_H_
#define KKRtmp_H_
/**********用于接收RTMP流************/
class CKKRtmp
{
   public:
	       CKKRtmp(void);
	       ~CKKRtmp(void); 
		   int SetupURL(const char* url);
		   int RtmpConnect();
		   int RtmpConnectStream(uint32_t dSeek);
		   void ReadRtmpStream();
   private:
           RTMP *m_pRtmpInstance;
		   uint32_t m_dSeek;
		   uint32_t m_bufferSize;
};
#endif
