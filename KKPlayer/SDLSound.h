#include "IKKAudio.h"
#ifndef SDLSound_H_
#define SDLSound_H_
#include "../libx86/SDL1.2.15/include/SDL.h"
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
		virtual void InitAudio();
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
		pfun m_pFun;
		 void* m_UserData;
};
#endif