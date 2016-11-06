#include "com_ic70_kkplayer_kkplayer_CJniKKPlayer.h"
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
    if(G_pKKUI==NULL)
        G_pKKUI = new CAndKKPlayerUI();
    int Ret=(int)G_pKKUI;
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
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_DelKK(JNIEnv *env, jobject instance,jint obj)
{
    if(G_pKKUI!=NULL)
    {
       CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
       delete pKKUI;
        G_pKKUI=NULL;
        LOGE("DelKK\n");
    }
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKOpenMedia(JNIEnv *env, jobject instance, jstring str_,jint obj)
{
    //char str[] ="rtmp://121.42.14.63/live/livestream";
    const char *str=env->GetStringUTFChars(str_, 0);

    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    int l= pKKUI->OpenMedia((char*)str);
    env->ReleaseStringUTFChars(str_, str);
    return l;
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKIsNeedReConnect(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    if(pKKUI->GetNeedReconnect())
        return 1;
    return 0;
}
JNIEXPORT jstring JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_GetMediaInfo(JNIEnv *env, jobject instance, jint obj) {
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    MEDIA_INFO info=pKKUI->GetMediaInfo();
    char s[256]="";
    sprintf(s,"%d;%d",info.CurTime,info.TotalTime);
    return env->NewStringUTF(s);
}
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_Pause(JNIEnv *env, jobject instance, jint obj) {
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->Pause();
}
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_Seek(JNIEnv *env, jobject instance, jint obj, jint value)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->Seek(value);
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKCloseMedia(JNIEnv *env,jobject instance, jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->CloseMedia();
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKGetPlayerState(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetPlayerState();
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKIsReady(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetIsReady();
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKGetRealtime(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetRealtime();
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKGetRealtimeDelay(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetRealtimeDelay();
}
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKForceFlushQue(JNIEnv *env, jobject instance,jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->ForceFlushQue();
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKSetMinRealtimeDelay(JNIEnv *env, jobject instance,jint obj, jint value)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->SetMinRealtimeDelay(value);
}