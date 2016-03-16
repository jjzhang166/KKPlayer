//
// Created by saint on 2016/3/10.
//
#include "F:/ProgramTool/OpenPro/KKPlayer/KKPlayerCore/IKKAudio.h"
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
    private:
             void *m_pBuf;
             int m_nBufLength;
             pfun m_pFun;
             int m_hwnd;
             void* m_UserData;
};


#endif //KKPLAYERANDROID_ANDKKAUDIO_H
