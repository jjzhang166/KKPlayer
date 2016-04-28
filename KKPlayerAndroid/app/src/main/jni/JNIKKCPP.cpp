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
#include "AndKKPlayerUI.h"
#include <map>
CAndKKPlayerUI *G_pKKUI =NULL;
void* ConvertThread(void *param)
{

}
/***********初始一个KKUI**********/
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_IniKK(JNIEnv *pEv, jobject p)
{
    
    CAndKKPlayerUI *pKKUI = new CAndKKPlayerUI();
    G_pKKUI=pKKUI;
    int Ret=(int)pKKUI;
    return Ret;
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_IniGl(JNIEnv *env, jobject instance, jint obj)
{
    CAndKKPlayerUI *pKKUI = (CAndKKPlayerUI *)obj;
    return pKKUI->IniGl();
}


JNIEXPORT jint JNICALL
Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_Resizeint(JNIEnv *env, jobject instance, jint obj,
                                                       jint w, jint h) {
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *)obj;
    return pKKUI->Resizeint(w,h);
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_GlRender(JNIEnv *env, jobject instance, jint obj) {
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *)obj;
    pKKUI->renderFrame();
    return 1;
}
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_DelKK(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    delete pKKUI;
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKOpenMedia(JNIEnv *env, jobject instance, jstring str_,jint obj)
{
    const char *str = env->GetStringUTFChars(str_, 0);

    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    int l= pKKUI->OpenMedia((char*)str);
    env->ReleaseStringUTFChars(str_, str);
    return l;
}
