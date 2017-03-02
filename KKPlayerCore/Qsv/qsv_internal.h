
#ifndef KK_AVCODEC_QSV_INTERNAL_H
#define KK_AVCODEC_QSV_INTERNAL_H

extern  "C"{
#include <mfx/mfxvideo.h>

#include "libavutil/frame.h"

#include "libavcodec/avcodec.h"
}


#define ASYNC_DEPTH_DEFAULT 4       // internal parallelism

#define QSV_MAX_ENC_PAYLOAD 2       // # of mfxEncodeCtrl payloads supported


typedef struct KKQSVFrame {
    AVFrame *frame;
    mfxFrameSurface1 *surface;
    mfxEncodeCtrl enc_ctrl;

    mfxFrameSurface1 surface_internal;

    int queued;

    struct KKQSVFrame *next;
} KKQSVFrame;

typedef struct KKQSVSession {
    mfxSession session;
} QSVSession;

/**
 * Convert a libmfx error code into a ffmpeg error code.
 */
int ff_qsv_error(int mfx_err);

int ff_qsv_codec_id_to_mfx(enum AVCodecID codec_id);

int ff_qsv_init_internal_session(AVCodecContext *avctx, KKQSVSession *qs,
                                 const char *load_plugins);
int ff_qsv_close_internal_session(KKQSVSession *qs);

#endif /* AVCODEC_QSV_INTERNAL_H */
