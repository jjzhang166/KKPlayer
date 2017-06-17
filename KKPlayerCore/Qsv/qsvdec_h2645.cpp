

#include <stdint.h>
#include <string.h>

extern "C"{
#include <mfx/mfxvideo.h>

#include "libavutil/common.h"
#include "libavutil/fifo.h"
#include "libavutil/opt.h"

#include "libavcodec/avcodec.h"
}
#include "qsvdec.h"

enum LoadPlugin {
    LOAD_PLUGIN_NONE,
    LOAD_PLUGIN_HEVC_SW,
};

typedef struct KKQSVH2645Context {
    AVClass *_class;
    KKQSVContext qsv;

    int load_plugin;

    // the filter for converting to Annex B
    AVBitStreamFilterContext *bsf;

} KKQSVH2645Context;
void KKFreeQsv(AVCodecContext *avct);
static av_cold int qsv_decode_close(AVCodecContext *avctx)
{
    KKQSVH2645Context *s = ( KKQSVH2645Context*)avctx->priv_data;
    ff_qsv_decode_close(&s->qsv);
    av_bitstream_filter_close(s->bsf);
	KKFreeQsv(avctx);
	avctx->opaque=NULL;
	avctx->hwaccel_context=NULL;
	avctx->get_format         = 0;
	avctx->get_buffer2        = 0;
    return 0;
}

static av_cold int qsv_decode_init(AVCodecContext *avctx)
{
    KKQSVH2645Context *s = ( KKQSVH2645Context*)avctx->priv_data;
    int ret;

    if (avctx->codec_id == AV_CODEC_ID_HEVC && s->load_plugin != LOAD_PLUGIN_NONE) {
        static const char *uid_hevcenc_sw = "15dd936825ad475ea34e35f3f54217a6";

        if (s->qsv.load_plugins[0]) {
            av_log(avctx, AV_LOG_WARNING,
                   "load_plugins is not empty, but load_plugin is not set to 'none'."
                   "The load_plugin value will be ignored.\n");
        } else {
            av_freep(&s->qsv.load_plugins);
            s->qsv.load_plugins = av_strdup(uid_hevcenc_sw);
            if (!s->qsv.load_plugins)
                return AVERROR(ENOMEM);
        }
    }

    if (avctx->codec_id == AV_CODEC_ID_H264) {
        s->bsf = av_bitstream_filter_init("h264_mp4toannexb");
        //regarding ticks_per_frame description, should be 2 for h.264:
        avctx->ticks_per_frame = 2;
    } else
        s->bsf = av_bitstream_filter_init("hevc_mp4toannexb");
    if (!s->bsf) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    return 0;
fail:
    qsv_decode_close(avctx);
    return ret;
}

static int qsv_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame, AVPacket *avpkt)
{
    KKQSVH2645Context *s = ( KKQSVH2645Context*)avctx->priv_data;
    AVFrame *frame    =(AVFrame *) data;
    int ret;
    uint8_t *p_filtered = NULL;
    int      n_filtered = NULL;
    AVPacket pkt_filtered = { 0 };

    if (avpkt->size) {
        if (avpkt->size > 3 && !avpkt->data[0] &&
            !avpkt->data[1] && !avpkt->data[2] && 1==avpkt->data[3]) {
            /* we already have annex-b prefix */
            return ff_qsv_decode(avctx, &s->qsv, frame, got_frame, avpkt);

        } else {
            /* no annex-b prefix. try to restore: */
            ret = av_bitstream_filter_filter(s->bsf, avctx, "private_spspps_buf",
                                         &p_filtered, &n_filtered,
                                         avpkt->data, avpkt->size, 0);
            if (ret>=0) {
                pkt_filtered.pts  = avpkt->pts;
                pkt_filtered.data = p_filtered;
                pkt_filtered.size = n_filtered;

                ret = ff_qsv_decode(avctx, &s->qsv, frame, got_frame, &pkt_filtered);

                if (p_filtered != avpkt->data)
                    av_free(p_filtered);
                return ret > 0 ? avpkt->size : ret;
            }
        }
    }

    return ff_qsv_decode(avctx, &s->qsv, frame, got_frame, avpkt);
}

static void qsv_decode_flush(AVCodecContext *avctx)
{
    KKQSVH2645Context *s = ( KKQSVH2645Context*)avctx->priv_data;
    kk_qsv_decode_flush(avctx, &s->qsv);

	
}
static AVPixelFormat pix_fmts[] ={ AV_PIX_FMT_NV12,AV_PIX_FMT_QSV,AV_PIX_FMT_NONE };
#define OFFSET(x) offsetof(KKQSVH2645Context, x)
#define VD AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_DECODING_PARAM
#define CONFIG_HEVC_QSV_DECODER 
#ifdef CONFIG_HEVC_QSV_DECODER

AVHWAccel ff_hevc_qsv_hwaccel = {"hevc_qsv",AVMEDIA_TYPE_VIDEO,AV_CODEC_ID_HEVC,AV_PIX_FMT_QSV};

//static const AVOption hevc_options[] = {
//    { "async_depth", "Internal parallelization depth, the higher the value the higher the latency.", OFFSET(qsv.async_depth), AV_OPT_TYPE_INT, { .i64 = ASYNC_DEPTH_DEFAULT }, 0, INT_MAX, VD },
//
//    { "load_plugin", "A user plugin to load in an internal session", OFFSET(load_plugin), AV_OPT_TYPE_INT, { .i64 = LOAD_PLUGIN_HEVC_SW }, LOAD_PLUGIN_NONE, LOAD_PLUGIN_HEVC_SW, VD, "load_plugin" },
//    { "none",     NULL, 0, AV_OPT_TYPE_CONST, { .i64 = LOAD_PLUGIN_NONE },    0, 0, VD, "load_plugin" },
//    { "hevc_sw",  NULL, 0, AV_OPT_TYPE_CONST, { .i64 = LOAD_PLUGIN_HEVC_SW }, 0, 0, VD, "load_plugin" },
//
//    { "load_plugins", "A :-separate list of hexadecimal plugin UIDs to load in an internal session",
//        OFFSET(qsv.load_plugins), AV_OPT_TYPE_STRING, { .str = "" }, 0, 0, VD },
//    { NULL },
//};

static const AVClass hevc_class = {"kk_hevc_qsv",av_default_item_name,NULL,LIBAVUTIL_VERSION_INT};

AVCodec kk_hevc_qsv_decoder = {
    "kk_hevc_qsv",
    "HEVC (Intel Quick Sync Video acceleration)",
     AVMEDIA_TYPE_VIDEO,
     AV_CODEC_ID_HEVC,
	 AV_CODEC_CAP_DELAY | AV_CODEC_CAP_DR1,
	 0,
     pix_fmts,
	 0,0,0,0,
	 &hevc_class,
	 0,
	 sizeof(KKQSVH2645Context),
	 0,0,0,0,0,
     qsv_decode_init,
	 0,
	 0,
	 qsv_decode_frame,
	 qsv_decode_close,
	 0,0,0,0,
     qsv_decode_flush,
	 0
};
void Registerkk_h265_qsv_decoder()
{
   avcodec_register(&kk_hevc_qsv_decoder);
}

#endif


//static const AVOption options[] = {
//    { "async_depth", "Internal parallelization depth, the higher the value the higher the latency.", OFFSET(qsv.async_depth), AV_OPT_TYPE_INT, { ASYNC_DEPTH_DEFAULT }, 0, INT_MAX, VD },
//    { NULL },
//};
static const AVClass _class = {"kk_h264_qsv",av_default_item_name,NULL,LIBAVUTIL_VERSION_INT};

AVCodec kk_h264_qsv_decoder = { "kk_h264_qsv","H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10 (Intel Quick Sync Video acceleration)",
     AVMEDIA_TYPE_VIDEO,AV_CODEC_ID_H264,AV_CODEC_CAP_DELAY | AV_CODEC_CAP_DR1,0,pix_fmts,
	 0,0,0,0,&_class,0,sizeof(KKQSVH2645Context),0,0,0,0,0,
     qsv_decode_init,0,0,qsv_decode_frame,qsv_decode_close,0,0,0,0,
     qsv_decode_flush,0};

void Registerkk_h264_qsv_decoder()
{
   avcodec_register(&kk_h264_qsv_decoder);
}

