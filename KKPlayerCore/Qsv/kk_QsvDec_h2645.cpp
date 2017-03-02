
#include <stdint.h>
#include <string.h>
#include "kk_QsvDec.h"

enum LoadPlugin {
    LOAD_PLUGIN_NONE,
    LOAD_PLUGIN_HEVC_SW,
};

typedef struct QSVH2645Context {
    AVClass *_class;
    QSVContext qsv;

    int load_plugin;

    // the filter for converting to Annex B
    AVBitStreamFilterContext *bsf;

} QSVH2645Context;


static int qsv_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame, AVPacket *avpkt)
{
    
    return 0;
}

static av_cold int qsv_decode_close(AVCodecContext *avctx)
{
    

    return 0;
}

static av_cold int qsv_decode_init(AVCodecContext *avctx)
{
   return 0;
}

static void qsv_decode_flush(AVCodecContext *avctx)
{
   
}

static AVPixelFormat pix_fmts[] ={ AV_PIX_FMT_NV12,AV_PIX_FMT_QSV,AV_PIX_FMT_NONE };
static const AVClass _class = {"kk_h264_qsv",av_default_item_name,NULL,LIBAVUTIL_VERSION_INT};
AVCodec kk_h264_qsv_decoder = { "kk_h264_qsv","H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10 (Intel Quick Sync Video acceleration)",
     AVMEDIA_TYPE_VIDEO,AV_CODEC_ID_H264,AV_CODEC_CAP_DELAY | AV_CODEC_CAP_DR1,0,pix_fmts,
	 0,0,0,0,&_class,0,sizeof(QSVH2645Context),0,0,0,0,0,
     qsv_decode_init,0,0,qsv_decode_frame,qsv_decode_close,0,0,0,0,
     qsv_decode_flush,0};
