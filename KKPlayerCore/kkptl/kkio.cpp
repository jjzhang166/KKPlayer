
//#include "../Includeffmpeg.h"
//extern "C"{
//	#include "libavformat/avio.h"
//};
typedef unsigned long long uint64_t;
#include "../KKVideoInfo.h"
void Packet_Queue_All_Flush(SKK_VideoState *pVideoInfo);
void Queue_All_Flush(void *pVideoInfo)
{
    SKK_VideoState *	is=	(SKK_VideoState *)pVideoInfo;
	Packet_Queue_All_Flush(is);
}

void CalPlayerDelay(void *opaque,int64_t Pts,int AVType)
{
          SKK_VideoState *	is=	(SKK_VideoState *)opaque;
		  if(is->audio_st!=NULL&& AVType==0)
		  {
			  double cupts= Pts * av_q2d(is->audio_st->time_base);
			  is->nRealtimeDelay=cupts-is->audio_clock;
		  }else if(is->video_st!=NULL&& AVType==1){
			  double cupts= Pts * av_q2d(is->video_st->time_base);
			  is->nRealtimeDelay=cupts-is->video_clock;
		  }

}
AVIOContext * CreateKKIo(SKK_VideoState *kkAV)
{
	/*int int ll=AVERROR_EOF;
	AVERROR(errno)*/
	size_t len=32768;//1024*4;//
	unsigned char *aviobuffer=(unsigned char*)av_malloc(len);  
	KKPlugin* pKKP=kkAV->pKKPluginInfo->CreKKP();
	pKKP->URL=kkAV->filename;
	pKKP->PlayerOpaque=kkAV;
	pKKP->FlushQue= Queue_All_Flush;
	pKKP->CalPlayerDelay=CalPlayerDelay;
	pKKP->kkirq=kkAV->pFormatCtx->interrupt_callback.callback;
	pKKP->FirstRead=1;
	AVIOContext *avio=avio_alloc_context(aviobuffer, len,0,pKKP, pKKP->kkread,NULL,pKKP->kkseek); 

	if(pKKP->RealTime==1)
	{
		kkAV->realtime=1;
		kkAV->pFormatCtx->probesize=1024*4;
		kkAV->pFormatCtx->max_analyze_duration=AV_TIME_BASE; 
	}
	
	return avio;
}

void FreeKKIo(SKK_VideoState *kkAV)
{
	AVIOContext *io=kkAV->pFormatCtx->pb;
    KKPlugin* pKKP=(KKPlugin*)io->opaque;
    
    kkAV->pKKPluginInfo->DelKKp(pKKP);
	::av_free(io->buffer);
    io->buffer=NULL;

	::av_free(io);
}