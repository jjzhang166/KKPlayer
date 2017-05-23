#include "../librtmp/rtmp.h"
#include <string.h>
#include "../KKPlayer.h"
static int librtmp_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
  KKPlugin* plu=(KKPlugin*)opaque;

  if(plu->opaque==NULL){
      RTMP *rtmp=RTMP_Alloc();  
      RTMP_Init(rtmp);
	  char url[1024]="rtmp://live.hkstv.hk.lxdns.com/live/hks";
	  if(!RTMP_SetupURL(rtmp,url))  
      { 
		   return KK_AVERROR(EAGAIN);
	  }
	 
	  RTMP_SetBufferMS(rtmp, 1000);        
      
      if(!RTMP_Connect(rtmp,NULL)){  
      //  RTMP_Log(RTMP_LOGERROR,"Connect Err\n");  
        RTMP_Free(rtmp);  
        return KK_AVERROR(EAGAIN); 
      }
	  if(!RTMP_ConnectStream(rtmp,0)){  
       // RTMP_Log(RTMP_LOGERROR,"ConnectStream Err\n");  
        RTMP_Close(rtmp);  
        RTMP_Free(rtmp);  
        return KK_AVERROR(EAGAIN);  
      }  
	  plu->opaque=rtmp;
  }

   RTMP *rtmp=(RTMP *)plu->opaque;
   int nRead=RTMP_Read(rtmp,(char*)buf,buf_size);
   if(nRead<=0)
   {
      return  KK_AVERROR_EOF;
   }
   
   if(plu->CalPlayerDelay!=NULL&&rtmp->m_read.timestamp>0&&rtmp->m_read.pkgtype == RTMP_PACKET_TYPE_AUDIO)
		{
			plu->CalPlayerDelay(plu->PlayerOpaque,rtmp->m_read.timestamp,0);
		}
  
   return nRead;
}

extern "C"{
	    
		
		KKPlugin* CreatelibRtmpPlugin()
		{

			
			KKPlugin* librtmp_plugin =(KKPlugin* ) ::malloc(sizeof(KKPlugin));
			memset(librtmp_plugin,0,sizeof(KKPlugin));
            librtmp_plugin->RealTime=1;
			librtmp_plugin->kkread=librtmp_read_packet;
			return  librtmp_plugin;

		}
};

void AddlibRtmpPluginInfo()
{
	KKPluginInfo PluInfo;
	memset(&PluInfo,0,sizeof(KKPluginInfo));

	PluInfo.CreKKP=CreatelibRtmpPlugin;
	//PluInfo.DelKKp=DelSrsRtmpPlugin;
	strcpy(PluInfo.ptl,"librtmp");
	KKPlayer::AddKKPluginInfo(PluInfo);
}