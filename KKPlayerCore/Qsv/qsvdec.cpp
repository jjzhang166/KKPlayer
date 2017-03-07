#include <string.h>
#include <sys/types.h>
#include "../Includeffmpeg.h"
extern "C"{
#include <mfx/mfxvideo.h>
#include "libavutil/common.h"
#include "libavutil/mem.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"
#include "libavutil/time.h"

#include "libavcodec/avcodec.h"

int av_get_format(AVCodecContext *avctx, const enum AVPixelFormat *fmt);
int av_get_buffer(AVCodecContext *avctx, AVFrame *frame, int flags);
}

//#include "internal.h"
#include "qsv.h"
#include "qsv_internal.h"
#include "qsvdec.h"
#include <assert.h>
int ff_qsv_map_pixfmt(enum AVPixelFormat format)
{
    switch (format) {
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUVJ420P:
        return AV_PIX_FMT_NV12;
    default:
        return AVERROR(ENOSYS);
    }
}

mfxStatus ExtendMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize);
mfxStatus InitMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize);
static int qsv_decode_init(AVCodecContext *avctx, KKQSVContext *q, AVPacket *avpkt)
{

	if (!avctx->hwaccel_context){
	    assert(0);
	}
	KK_AVQSVContext *qsv = ( KK_AVQSVContext *)avctx->hwaccel_context;
	q->session        = qsv->session;
    q->iopattern      = qsv->iopattern;
    q->ext_buffers    = qsv->ext_buffers;
    q->nb_ext_buffers = qsv->nb_ext_buffers;
	q->async_depth=     qsv->param.AsyncDepth;
    //q->iopattern  = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;//MFX_IOPATTERN_OUT_SYSTEM_MEMORY
	mfxVideoParam param = { { 0 } };
	mfxBitstream bs   = { { { 0 } } };
	//mfxBitstream&    bs   = qsv->mfx_enc_bs;
    int ret;
    enum AVPixelFormat pix_fmts[3] = { AV_PIX_FMT_QSV,AV_PIX_FMT_NV12,AV_PIX_FMT_NONE };

    ret = av_get_format(avctx, pix_fmts);
    if (ret < 0)
        return ret;

    avctx->pix_fmt      = (AVPixelFormat)ret;
	
    if (!q->session) {
        assert(0);
    }

	if (avpkt->size) {
        bs.Data       = avpkt->data;
        bs.DataLength = avpkt->size;
        bs.MaxLength  = bs.DataLength;
        bs.TimeStamp  = avpkt->pts;
    } else
        return AVERROR_INVALIDDATA;

   /* if (avpkt->size) {

		 if (bs.MaxLength == bs.DataLength){
               ret = ExtendMfxBitstream(&bs, bs.MaxLength * 2);
         }
         if(bs.DataLength>0)
		 memmove(bs.Data, bs.Data + bs.DataOffset,bs.DataLength);
         bs.DataOffset = 0;
		 memcpy(bs.Data+ bs.DataLength,avpkt->data,avpkt->size);
         bs.DataLength += avpkt->size;
         bs.TimeStamp  = avpkt->pts;
		 FILE * bd=::fopen("F:/BaiduNetdiskDownload/test10.h26","wb");
		 fwrite(bs.Data,1,bs.DataLength,bd);
		 fclose(bd);
    } else
        return AVERROR_INVALIDDATA;*/

	 
    ret = ff_qsv_codec_id_to_mfx(avctx->codec_id);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "Unsupported codec_id %08x\n", avctx->codec_id);
        return ret;
    }
    int xc=MFX_FOURCC_NV12;
    param.mfx.CodecId = ret;


	
    ret = MFXVideoDECODE_DecodeHeader(q->session, &bs, &param);
    if (MFX_ERR_MORE_DATA==ret) {
        /* this code means that header not found so we return packet size to skip
           a current packet
         */
        return avpkt->size;
    } else if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "Decode header error %d\n", ret);
        return ff_qsv_error(ret);
    }

	

    mfxStatus sts=MFXVideoDECODE_Query(q->session,&param, &param);
	
	 if (sts < 0) {
		return ff_qsv_error(sts);
	 }
	
	param.IOPattern   = q->iopattern;
    param.AsyncDepth  = q->async_depth;
    param.ExtParam    = q->ext_buffers;
    param.NumExtParam = q->nb_ext_buffers;
    param.mfx.FrameInfo.BitDepthLuma   = 8;
    param.mfx.FrameInfo.BitDepthChroma = 8;
    param.mfx.Rotation = 0;
	qsv->param=param;

	mfxFrameAllocRequest request;
	memset(&request,0,sizeof(request));
	ret=MFXVideoDECODE_QueryIOSurf(q->session, &param,&request);
	
	/**/
	//sts=MFXVideoDECODE_Query(decCtx->mfx_session,&decCtx->dec_param, &decCtx->dec_param);
    ret = MFXVideoDECODE_Init(q->session, &param);
    if (ret < 0) {
        if (MFX_ERR_INVALID_VIDEO_PARAM==ret) {
            av_log(avctx, AV_LOG_ERROR,
                   "Error initializing the MFX video decoder, unsupported video\n");
        } else {
            av_log(avctx, AV_LOG_ERROR,
                   "Error initializing the MFX video decoder %d\n", ret);
        }
        return ff_qsv_error(ret);
    }

	

    avctx->profile      = param.mfx.CodecProfile;
    avctx->level        = param.mfx.CodecLevel;
    avctx->coded_width  = param.mfx.FrameInfo.Width;
    avctx->coded_height = param.mfx.FrameInfo.Height;
	int w=param.mfx.FrameInfo.CropW - param.mfx.FrameInfo.CropX;
	int h=param.mfx.FrameInfo.CropH - param.mfx.FrameInfo.CropY;
	avctx->width        = w==0 ? avctx->coded_width:w;
	avctx->height       = h==0 ? avctx->coded_height:h;

    /* maximum decoder latency should be not exceed max DPB size for h.264 and
       HEVC which is 16 for both cases.
       So weare  pre-allocating fifo big enough for 17 elements:
     */
    if (!q->async_fifo) {
        q->async_fifo = av_fifo_alloc((1 + 16) *
                                      (sizeof(mfxSyncPoint*) + sizeof(KKQSVFrame*)));
        if (!q->async_fifo)
            return AVERROR(ENOMEM);
    }

    if (!q->input_fifo) {
        q->input_fifo = av_fifo_alloc(1024*16);
        if (!q->input_fifo)
            return AVERROR(ENOMEM);
    }

    if (!q->pkt_fifo) {
        q->pkt_fifo = av_fifo_alloc( sizeof(AVPacket) * (1 + 16) );
        if (!q->pkt_fifo)
            return AVERROR(ENOMEM);
    }
    q->engine_ready = 1;

    return 0;
}

static int alloc_frame(AVCodecContext *avctx, KKQSVFrame *frame)
{
    int ret;

    ret = av_get_buffer(avctx, frame->frame, AV_GET_BUFFER_FLAG_REF);
    if (ret < 0)
        return ret;

    if (frame->frame->format == AV_PIX_FMT_QSV) {
        frame->surface = (mfxFrameSurface1*)frame->frame->data[3];
    } else {
        frame->surface_internal.Info.BitDepthLuma   = 8;
        frame->surface_internal.Info.BitDepthChroma = 8;
        frame->surface_internal.Info.FourCC         = MFX_FOURCC_NV12;
        frame->surface_internal.Info.Width          = avctx->coded_width;
        frame->surface_internal.Info.Height         = avctx->coded_height;
        frame->surface_internal.Info.ChromaFormat   = MFX_CHROMAFORMAT_YUV420;

        frame->surface_internal.Data.PitchLow = frame->frame->linesize[0];
        frame->surface_internal.Data.Y        = frame->frame->data[0];
        frame->surface_internal.Data.UV       = frame->frame->data[1];

        frame->surface = &frame->surface_internal;
    }

    return 0;
}

static void qsv_clear_unused_frames(KKQSVContext *q)
{
    KKQSVFrame *cur = (KKQSVFrame *)q->work_frames;
    while (cur) {
        if (cur->surface && !cur->surface->Data.Locked && !cur->queued) {
            //cur->surface = NULL;
            //av_frame_unref(cur->frame);
        }
        cur = cur->next;
    }
}

static int get_surface(AVCodecContext *avctx, KKQSVContext *q, mfxFrameSurface1 **surf)
{
    KKQSVFrame *frame, **last;
    int ret;

    qsv_clear_unused_frames(q);

    frame = q->work_frames;
    last  = &q->work_frames;
    while (frame) {
        if (!frame->surface) {
            ret = alloc_frame(avctx, frame);
            if (ret < 0)
                return ret;
            *surf = frame->surface;
            return 0;
        }

        last  = &frame->next;
        frame = frame->next;
    }

    frame =(KKQSVFrame *) av_mallocz(sizeof(*frame));
    if (!frame)
        return AVERROR(ENOMEM);
    frame->frame = av_frame_alloc();
    if (!frame->frame) {
        av_freep(&frame);
        return AVERROR(ENOMEM);
    }
    *last = frame;

    ret = alloc_frame(avctx, frame);
    if (ret < 0)
        return ret;

    *surf = frame->surface;

    return 0;
}

static KKQSVFrame *find_frame(KKQSVContext *q, mfxFrameSurface1 *surf)
{
    KKQSVFrame *cur = q->work_frames;
    while (cur) {
        if (surf == cur->surface)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

/*  This function uses for 'smart' releasing of consumed data
    from the input bitstream fifo.
    Since the input fifo mapped to mfxBitstream which does not understand
    a wrapping of data over fifo end, we should also to relocate a possible
    data rest to fifo begin. If rest of data is absent then we just reset fifo's
    pointers to initial positions.
    NOTE the case when fifo does contain unconsumed data is rare and typical
    amount of such data is 1..4 bytes.
*/
static void qsv_fifo_relocate(AVFifoBuffer *f, int bytes_to_free)
{
    int data_size;
    int data_rest = 0;

    av_fifo_drain(f, bytes_to_free);

    data_size = av_fifo_size(f);
    if (data_size > 0) {
        if (f->buffer!=f->rptr) {
            if ( (f->end - f->rptr) < data_size) {
                data_rest = data_size - (f->end - f->rptr);
                data_size-=data_rest;
                memmove(f->buffer+data_size, f->buffer, data_rest);
            }
            memmove(f->buffer, f->rptr, data_size);
            data_size+= data_rest;
        }
    }
    f->rptr = f->buffer;
    f->wptr = f->buffer + data_size;
    f->wndx = data_size;
    f->rndx = 0;
}


static void close_decoder(KKQSVContext *q)
{
    KKQSVFrame *cur;

    if (q->session)
        MFXVideoDECODE_Close(q->session);

    while (q->async_fifo && av_fifo_size(q->async_fifo)) {
        KKQSVFrame *out_frame;
        mfxSyncPoint *sync;

        av_fifo_generic_read(q->async_fifo, &out_frame, sizeof(out_frame), NULL);
        av_fifo_generic_read(q->async_fifo, &sync,      sizeof(sync),      NULL);

        av_freep(&sync);
    }

    cur = q->work_frames;
    while (cur) {
        q->work_frames = cur->next;
        av_frame_free(&cur->frame);
        av_freep(&cur);
        cur = q->work_frames;
    }

    q->engine_ready   = 0;
    q->reinit_pending = 0;
}


mfxStatus WriteNextFrameI420(mfxFrameSurface1 *pSurface)
{
  
    mfxFrameInfo &pInfo = pSurface->Info;
    mfxFrameData &pData = pSurface->Data;

    mfxU32 i, j, h, w;
    mfxU32 vid = pInfo.FrameId.ViewId;

    FILE * fd=::fopen("D://ttt.yuv","wb");
    // Write Y
    switch (pInfo.FourCC)
    {
        case MFX_FOURCC_YV12:
        case MFX_FOURCC_NV12:
        {
            
			 fwrite(pData.Y + (pInfo.CropY * pData.Pitch + pInfo.CropX)+ i * pData.Pitch, 1, pInfo.CropW,fd);
            break;
        }
        default:
        {
           
            return MFX_ERR_UNSUPPORTED;
        }
    }

    // Write U and V
    switch (pInfo.FourCC)
    {
        case MFX_FOURCC_YV12:
        {
           
            break;
        }
        case MFX_FOURCC_NV12:
        {
            h = pInfo.CropH / 2;
            w = pInfo.CropW;
            for (i = 0; i < h; i++){
                for (j = 0; j < w; j += 2){
                   
                    fwrite(pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX) + i * pData.Pitch + j, 1, 1, fd);
                }
            }
            for (i = 0; i < h; i++)
            {
                for (j = 1; j < w; j += 2)
                {
                   fwrite(pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX)+ i * pData.Pitch + j, 1, 1, fd);
                }
            }
            break;
        }
        default:
        {
           // msdk_printf(MSDK_STRING("ERROR: I420 output is accessible only for NV12 and YV12.\n"));
            return MFX_ERR_UNSUPPORTED;
        }
    }
	::fclose(fd);
    return MFX_ERR_NONE;
}

static int do_qsv_decode(AVCodecContext *avctx, KKQSVContext *q,
                  AVFrame *frame, int *got_frame,
                  AVPacket *avpkt)
{
    KKQSVFrame *out_frame=NULL;
    mfxFrameSurface1 *insurf=NULL;
    mfxFrameSurface1 *outsurf=NULL;
  
    mfxBitstream bs = { { { 0 } } };
    int ret;
    int n_out_frames;
    int buffered = 0;
    int flush    = !avpkt->size || q->reinit_pending;

    if (!q->engine_ready) {
        ret = qsv_decode_init(avctx, q, avpkt);
        if (ret)
            return ret;
    }

    if (!flush) {
        if (av_fifo_size(q->input_fifo)) {
            /* we have got rest of previous packet into buffer */
            if (av_fifo_space(q->input_fifo) < avpkt->size) {
                ret = av_fifo_grow(q->input_fifo, avpkt->size);
                if (ret < 0)
                    return ret;
            }
            av_fifo_generic_write(q->input_fifo, avpkt->data, avpkt->size, NULL);
            bs.Data       = q->input_fifo->rptr;
            bs.DataLength = av_fifo_size(q->input_fifo);
            buffered = 1;
        } else {
            bs.Data       = avpkt->data;
            bs.DataLength = avpkt->size;
        }
        bs.MaxLength  = bs.DataLength;
        bs.TimeStamp  = avpkt->pts;
    }

   

   
        ret = get_surface(avctx, q, &insurf);
        if (ret < 0)
            return ret;
        do {
             ret = MFXVideoDECODE_DecodeFrameAsync(q->session,&bs,insurf, &outsurf, &q->sync);
			 if (MFX_WRN_VIDEO_PARAM_CHANGED==ret) {
				 /* TODO: handle here minor sequence header changing */
				 continue;
			 }
			if(ret==MFX_ERR_MORE_DATA)
			{
			    av_fifo_generic_write(q->input_fifo, avpkt->data,avpkt->size, NULL);
			}
            if (ret != MFX_WRN_DEVICE_BUSY)
                break;
            av_usleep(500);
        } while (1);

        
		//解码成功
        if (ret==MFX_ERR_NONE) {
			qsv_fifo_relocate(q->input_fifo, bs.DataOffset);
			ret = MFXVideoCORE_SyncOperation(q->session, q->sync, 1000);
			if (MFX_ERR_NONE == ret) { 
				KKQSVFrame *out_frame = find_frame(q, outsurf);
				if (!out_frame) {
					av_log(avctx, AV_LOG_ERROR,
						   "The returned surface does not correspond to any frame\n");
					return AVERROR_BUG;
				}

				out_frame->queued = 1;
				av_fifo_generic_write(q->async_fifo, &out_frame, sizeof(out_frame), NULL);
			}
        }
  
   
    if (MFX_ERR_MORE_DATA!=ret && ret < 0) {
      
        av_log(avctx, AV_LOG_ERROR, "Error %d during QSV decoding.\n", ret);
        return ff_qsv_error(ret);
    }

    //获取解码的数据
    n_out_frames = av_fifo_size(q->async_fifo) / (sizeof(out_frame));
    if (n_out_frames > q->async_depth || (flush && n_out_frames) ) {
        AVFrame *src_frame;

        av_fifo_generic_read(q->async_fifo, &out_frame, sizeof(out_frame), NULL);
        out_frame->queued = 0;

        do {
			ret = MFXVideoCORE_SyncOperation(q->session, q->sync, 1000);
        } while (ret == MFX_WRN_IN_EXECUTION);

      
        src_frame = out_frame->frame;

        ret = av_frame_ref(frame, src_frame);
        if (ret < 0)
            return ret;

        outsurf = out_frame->surface;

		
        WriteNextFrameI420(out_frame->surface);
        frame->pkt_pts = frame->pts = outsurf->Data.TimeStamp;

        frame->repeat_pict =
            outsurf->Info.PicStruct & MFX_PICSTRUCT_FRAME_TRIPLING ? 4 :
            outsurf->Info.PicStruct & MFX_PICSTRUCT_FRAME_DOUBLING ? 2 :
            outsurf->Info.PicStruct & MFX_PICSTRUCT_FIELD_REPEATED ? 1 : 0;
        frame->top_field_first =
            outsurf->Info.PicStruct & MFX_PICSTRUCT_FIELD_TFF;
        frame->interlaced_frame =
            !(outsurf->Info.PicStruct & MFX_PICSTRUCT_PROGRESSIVE);

        *got_frame = 1;
    }

    return avpkt->size;
}
/*
 This function inserts a packet at fifo front.
*/
static void qsv_packet_push_front(KKQSVContext *q, AVPacket *avpkt)
{
    int fifo_size = av_fifo_size(q->pkt_fifo);
    if (!fifo_size) {
    /* easy case fifo is empty */
        av_fifo_generic_write(q->pkt_fifo, avpkt, sizeof(*avpkt), NULL);
    } else {
    /* realloc necessary */
        AVPacket pkt;
        AVFifoBuffer *fifo = av_fifo_alloc(fifo_size+av_fifo_space(q->pkt_fifo));

        av_fifo_generic_write(fifo, avpkt, sizeof(*avpkt), NULL);

        while (av_fifo_size(q->pkt_fifo)) {
            av_fifo_generic_read(q->pkt_fifo, &pkt, sizeof(pkt), NULL);
            av_fifo_generic_write(fifo,       &pkt, sizeof(pkt), NULL);
        }
        av_fifo_free(q->pkt_fifo);
        q->pkt_fifo = fifo;
    }
}
int ff_qsv_decode(AVCodecContext *avctx, KKQSVContext *q,
                  AVFrame *frame, int *got_frame,
                  AVPacket *avpkt)
{
    AVPacket pkt_ref = { 0 };
    int ret = 0;

    if (q->pkt_fifo && av_fifo_size(q->pkt_fifo) >= sizeof(AVPacket)) {
        /* we already have got some buffered packets. so add new to tail */
        ret = av_packet_ref(&pkt_ref, avpkt);
        if (ret < 0)
            return ret;
        av_fifo_generic_write(q->pkt_fifo, &pkt_ref, sizeof(pkt_ref), NULL);
    }
    if (q->reinit_pending) {
        ret = do_qsv_decode(avctx, q, frame, got_frame, avpkt);

        if (!*got_frame) {
            /* Flushing complete, no more frames  */
            close_decoder(q);
            //return ff_qsv_decode(avctx, q, frame, got_frame, avpkt);
        }
    }
    if (!q->reinit_pending) {
        if (q->pkt_fifo && av_fifo_size(q->pkt_fifo) >= sizeof(AVPacket)) {
            /* process buffered packets */
            while (!*got_frame && av_fifo_size(q->pkt_fifo) >= sizeof(AVPacket)) {
                av_fifo_generic_read(q->pkt_fifo, &pkt_ref, sizeof(pkt_ref), NULL);
                ret = do_qsv_decode(avctx, q, frame, got_frame, &pkt_ref);
                if (q->reinit_pending) {
                    /*
                       A rare case: new reinit pending when buffering existing.
                       We should to return the pkt_ref back to same place of fifo
                    */
                    qsv_packet_push_front(q, &pkt_ref);
                } else {
                    av_packet_unref(&pkt_ref);
                }
           }
        } else {
            /* general decoding */
            ret = do_qsv_decode(avctx, q, frame, got_frame, avpkt);
            if (q->reinit_pending) {
                ret = av_packet_ref(&pkt_ref, avpkt);
                if (ret < 0)
                    return ret;
                av_fifo_generic_write(q->pkt_fifo, &pkt_ref, sizeof(pkt_ref), NULL);
            }
        }
    }

    return ret;
}
/*
 This function resets decoder and corresponded buffers before seek operation
*/
void ff_qsv_decode_reset(AVCodecContext *avctx, KKQSVContext *q)
{
    KKQSVFrame *cur;
    AVPacket pkt;
    int ret = 0;
    mfxVideoParam param = { { 0 } };

    if (q->reinit_pending) {
        close_decoder(q);
    } else if (q->engine_ready) {
        ret = MFXVideoDECODE_GetVideoParam(q->session, &param);
        if (ret < 0) {
            av_log(avctx, AV_LOG_ERROR, "MFX decode get param error %d\n", ret);
        }

        ret = MFXVideoDECODE_Reset(q->session, &param);
        if (ret < 0) {
            av_log(avctx, AV_LOG_ERROR, "MFX decode reset error %d\n", ret);
        }

        /* Free all frames*/
        cur = q->work_frames;
        while (cur) {
            q->work_frames = cur->next;
            av_frame_free(&cur->frame);
            av_freep(&cur);
            cur = q->work_frames;
        }
    }

    /* Reset output surfaces */
	if(q->async_fifo!=NULL)
    av_fifo_reset(q->async_fifo);

    /* Reset input packets fifo */
	if(q->pkt_fifo)
    while (av_fifo_size(q->pkt_fifo)) {
        av_fifo_generic_read(q->pkt_fifo, &pkt, sizeof(pkt), NULL);
        av_packet_unref(&pkt);
    }

    /* Reset input bitstream fifo */
	if(q->input_fifo)
    av_fifo_reset(q->input_fifo);
}

int ff_qsv_decode_close(KKQSVContext *q)
{
    close_decoder(q);

    q->session = NULL;

    ff_qsv_close_internal_session(&q->internal_qs);

    av_fifo_free(q->async_fifo);
    q->async_fifo = NULL;

    av_fifo_free(q->input_fifo);
    q->input_fifo = NULL;

    av_fifo_free(q->pkt_fifo);
    q->pkt_fifo = NULL;

    return 0;
}
