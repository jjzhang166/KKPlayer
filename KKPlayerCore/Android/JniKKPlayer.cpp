#include "JniKKPlayer.h"
#include <jni.h>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <malloc.h>

#include "AndKKPlayerUI.h"
#include <map>
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
void* ConvertThread(void *param)
{

}
/***********初始一个KKUI**********/
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_IniKK(JNIEnv *pEv, jobject p, jint Render)
{
    CAndKKPlayerUI *pKKUI = new CAndKKPlayerUI(Render);
    int Ret=(int)pKKUI;
    return Ret;
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_IniGl(JNIEnv *env, jobject instance, jint obj)
{
    CAndKKPlayerUI *pKKUI = (CAndKKPlayerUI *)obj;
    return pKKUI->Init();
}



JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_OnSize(JNIEnv *env, jobject instance, jint obj,jint w, jint h) {
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *)obj;
    return pKKUI->OnSize(w,h);
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_GlRender(JNIEnv *env, jobject instance, jint obj) {
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *)obj;
    pKKUI->renderFrame(0);
    return 1;
}


JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_SurfaceRender(JNIEnv * env, jobject instance, jint obj, jobject surface)
{
	CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *)obj;
	
	LOGE("ANativeWindow_fromSurface ");
	ANativeWindow * mANativeWindow = ANativeWindow_fromSurface(env, surface);
	LOGE("Surface %d ",mANativeWindow);
	if(mANativeWindow !=NULL){
	
	pKKUI->renderFrame(mANativeWindow);
	ANativeWindow_release(mANativeWindow);
	LOGE("ANativeWindow_release ");
	}
    return 1;
}

JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_DelKK(JNIEnv *env, jobject instance,jint obj)
{
    if(obj!=0){
       CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
       delete pKKUI;
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
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    if(pKKUI->GetNeedReconnect())
        return 1;
	}
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
	if(obj!=0){
       CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
       pKKUI->Pause();
	}
}
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_Seek(JNIEnv *env, jobject instance, jint obj, jint value)
{
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->Seek(value);
	}
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKCloseMedia(JNIEnv *env,jobject instance, jint obj)
{
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->CloseMedia();
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKGetPlayerState(JNIEnv *env, jobject instance,jint obj)
{
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetPlayerState();
	}
	return -1;
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKIsReady(JNIEnv *env, jobject instance,jint obj)
{
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetIsReady();
	}
	return -1;
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKGetRealtime(JNIEnv *env, jobject instance,jint obj)
{
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetRealtime();
	}
	return -1;
}
JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKGetRealtimeDelay(JNIEnv *env, jobject instance,jint obj)
{
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    return  pKKUI->GetRealtimeDelay();
	}
	return -1;
}
JNIEXPORT void JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKForceFlushQue(JNIEnv *env, jobject instance,jint obj)
{
	if(obj!=0){
        CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
        pKKUI->ForceFlushQue();
	}
	
}

JNIEXPORT jint JNICALL Java_com_ic70_kkplayer_kkplayer_CJniKKPlayer_KKSetMinRealtimeDelay(JNIEnv *env, jobject instance,jint obj, jint value)
{
	if(obj!=0){
    CAndKKPlayerUI *pKKUI=(CAndKKPlayerUI *) obj;
    pKKUI->SetMinRealtimeDelay(value);
	}
	return -1;
}


///Java虚拟机变量
JavaVM *        kk_Gjava_vm=NULL;
pthread_mutex_t kk_Gvm_lock = PTHREAD_MUTEX_INITIALIZER;

/**
* JNI 库模块加载事件处理接口。可以做一些函数的加载
*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM * java_vm, void * reserved)
{
	kk_Gjava_vm=java_vm;
	return JNI_VERSION_1_6;
}
/**
* JNI 库模块卸载事件处理接口。
*/
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM * java_vm, void * reserved)
{
	kk_Gjava_vm=0;
}


///线程附加到vm中
void* kk_jni_attach_env()
{
    int ret = 0;
    JNIEnv *env = NULL;

    pthread_mutex_lock(&kk_Gvm_lock);
   
    JavaVM* java_vm=kk_Gjava_vm;
    if (!java_vm) {
         LOGE("No Java virtual machine has been registered\n");
         return NULL;
    }

    ret = java_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    switch(ret)
	{
		case JNI_EDETACHED:
			if (java_vm->AttachCurrentThread(&env, NULL) != 0) {
				LOGE("Failed to attach the JNI environment to the current thread\n");
				env = NULL;
			}
			break;
		case JNI_OK:
			break;
		case JNI_EVERSION:
			LOGE("The specified JNI version is not supported\n");
			break;
		default:
			LOGE("Failed to get the JNI environment attached to this thread \n");
			break;
    }

	if(env!=NULL){
		LOGE("attached to this thread Ok \n");
	}
	 pthread_mutex_unlock(&kk_Gvm_lock);
    return env;
}
int kk_jni_detach_env()
{
	pthread_mutex_lock(&kk_Gvm_lock);
	JavaVM* java_vm=kk_Gjava_vm;
    if (java_vm == NULL) {
        LOGE("No Java virtual machine has been registered\n");
		pthread_mutex_unlock(&kk_Gvm_lock);
        return -1;
    }

    int ret=java_vm->DetachCurrentThread();
	pthread_mutex_unlock(&kk_Gvm_lock);
	LOGE("detached to this thread Ok \n");
	return ret;
}
