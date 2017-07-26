#include "../librtmp/rtmp.h"
#include <string.h>
#include "../KKPlayer.h"
static int librtmp_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
  KKPlugin* plu=(KKPlugin*)opaque;

  if(plu->opaque==NULL){
      RTMP *rtmp=RTMP_Alloc();  
      RTMP_Init(rtmp);
	  rtmp->kkirq=plu->kkirq;
	  rtmp->player=plu->PlayerOpaque;
	 
	  char url[2048]="rtmp://live.hkstv.hk.lxdns.com/live/hks";
	  strcpy(url,plu->URL);
	  if(!RTMP_SetupURL(rtmp,url))  
      { 
		   return KK_AVERROR(EAGAIN);
	  }
	 
	  RTMP_SetBufferMS(rtmp, 1);
	  rtmp->Link.timeout = 30; 
	  rtmp->m_nBufferMS=500;
      rtmp->Link.lFlags |= RTMP_LF_LIVE;
	  //plu->kkirq
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
	/*  RTMP_Close(rtmp);  
	  RTMP_Close(rtmp);  */
	  plu->opaque=rtmp;
  }

   RTMP *rtmp=(RTMP *)plu->opaque;
   int nRead=RTMP_Read(rtmp,(char*)buf,buf_size);
   if(nRead<=0)
   {
      return  KK_AVERROR_EOF;
   }
   
   if(plu->CalPlayerDelay!=NULL&&rtmp->m_read.timestamp>0&&rtmp->m_read.pkgtype == RTMP_PACKET_TYPE_AUDIO){
			plu->CalPlayerDelay(plu->PlayerOpaque,rtmp->m_read.timestamp,0);
	}
  
   return nRead;
}

static void libPlayerWillClose(void *opaque)
{
	if(opaque!=0){
		 RTMP *rtmp= (RTMP *)opaque;
		 if(rtmp!=0)
		 {
		   RTMPSockBuf_Close(&rtmp->m_sb);
		   rtmp->m_sb.sb_socket = -1;
		 }
	}
}
void DellibRtmpPlugin(KKPlugin* p)
{
	RTMP *rtmp= (RTMP *)p->opaque;
	 if(RTMP_ConnectStream(rtmp,0)){ 
	    RTMP_Close(rtmp); 
	 }
    RTMP_Free(rtmp);
	::free(p);
}
extern "C"{
	    
		
		KKPlugin* CreatelibRtmpPlugin()
		{			
			KKPlugin* librtmp_plugin =(KKPlugin* ) ::malloc(sizeof(KKPlugin));
			memset(librtmp_plugin,0,sizeof(KKPlugin));
            librtmp_plugin->RealTime=1;
			librtmp_plugin->kkread=librtmp_read_packet;
			librtmp_plugin->kkPlayerWillClose=libPlayerWillClose;
			return  librtmp_plugin;
		}
};

void AddlibRtmpPluginInfo()
{
	KKPluginInfo PluInfo;
	memset(&PluInfo,0,sizeof(KKPluginInfo));

	PluInfo.CreKKP=CreatelibRtmpPlugin;
	PluInfo.DelKKp=DellibRtmpPlugin;
	strcpy(PluInfo.ptl,"librtmp");
	KKPlayer::AddKKPluginInfo(PluInfo);
}