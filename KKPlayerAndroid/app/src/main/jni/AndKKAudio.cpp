//
// Created by saint on 2016/3/10.
//

#include <malloc.h>
#include <string.h>
#include "AndKKAudio.h"
CAndKKAudio::CAndKKAudio()
{
    m_nBufLength=1024*4;
    m_pBuf=::malloc( m_nBufLength);
    memset(m_pBuf,0,m_nBufLength);


}
CAndKKAudio::~CAndKKAudio()
{

}
void CAndKKAudio::SetWindowHAND(int hwnd)
{
    m_hwnd=hwnd;
}
void CAndKKAudio::SetUserData(void* UserData)
{
   m_UserData=UserData;
}
/********设置音频回到函数*********/
void CAndKKAudio::SetAudioCallBack(pfun fun)
{
   m_pFun=fun;
}
/***********初始化音频设备*********/
void CAndKKAudio::InitAudio()
{

}
/*******读取音频数据********/
void CAndKKAudio::ReadAudio()
{
    if(m_pFun!=NULL)
    {

        //memset(buf,0,buf_len);
        m_pFun(m_UserData,(char*)m_pBuf,m_nBufLength);
        //RaiseVolume((char*)buf, buf_len, 1, 5);
    }
}
void CAndKKAudio::Start()
{

}
void CAndKKAudio::Stop()
{

}
/*********关闭**********/
void CAndKKAudio::CloseAudio()
{

}
/*********设置音量************/
void CAndKKAudio::SetVolume(long value)
{

}
long CAndKKAudio::GetVolume()
{
    return 0;
}