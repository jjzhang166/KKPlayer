
//#include "../Includeffmpeg.h"
//extern "C"{
//	#include "libavformat/avio.h"
//};
typedef unsigned long long uint64_t;
#include "../KKVideoInfo.h"
/*typedef struct KkioContext
{
	char urlstr[256];
	AVIOContext *pb;
}KkioContext;*/

//int Kkptl_read_packet(void *opaque, uint8_t *buf, int buf_size)
//{
//   KkioContext *pCtx=(KkioContext *)opaque;
//
//   /*if(pCtx->f==NULL)
//         pCtx->f=fopen(pCtx->urlstr,"rb");
//   int ret = ::fread(buf, 1,1024,pCtx->f);
//   if (ret == 0 )
//	   return AVERROR(EAGAIN);
//   return (ret == -1) ? AVERROR(errno) : ret;*/
//   if( pCtx->pFKKv==NULL)
//   {
//	   pCtx->pFKKv= new KKV::CKKV();
//	   pCtx->pFKKv->IniKKvFile(pCtx->urlstr);
//   }
//   int ret=pCtx->pFKKv->ReadKKVFile(buf,buf_size,0);
//   //if (ret == 0 )
//	  // return AVERROR(EAGAIN);
//   //int ll=AVERROR_EOF;//AVERROR(errno);
//   //if(ll==AVERROR_EOF)
//   //{
//	  // ll++;
//   //}
//  return (ret <= 0) ? AVERROR_EOF : ret;/**/
//}
//
///* Some systems may not have S_ISFIFO */
//#ifndef S_ISFIFO
//#  ifdef S_IFIFO
//#    define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
//#  else
//#    define S_ISFIFO(m) 0
//#  endif
//#endif
//
//int64_t Kkptl_seek(void *opaque, int64_t offset, int whence)
//{
//   KkioContext *pCtx=(KkioContext *)opaque;
//   int64_t  ret = pCtx->pFKKv->SeekAVFile(offset,whence);
//   int ll=AVERROR(errno);
//   if(ll==AVERROR_EOF)
//   {
//	   ll++;
//   }
//   return ret < 0 ?  AVERROR(errno)  : ret;/**/
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