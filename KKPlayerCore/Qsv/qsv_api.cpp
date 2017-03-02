
#include "../Includeffmpeg.h"

#include "qsv.h"

KK_AVQSVContext *kk_av_qsv_alloc_context(void)
{
    return (KK_AVQSVContext *)av_mallocz(sizeof(KK_AVQSVContext));
}

