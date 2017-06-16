#include "IKKAudio.h"
#ifndef SDLSound_H_
#define SDLSound_H_
#include "../../SDL-1.2.15/include/SDL.h"
#include "../KKPlayerCore/KKLock.h"
class CSDLSound: public IKKAudio
{
	public:
		CSDLSound();
		~CSDLSound();
		virtual void SetWindowHAND(int m_hwnd);
		virtual void SetUserData(void* UserData);
		/********设置音频回到函数*********/
		virtual void SetAudioCallBack(pfun fun);
		/***********初始化音频设备*********/
		int OpenAudio( int &wanted_channel_layout, int &wanted_nb_channels, int &wanted_sample_rate);
		/*******读取音频数据********/
		virtual void ReadAudio();	   
		virtual void Start();	   
		virtual void Stop();	   
		/*********关闭**********/
		virtual void CloseAudio();	
		/*********设置音量************/
		virtual void SetVolume(long value);
		virtual long GetVolume();

public:
		void KKSDLCall( Uint8 *stream, int len);
private:
	    CKKLock m_lock;
		pfun    m_pFun;
		void*   m_UserData;
		long    m_Vol;
		void*   m_pSdlAudio;
};
#endif