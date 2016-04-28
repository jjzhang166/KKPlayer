//
// Created by saint on 2016/3/10.
//
#include "../../../../../KKPlayerCore/IKKAudio.h"
#include "../../../../../KKPlayerCore/KKLock.h"
#include "../../../../../KKPlayerCore/KKCond_t.h"
#include <android/log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
//Android 音频实现
#ifndef KKPLAYERANDROID_ANDKKAUDIO_H
#define KKPLAYERANDROID_ANDKKAUDIO_H
class CAndKKAudio: public IKKAudio
{
   public:
             CAndKKAudio();
             ~CAndKKAudio();
              void SetWindowHAND(int hwnd);
              void SetUserData(void* UserData);
              /********设置音频回到函数*********/
              void SetAudioCallBack(pfun fun);
              /***********初始化音频设备*********/
              void InitAudio();
              /*******读取音频数据********/
              void ReadAudio();
              void Start();
              void Stop();
              /*********关闭**********/
              void CloseAudio();
             /*********设置音量************/
             void SetVolume(long value);
             long GetVolume();
             void SetCond();
    private:
             //static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
             SLObjectItf         m_engineObject;
             SLEngineItf         m_engineEngine;
             SLObjectItf         m_outputMixObject;

             bool m_nReadWait;
             // buffer queue player interfaces
             SLObjectItf     m_bqPlayerObject;
             SLPlayItf       m_bqPlayerPlay;
             SLAndroidSimpleBufferQueueItf  m_bqPlayerBufferQueue;
             SLEffectSendItf m_bqPlayerEffectSend;
             SLMuteSoloItf m_bqPlayerMuteSolo;
             SLVolumeItf m_bqPlayerVolume;
             void *m_pBuf;
             void *m_pBuf2;
             int m_nBufLength;
             pfun m_pFun;
             int m_hwnd;
             void* m_UserData;

             void       *m_pNext_buffer;
             int        m_nnext_buffer_index;
             size_t      bytes_per_buffer;
private:
            CKKLock m_ReadLock;
            CKKCond_t m_ReadLockCond;
};


#endif //KKPLAYERANDROID_ANDKKAUDIO_H
