#ifndef KK_AVCODEC_QSVDEC_H
#define KK_AVCODEC_QSVDEC_H
#include <stdint.h>
#include <sys/types.h>
extern "C"{
#include <mfx/mfxvideo.h>
#include "libavutil/fifo.h"
#include "libavutil/frame.h"
#include "libavutil/pixfmt.h"

#include "libavcodec/avcodec.h"
}
#include "qsv_internal.h"

typedef struct KKQSVContext {
    // the session used for decoding
    mfxSession session;
    mfxSyncPoint sync;
    // the session we allocated internally, in case the caller did not provide
    // one
    KKQSVSession internal_qs;

    /**
     * a linked list of frames currently being used by QSV
     */
    KKQSVFrame *work_frames;

    AVFifoBuffer *async_fifo;
	// ˝æ› ‰»Îª∫¥Ê
    AVFifoBuffer *input_fifo;

    // we should to buffer input packets at some cases
    // else it is not possible to handle dynamic stream changes correctly
    // this fifo uses for input packets buffering
    AVFifoBuffer *pkt_fifo;

    // this flag indicates that header parsed,
    // decoder instance created and ready to general decoding
    int engine_ready;

    // we can not just re-init decoder if different sequence header arrived
    // we should to deliver all buffered frames but we can not decode new packets
    // this time. So when reinit_pending is non-zero we flushing decoder and
    // accumulate new arrived packets into pkt_fifo
    int reinit_pending;

    // options set by the caller
    int async_depth;
    int iopattern;

    char *load_plugins;

    mfxExtBuffer **ext_buffers;
    int         nb_ext_buffers;
} KKQSVContext;

int ff_qsv_map_pixfmt(enum AVPixelFormat format);

int ff_qsv_decode(AVCodecContext *s, KKQSVContext *q,
                  AVFrame *frame, int *got_frame,
                  AVPacket *avpkt);

void ff_qsv_decode_reset(AVCodecContext *avctx, KKQSVContext *q);

int ff_qsv_decode_close(KKQSVContext *q);

#endif /* AVCODEC_QSVDEC_H */
