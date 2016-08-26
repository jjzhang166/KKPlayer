
//#include "../Includeffmpeg.h"
//extern "C"{
//	#include "libavformat/avio.h"
//};
typedef unsigned long long uint64_t;
#include "../KKVideoInfo.h"
void Packet_Queue_All_Flush(SKK_VideoState *pVideoInfo);
//extern "C"{
	void Queue_All_Flush(void *pVideoInfo)
	{
		Packet_Queue_All_Flush((SKK_VideoState *)pVideoInfo);
	}
//}
AVIOContext * CreateKKIo(SKK_VideoState *kkAV)
{
	/*int int ll=AVERROR_EOF;
	AVERROR(errno)*/
	size_t len=32768;
	unsigned char *aviobuffer=(unsigned char*)av_malloc(len);  
	KKPlugin* pKKP=kkAV->pKKPluginInfo->CreKKP();
	pKKP->URL=kkAV->filename;
	pKKP->kkirqOpaque=kkAV;
	pKKP->FlushQue= Queue_All_Flush;
	pKKP->kkirq=kkAV->pFormatCtx->interrupt_callback.callback;
	AVIOContext *avio=avio_alloc_context(aviobuffer, len,0,pKKP, pKKP->kkread,NULL,pKKP->kkseek); 
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