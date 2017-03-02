#ifndef kk_QsvDecH_
#define kk_QsvDecH_
#include "../Includeffmpeg.h"
//typedef unsigned char	Uint8;
extern "C"{
    #include <mfx/mfxvideo.h>
	#include "libavutil/common.h"
	#include "libavutil/fifo.h"
	#include "libavutil/opt.h"
	#include <libavcodec/qsv.h>
}

typedef struct QSVContext {
    // the session used for decoding
    mfxSession session;

    AVFifoBuffer *async_fifo;
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
} QSVContext;
#endif