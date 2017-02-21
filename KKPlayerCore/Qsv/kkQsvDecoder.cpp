#include "../Includeffmpeg.h"
#include <tchar.h>
#include "kkQsvDecoder.h"

#include <mfxdefs.h>
#include <mfxdefs.h>
#include <mfxvideo.h>
#include <libavcodec/qsv.h>
void *KK_Malloc_(size_t size);
void  KK_Free_(void *ptr);

#define msdk_printf   _tprintf
#define MSDK_STRING(x) _T(x)
#define MSDK_CHAR(x) _T(x)
#define MSDK_CHECK_POINTER(P, ...)               {if (!(P)) {return __VA_ARGS__;}}
#define MSDK_PRINT_RET_MSG(ERR) {msdk_printf(MSDK_STRING("\nReturn on error: error code %d,\t%s\t%d\n\n"), (int)ERR, MSDK_STRING(__FILE__), __LINE__);}
#define MSDK_CHECK_ERROR(P, X, ERR)              {if ((X) == (P)) {MSDK_PRINT_RET_MSG(ERR); return ERR;}}
#define MSDK_SAFE_DELETE_ARRAY(P)                {if (P) {KK_Free_(P); P = NULL;}}
//QSV硬解码环境
typedef struct KKQSVDecCtx
{
	//版本号
	mfxVersion            mfx_ver;
    mfxSession            mfx_session;
	//work surfaces
    mfxFrameSurface1     *surfaces;
	//输入数据
	mfxBitstream          mfx_enc_bs; // contains encoded data
    mfxMemId             *surface_ids;
    int                  *surface_used;
    int                   nb_surfaces;
	//解码参数
    mfxVideoParam         dec_param;
    mfxFrameInfo          frame_info;
	mfxSyncPoint          syncp;

	mfxFrameAllocator     frame_allocator;
	bool                  de_header;
	AVQSVContext          hw_ctx;
	
}KKQSVDecCtx;



static mfxStatus frame_alloc(mfxHDL pthis, mfxFrameAllocRequest *req,
                             mfxFrameAllocResponse *resp)
{
    KKQSVDecCtx *decode = (KKQSVDecCtx *)pthis;
    int err, i;

    if (decode->surfaces) {
        fprintf(stderr, "Multiple allocation requests.\n");
        return MFX_ERR_MEMORY_ALLOC;
    }
    if (!(req->Type & MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET)) {
        fprintf(stderr, "Unsupported surface type: %d\n", req->Type);
        return MFX_ERR_UNSUPPORTED;
    }
    if (req->Info.BitDepthLuma != 8 || req->Info.BitDepthChroma != 8 ||
        req->Info.Shift || req->Info.FourCC != MFX_FOURCC_NV12 ||
        req->Info.ChromaFormat != MFX_CHROMAFORMAT_YUV420) {
        fprintf(stderr, "Unsupported surface properties.\n");
        return MFX_ERR_UNSUPPORTED;
    }

    decode->surfaces     = (mfxFrameSurface1*)av_malloc_array (req->NumFrameSuggested, sizeof(mfxFrameSurface1));
    decode->surface_ids  = (mfxMemId*)av_malloc_array (req->NumFrameSuggested, sizeof(mfxMemId));
    decode->surface_used = (int*)av_mallocz_array(req->NumFrameSuggested, sizeof(int));
    if (!decode->surfaces || !decode->surface_ids || !decode->surface_used)
        goto fail;

   
    decode->nb_surfaces = req->NumFrameSuggested;

    for (i = 0; i < decode->nb_surfaces; i++)
        decode->surface_ids[i] = &decode->surfaces[i];

    resp->mids           = decode->surface_ids;
    resp->NumFrameActual = decode->nb_surfaces;

    decode->frame_info = req->Info;

    return MFX_ERR_NONE;
fail:
    av_freep(&decode->surfaces);
    av_freep(&decode->surface_ids);
    av_freep(&decode->surface_used);

    return MFX_ERR_MEMORY_ALLOC;
}

static mfxStatus frame_free(mfxHDL pthis, mfxFrameAllocResponse *resp)
{
    return MFX_ERR_NONE;
}

static mfxStatus frame_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr)
{
    return MFX_ERR_UNSUPPORTED;
}

static mfxStatus frame_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr)
{
    return MFX_ERR_UNSUPPORTED;
}

static mfxStatus frame_get_hdl(mfxHDL pthis, mfxMemId mid, mfxHDL *hdl)
{
    *hdl = mid;
    return MFX_ERR_NONE;
}




static void WipeMfxBitstream(mfxBitstream* pBitstream)
{
    MSDK_CHECK_POINTER(pBitstream);

    //free allocated memory
    MSDK_SAFE_DELETE_ARRAY(pBitstream->Data);
}

static mfxStatus ExtendMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize)
{
    MSDK_CHECK_POINTER(pBitstream, MFX_ERR_NULL_PTR);

    MSDK_CHECK_ERROR(nSize <= pBitstream->MaxLength, true, MFX_ERR_UNSUPPORTED);

    mfxU8* pData =  (mfxU8*)KK_Malloc_(nSize);
    MSDK_CHECK_POINTER(pData, MFX_ERR_MEMORY_ALLOC);

    memmove(pData, pBitstream->Data + pBitstream->DataOffset, pBitstream->DataLength);

    WipeMfxBitstream(pBitstream);

    pBitstream->Data       = pData;
    pBitstream->DataOffset = 0;
    pBitstream->MaxLength  = nSize;

    return MFX_ERR_NONE;
}
static mfxStatus InitMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize)
{
    //check input params
    MSDK_CHECK_POINTER(pBitstream, MFX_ERR_NULL_PTR);
    MSDK_CHECK_ERROR(nSize, 0, MFX_ERR_NOT_INITIALIZED);

    //prepare pBitstream
    WipeMfxBitstream(pBitstream);

    //prepare buffer
    pBitstream->Data = (mfxU8*)KK_Malloc_(nSize);
    MSDK_CHECK_POINTER(pBitstream->Data, MFX_ERR_MEMORY_ALLOC);

    pBitstream->MaxLength = nSize;

    return MFX_ERR_NONE;
}


static enum AVPixelFormat Qsv_GetFormat( AVCodecContext *p_context,const enum AVPixelFormat *pi_fmt )
{
	
	return avcodec_default_get_format( p_context, pi_fmt );
}

static int Qsv_GetFrameBuf( struct AVCodecContext *p_context, AVFrame *p_ff_pic,int flags)
{
	
	return -1;
}

int BindQsvModule(AVCodecContext  *pCodecCtx)
{
	if(pCodecCtx->codec_id==AV_CODEC_ID_H264)
	{
		pCodecCtx->get_format         = Qsv_GetFormat;
	    pCodecCtx->get_buffer2        = Qsv_GetFrameBuf;
	    pCodecCtx->thread_count       = 1;
	    pCodecCtx->slice_flags       |= SLICE_FLAG_ALLOW_FIELD;

		//硬解
		mfxIMPL impl =MFX_IMPL_HARDWARE;
		KKQSVDecCtx *decCtx=(KKQSVDecCtx*)::KK_Malloc_(sizeof(KKQSVDecCtx));
		pCodecCtx->hwaccel_context=&decCtx->hw_ctx;
		pCodecCtx->opaque=decCtx;
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

	    sts=MFXInit(impl,&decCtx->mfx_ver , &decCtx->mfx_session); 
		if(sts!= MFX_ERR_NONE){
		       return sts;
		}


		decCtx->frame_allocator.pthis = decCtx;
        decCtx->frame_allocator.Alloc = frame_alloc;
        decCtx->frame_allocator.Lock  = frame_lock;
        decCtx->frame_allocator.Unlock = frame_unlock;
        decCtx->frame_allocator.GetHDL = frame_get_hdl;
        decCtx->frame_allocator.Free   = frame_free;
		decCtx->de_header=false;
		MFXVideoCORE_SetFrameAllocator(decCtx->mfx_session, &decCtx->frame_allocator);
        if(sts!= MFX_ERR_NONE){
		       return sts;
		}

		 if (MFX_CODEC_CAPTURE != decCtx->dec_param.mfx.CodecId){
            sts = InitMfxBitstream(&decCtx->mfx_enc_bs, 1024 * 1024);
            if(sts!= MFX_ERR_NONE){
		       return sts;
		    }
         }

		 decCtx->hw_ctx.iopattern=MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
		 decCtx->hw_ctx.session=decCtx->mfx_session;
		 return 0;
		sts = MFXVideoDECODE_DecodeHeader(decCtx->mfx_session, &decCtx->mfx_enc_bs, &decCtx->dec_param);
		if (!sts){
            //m_bVppIsUsed = IsVppRequired(pParams);
			 decCtx->de_header=true;
		}else{
		     decCtx->de_header=false;
		}
		
		//视频是否旋转
		decCtx->dec_param.mfx.Rotation =0;// MFX_ROTATION_0;

		decCtx->dec_param.IOPattern =MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

		decCtx->dec_param.AsyncDepth=4;

		 sts=MFXVideoDECODE_Query(decCtx->mfx_session,&decCtx->dec_param, &decCtx->dec_param);

		 mfxFrameAllocRequest Request;
		 // calculate number of surfaces required for decoder
         sts = MFXVideoDECODE_QueryIOSurf(decCtx->mfx_session,&decCtx->dec_param,&Request);
         //初始化解码器
		 sts =MFXVideoDECODE_Init(decCtx->mfx_session,&decCtx->dec_param);

		 sts =MFXVideoDECODE_GetVideoParam(decCtx->mfx_session,&decCtx->dec_param);


		 mfxFrameSurface1     *CurSurface=NULL;
		 mfxFrameSurface1     *pOutSurface = NULL;
		  MFXVideoDECODE_DecodeFrameAsync(decCtx->mfx_session,&decCtx->mfx_enc_bs,CurSurface,&pOutSurface,&decCtx->syncp);
		return 0;
	}
	return -1;

}