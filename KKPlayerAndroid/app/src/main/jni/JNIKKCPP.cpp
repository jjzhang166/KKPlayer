#include "com_ic70_kkplayer_kkplayer_CJniKKPlayer.h"
#include "ffmpeg2.8.6/include/libavutil/avstring.h"
#include "ffmpeg2.8.6/include/libavutil/eval.h"
#include "ffmpeg2.8.6/include/libavutil/mathematics.h"
#include "ffmpeg2.8.6/include/libavutil/pixdesc.h"
#include "ffmpeg2.8.6/include/libavutil/imgutils.h"
#include "ffmpeg2.8.6/include/libavutil/dict.h"
#include "ffmpeg2.8.6/include/libavutil/parseutils.h"
#include "ffmpeg2.8.6/include/libavutil/samplefmt.h"
#include "ffmpeg2.8.6/include/libavutil/avassert.h"
#include "ffmpeg2.8.6/include/libavutil/time.h"
#include "ffmpeg2.8.6/include/libavformat/avformat.h"
#include "ffmpeg2.8.6/include/libavdevice/avdevice.h"
#include "ffmpeg2.8.6/include/libswscale/swscale.h"
#include "ffmpeg2.8.6/include/libavutil/opt.h"
#include "ffmpeg2.8.6/include/libavcodec/avfft.h"
#include "ffmpeg2.8.6/include/libswresample/swresample.h"
#include "ffmpeg2.8.6/include/libavfilter/avfilter.h"

# include "ffmpeg2.8.6/include/libavfilter/avcodec.h"
# include "ffmpeg2.8.6/include/libavfilter/avfilter.h"
# include "ffmpeg2.8.6/include/libavfilter/buffersink.h"
# include "ffmpeg2.8.6/include/libavfilter/buffersrc.h"
#include  "ffmpeg2.8.6/include/libavutil/mathematics.h"
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <malloc.h>

void* ConvertThread(void *param)
{

}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_IniKK
        (JNIEnv *pEv, jobject p)
{
    /*std::string aa;
    pthread_t tid_task;
    pthread_rwlock_t t;
    pthread_rwlock_init(&t,NULL);
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
   
    int ret_thread = pthread_create(&tid_task, NULL, ConvertThread, NULL);

     int i=0;
     i++;
     i=1;
     avdevice_register_all();
     av_register_all();
     avfilter_register_all();
     avformat_network_init(); */
    return ret_thread;
}

JNIEXPORT jint JNICALL
Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_IniGl(JNIEnv *env, jobject instance) {
    return 1;
}

JNIEXPORT jint JNICALL
Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_Resizeint(JNIEnv *env, jobject instance, jint gl,
                                                       jint w, jint h) {

    return 1;
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_GlRender(JNIEnv *env, jobject instance) {
    return 1;
}