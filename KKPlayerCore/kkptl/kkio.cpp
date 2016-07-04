//#include "../Includeffmpeg.h"
//3.0以后版本取消了ffurl_register_protocol
extern "C"
{
   #include "../libx86/ffmpeg/include/libavutil/opt.h"
   #include "../libx86/ffmpeg/include/libavutil/log.h"
   #include "../libx86/ffmpeg/include/libavformat/avio.h"
   typedef struct KkioContext{
			/*int (*read_packet)(void *opaque, uint8_t *buf, int buf_size);
			int (*write_packet)(void *opaque, uint8_t *buf, int buf_size);
			int64_t (*seek)(void *opaque, int64_t offset, int whence);*/
            FILE *f;
			char urlstr[256];
			AVIOContext *pb;
   }KkioContext;/**/
};

int Kkptl_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
   KkioContext *pCtx=(KkioContext *)opaque;
   if(pCtx->f==NULL)
   {
       pCtx->f=fopen(pCtx->urlstr,"rb");
   }
   int len=::fread(buf,1,buf_size,pCtx->f);
   return len;
}

/* Some systems may not have S_ISFIFO */
#ifndef S_ISFIFO
#  ifdef S_IFIFO
#    define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#  else
#    define S_ISFIFO(m) 0
#  endif
#endif

int64_t Kkptl_seek(void *opaque, int64_t offset, int whence)
{
   KkioContext *pCtx=(KkioContext *)opaque;

   int64_t ret;

   if (whence == AVSEEK_SIZE) {
	  
	   fseek(pCtx->f,0,SEEK_END); //定位到文件末 
	   ret = ftell(pCtx->f); //文件长度
	   fseek(pCtx->f,0,SEEK_SET ); //定位到文件开始
	   return ret < 0 ? AVERROR(errno) :ret;
   }

   ret = ::fseek(pCtx->f,offset, whence);

   return ret < 0 ? AVERROR(errno) : ret;

}

AVIOContext * CreateKKIo(char *strUrl)
{
	KkioContext *pCtx=(KkioContext *)av_malloc(sizeof(KkioContext));  
	memset(pCtx,0,sizeof(KkioContext));
	strcpy(pCtx->urlstr,strUrl);
    size_t len=32768;
	unsigned char *aviobuffer=(unsigned char*)av_malloc(len);  

	AVIOContext *avio=avio_alloc_context(aviobuffer, len,0,pCtx, Kkptl_read_packet,NULL,Kkptl_seek); 
	pCtx->pb=avio;
	return avio;
}

