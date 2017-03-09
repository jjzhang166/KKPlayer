#include "../Includeffmpeg.h"
#include <tchar.h>
#include "kkQsvDecoder.h"


extern "C"{
#include <mfx/mfxdefs.h>
#include <mfx/mfxvideo.h>

}
#include "qsv.h"
void *KK_Malloc_(size_t size);
void  KK_Free_(void *ptr);

#define msdk_printf   _tprintf
#define MSDK_STRING(x) _T(x)
#define MSDK_CHAR(x) _T(x)
#define MSDK_CHECK_POINTER(P, ...)               {if (!(P)) {return __VA_ARGS__;}}
#define MSDK_PRINT_RET_MSG(ERR) {msdk_printf(MSDK_STRING("\nReturn on error: error code %d,\t%s\t%d\n\n"), (int)ERR, MSDK_STRING(__FILE__), __LINE__);}
#define MSDK_CHECK_ERROR(P, X, ERR)              {if ((X) == (P)) {MSDK_PRINT_RET_MSG(ERR); return ERR;}}
#define MSDK_SAFE_DELETE_ARRAY(P)                {if (P) {KK_Free_(P); P = NULL;}}
#define MSDK_ALIGN32(X) (((mfxU32)((X)+31)) & (~ (mfxU32)31))
#define MSDK_MEMCPY_VAR(dstVarName, src, count) memcpy_s(&(dstVarName), sizeof(dstVarName), (src), (count))
#define ID_BUFFER MFX_MAKEFOURCC('B','U','F','F')
#define ID_FRAME  MFX_MAKEFOURCC('F','R','M','E')


//QSV硬解码环境
typedef struct KKQSVDecCtx
{
	//版本号
	mfxVersion            mfx_ver;
    mfxSession            mfx_session;
	
	//work surfaces
    mfxFrameSurface1     surfaces[17];
    mfxMemId             MemIds[17];
    int                  surface_used[17];

    int                   nb_surfaces;
	int                   nb_cursurId;
	//解码参数
    mfxVideoParam         dec_param;
    mfxFrameInfo          frame_info;
	mfxSyncPoint          syncp;

	mfxFrameAllocator     frame_allocator;
	bool                  de_header;
	int                   picw;
	int                   pich;
	KK_AVQSVContext          *hw_ctx;
	
	mfxFrameAllocResponse resp;
	mfxFrameAllocRequest request;

	AVFrame* tmp_frame;
	bool Okxx;
}KKQSVDecCtx;

static const mfxU32 MEMTYPE_FROM_MASK = MFX_MEMTYPE_FROM_ENCODE | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN | MFX_MEMTYPE_FROM_VPPOUT | MFX_MEMTYPE_FROM_ENC | MFX_MEMTYPE_FROM_PAK;
static mfxStatus CheckRequestType(mfxFrameAllocRequest *request)
{
    if (0 == request)
        return MFX_ERR_NULL_PTR;

    // check that Media SDK component is specified in request
    if ((request->Type & MEMTYPE_FROM_MASK) != 0)
        return MFX_ERR_NONE;
    else
        return MFX_ERR_UNSUPPORTED;
}
static mfxStatus frame_alloc(mfxHDL pthis, mfxFrameAllocRequest *request,mfxFrameAllocResponse *resp)
{
    KKQSVDecCtx *decode = (KKQSVDecCtx *)pthis;
    int err, i;

    
    /*if (!(req->Type & MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET)) {
        fprintf(stderr, "Unsupported surface type: %d\n", req->Type);
        return MFX_ERR_UNSUPPORTED;
    }*/
    if (request->Info.BitDepthLuma != 8 || request->Info.BitDepthChroma != 8 ||request->Info.Shift || request->Info.FourCC != MFX_FOURCC_NV12 ||request->Info.ChromaFormat != MFX_CHROMAFORMAT_YUV420) 
	{
        fprintf(stderr, "Unsupported surface properties.\n");
        return MFX_ERR_UNSUPPORTED;
    }

   
	request->Type |= MFX_MEMTYPE_SYSTEM_MEMORY;

	mfxStatus sts =CheckRequestType(request);
    decode->nb_surfaces = request->NumFrameSuggested;


	mfxU32 numAllocated = 0;

	//计算存储空间大小
    mfxU32 Width2 = MSDK_ALIGN32(request->Info.Width);
    mfxU32 Height2 = MSDK_ALIGN32(request->Info.Height);
    mfxU32 nbytes=0;

    switch (request->Info.FourCC)
    {
		case MFX_FOURCC_YV12:
		case MFX_FOURCC_NV12:
			nbytes = Width2*Height2 + (Width2>>1)*(Height2>>1) + (Width2>>1)*(Height2>>1);
			break;
		case MFX_FOURCC_NV16:
			nbytes = Width2*Height2 + (Width2>>1)*(Height2) + (Width2>>1)*(Height2);
			break;
		case MFX_FOURCC_RGB3:
			nbytes = Width2*Height2 + Width2*Height2 + Width2*Height2;
			break;
		case MFX_FOURCC_RGB4:
			nbytes = Width2*Height2 + Width2*Height2 + Width2*Height2 + Width2*Height2;
			break;
		case MFX_FOURCC_UYVY:
		case MFX_FOURCC_YUY2:
			nbytes = Width2*Height2 + (Width2>>1)*(Height2) + (Width2>>1)*(Height2);
			break;
		case MFX_FOURCC_R16:
			nbytes = 2*Width2*Height2;
			break;
		case MFX_FOURCC_P010:
			nbytes = Width2*Height2 + (Width2>>1)*(Height2>>1) + (Width2>>1)*(Height2>>1);
			nbytes *= 2;
			break;
		case MFX_FOURCC_A2RGB10:
			nbytes = Width2*Height2*4; // 4 bytes per pixel
			break;
		case MFX_FOURCC_P210:
			nbytes = Width2*Height2 + (Width2>>1)*(Height2) + (Width2>>1)*(Height2);
			nbytes *= 2; // 16bits
			break;
		default:
			return MFX_ERR_UNSUPPORTED;
    }

	int tx=nbytes+MSDK_ALIGN32(sizeof(mfxFrameData));
    for (i = 0; i < decode->nb_surfaces; i++){ 
		     //分配帧内存
		     decode->MemIds[i]=(mfxMemId   *)::KK_Malloc_(tx);
			 
			 mfxFrameSurface1* sur=&decode->surfaces[i];
			 MSDK_MEMCPY_VAR(sur->Info, &( request->Info), sizeof(mfxFrameInfo)); 
			 mfxFrameData *ptr=&decode->surfaces[i].Data;
             
			 
			
			 ptr->B = ptr->Y =(mfxU8   *)decode->MemIds[i];
			 ptr->MemType=MFX_MEMTYPE_SYSTEM_MEMORY;
			 ptr->MemId=ptr->B;
			 switch (decode->surfaces[i].Info.FourCC){
					case MFX_FOURCC_NV12:
						ptr->U = ptr->Y + Width2 * Height2;
						ptr->V = ptr->U + 1;
						ptr->Pitch = Width2;
						break;
					case MFX_FOURCC_NV16:
						ptr->U = ptr->Y + Width2 * Height2;
						ptr->V = ptr->U + 1;
						ptr->Pitch = Width2;
						break;
					case MFX_FOURCC_YV12:
						ptr->V = ptr->Y + Width2 * Height2;
						ptr->U = ptr->V + (Width2 >> 1) * (Height2 >> 1);
						ptr->Pitch = Width2;
						break;
					case MFX_FOURCC_UYVY:
						ptr->U = ptr->Y;
						ptr->Y = ptr->U + 1;
						ptr->V = ptr->U + 2;
						ptr->Pitch = 2 * Width2;
						break;
					case MFX_FOURCC_YUY2:
						ptr->U = ptr->Y + 1;
						ptr->V = ptr->Y + 3;
						ptr->Pitch = 2 * Width2;
						break;
					case MFX_FOURCC_RGB3:
						ptr->G = ptr->B + 1;
						ptr->R = ptr->B + 2;
						ptr->Pitch = 3 * Width2;
						break;
					case MFX_FOURCC_RGB4:
					case MFX_FOURCC_A2RGB10:
						ptr->G = ptr->B + 1;
						ptr->R = ptr->B + 2;
						ptr->A = ptr->B + 3;
						ptr->Pitch = 4 * Width2;
						break;
					 case MFX_FOURCC_R16:
						ptr->Y16 = (mfxU16 *)ptr->B;
						ptr->Pitch = 2 * Width2;
						break;
					case MFX_FOURCC_P010:
						ptr->U = ptr->Y + Width2 * Height2 * 2;
						ptr->V = ptr->U + 2;
						ptr->Pitch = Width2 * 2;
						break;
					case MFX_FOURCC_P210:
						ptr->U = ptr->Y + Width2 * Height2 * 2;
						ptr->V = ptr->U + 2;
						ptr->Pitch = Width2 * 2;
						break;
					default:
						return MFX_ERR_UNSUPPORTED;
				}
   
	}

	resp->mids           = decode->MemIds;
    resp->NumFrameActual = decode->nb_surfaces;
    decode->frame_info =   request->Info;
//decode->nb_surfaces=
    return MFX_ERR_NONE;
fail:
   
    return MFX_ERR_MEMORY_ALLOC;
}



static mfxStatus frame_free(mfxHDL pthis, mfxFrameAllocResponse *resp)
{
    return MFX_ERR_NONE;
}

static mfxStatus frame_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr)
{
	ptr->Locked=1;
    return MFX_ERR_UNSUPPORTED;
}

static mfxStatus frame_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr)
{
	ptr->Locked=0;
    return MFX_ERR_UNSUPPORTED;
}

static mfxStatus frame_get_hdl(mfxHDL pthis, mfxMemId mid, mfxHDL *hdl)
{
	return MFX_ERR_UNSUPPORTED;
    *hdl = mid;
    return MFX_ERR_NONE;
}

static void free_surfaces( KKQSVDecCtx *decode)
{

    decode->nb_surfaces = 0;
}

static enum AVPixelFormat Qsv_GetFormat( AVCodecContext *p_context,const enum AVPixelFormat *pix_fmts )
{
	 while (*pix_fmts != AV_PIX_FMT_NONE)
	 {
        if(*pix_fmts == AV_PIX_FMT_QSV)
		{
            return AV_PIX_FMT_QSV;
		}
        pix_fmts++;
    }

	return avcodec_default_get_format( p_context,pix_fmts );
}

static void free_buffer(void *opaque, uint8_t *data)
{
    int *i=(int *)opaque;
	*i=0;
}
static int Qsv_GetFrameBuf( struct AVCodecContext *avctx, AVFrame *frame,int flags)
{
	KKQSVDecCtx *decode = (KKQSVDecCtx*)avctx->opaque;

    mfxFrameSurface1 *surf;
    AVBufferRef *surf_buf;
    int idx=0;

	if(decode->nb_surfaces==0)
	{
        mfxVideoParam param = { { 0 } };
		mfxFrameAllocRequest request;
	    memset(&request,0,sizeof(request));
	    param =decode->hw_ctx->param;
	    int ret=MFXVideoDECODE_QueryIOSurf(decode->hw_ctx->session, &param,&request);
	    mfxFrameAllocResponse resp;
	    frame_alloc((mfxHDL)decode, &request,&resp);
	}/**/
	
  for (idx = 0; idx < decode->nb_surfaces; idx++) 
	{
		int  surface_used=decode->surface_used[idx];
        if (!surface_used)
		{ 
            break;
		}
    }/*
	decode->nb_cursurId++;
	if(decode->nb_cursurId>decode->nb_surfaces-1){
	    decode->nb_cursurId=0;
	}
    idx =decode->nb_cursurId;*/
	
    if (idx == decode->nb_surfaces) {
        fprintf(stderr, "No free surfaces\n");
        return AVERROR(ENOMEM);
    }

	
    surf =&decode->surfaces[idx]; //( mfxFrameSurface1 *)av_mallocz(sizeof(*surf));
	
    if (!surf)
        return AVERROR(ENOMEM);
	int *i=&decode->surface_used[idx];
    surf_buf = av_buffer_create(0,NULL, free_buffer,i, AV_BUFFER_FLAG_READONLY);
    if (!surf_buf) {
        av_freep(&surf);
        return AVERROR(ENOMEM);
    }

	
    frame->buf[0]  = surf_buf;
    frame->data[3] = (uint8_t*)surf;

    decode->surface_used[idx] = 1;
	return 0;
}




void Registerkk_h264_qsv_decoder();


void KKFreeQsv(AVCodecContext *avct)
{
	KKQSVDecCtx *decCtx=(KKQSVDecCtx*)avct->opaque;

	for(int i=0;i<decCtx->nb_surfaces;++i){
	     KK_Free_(decCtx->MemIds[i]);
		 decCtx->MemIds[i]=NULL;
	}
   av_free(decCtx->hw_ctx);
	KK_Free_(decCtx);
	avct->opaque=NULL;
	//已在解码器中释放
	//MFXVideoDECODE_Close(decCtx->hw_ctx->session);
	//MFXClose(decCtx->hw_ctx->session);
	//decCtx->hw_ctx->session=0;
	
	//decCtx->hw_ctx=NULL;
	avct->hwaccel_context=0;

}
//绑定环境
int BindQsvModule(AVCodecContext  *pCodecCtx)
{
    static int Qsv_i=1;
	if(Qsv_i==1)
       Registerkk_h264_qsv_decoder();
	Qsv_i=0;
	if(pCodecCtx->codec_id==AV_CODEC_ID_H264)
	{
		//pCodecCtx->codec_id=
		pCodecCtx->get_format         = Qsv_GetFormat;
	    pCodecCtx->get_buffer2        = Qsv_GetFrameBuf;
	    pCodecCtx->thread_count       = 1;
	    pCodecCtx->slice_flags       |= SLICE_FLAG_ALLOW_FIELD;

		//硬解
		mfxIMPL impl =MFX_IMPL_HARDWARE;
		
		KKQSVDecCtx *decCtx=(KKQSVDecCtx*)::KK_Malloc_(sizeof(KKQSVDecCtx));

		decCtx->tmp_frame=0;
	    decCtx->Okxx=0;
		decCtx->hw_ctx =kk_av_qsv_alloc_context();
		pCodecCtx->hwaccel_context=decCtx->hw_ctx;
		pCodecCtx->opaque=decCtx;

      
		
		decCtx->picw=pCodecCtx->width;
		decCtx->pich=pCodecCtx->height;
		//h264
		decCtx->dec_param.mfx.CodecId=MFX_CODEC_AVC;
		decCtx->mfx_ver.Major = 1;
	    decCtx->mfx_ver.Minor = 0;
	    mfxStatus sts=MFXInit(impl,&decCtx->mfx_ver, &decCtx->mfx_session); 
		if(sts!= MFX_ERR_NONE){
			  return sts;
		}
	    sts = MFXQueryVersion(decCtx->mfx_session , &decCtx->mfx_ver);  //get real API version of the loaded library
		if(sts!= MFX_ERR_NONE){
		     return sts;
		}
	    MFXClose(decCtx->mfx_session);
       decCtx->mfx_session=0;

	    sts=MFXInit(impl,&decCtx->mfx_ver , &decCtx->mfx_session); 
		if(sts!= MFX_ERR_NONE){
		       return sts;
		}


		decCtx->frame_allocator.pthis  = decCtx;
        decCtx->frame_allocator.Alloc  = frame_alloc;
        decCtx->frame_allocator.Lock   = frame_lock;
        decCtx->frame_allocator.Unlock = frame_unlock;
        decCtx->frame_allocator.GetHDL = frame_get_hdl;
        decCtx->frame_allocator.Free   = frame_free;
		decCtx->de_header=false;/**/
	    //MFXVideoCORE_SetHandle(decCtx->mfx_session, MFX_HANDLE_VA_DISPLAY, decode.va_dpy);
		//只有使用 
		//MFXVideoCORE_SetFrameAllocator(decCtx->mfx_session, &decCtx->frame_allocator);
        if(sts!= MFX_ERR_NONE){
		       return sts;
		}

		/* if (MFX_CODEC_CAPTURE != decCtx->dec_param.mfx.CodecId){
            sts = InitMfxBitstream(&decCtx->hw_ctx->mfx_enc_bs, 1024 * 1024);
            if(sts!= MFX_ERR_NONE){
		       return sts;
		    }
         }*/
		decCtx->hw_ctx->param.AsyncDepth=10;
		 decCtx->hw_ctx->iopattern=MFX_IOPATTERN_OUT_SYSTEM_MEMORY;//MFX_IOPATTERN_OUT_VIDEO_MEMORY
		 decCtx->hw_ctx->session=decCtx->mfx_session;
		return 0;
	}
	return -1;

}


