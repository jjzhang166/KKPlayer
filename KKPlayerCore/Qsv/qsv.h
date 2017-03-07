
#ifndef KK_AVCODEC_QSV_H
#define KK_AVCODEC_QSV_H
#include "../Includeffmpeg.h"
//typedef unsigned char	Uint8;
extern "C"{
#include <mfx/mfxvideo.h>
#include "libavutil/buffer.h"
}

typedef struct KK_AVQSVContext {
   
    mfxSession                    session;
    int                           iopattern;
    mfxExtBuffer                  **ext_buffers;
    int                           nb_ext_buffers;
    int opaque_alloc;
    int nb_opaque_surfaces;

   
    int opaque_alloc_type;

	//½âÂë²ÎÊý
	mfxVideoParam param;
} KK_AVQSVContext;
KK_AVQSVContext *kk_av_qsv_alloc_context(void);

#endif
