#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include "IKKAudio.h"
#include "KKLock.h"

#ifndef KKSound_H_
#define KKSound_H_


class CKKSound: public IKKAudio
{
   public:
	       CKKSound(void);
	       ~CKKSound(void);

		   int InitAudio();
		   void SetWindowHAND(int m_hwnd);
		   void SetUserData(void* UserData);
		   /********设置音频回到函数*********/
		   void SetAudioCallBack(pfun fun);
		   //需要在线程中单独调用
		   void ReadAudio();
		   void CloseAudio();
           void SetVolume(long value);
		   long GetVolume();
		   void Start();   
		   void Stop();
   private:
	       static unsigned __stdcall KKAudiothread(LPVOID lpParameter);
	       DWORD res;
		   LPVOID buf; 
		   DWORD  buf_len;  
		   DWORD offset;
	       LPDIRECTSOUND8 lpDirectSound;
		   IDirectSoundBuffer *ppDSBuffer;
		   IDirectSoundBuffer8 *m_pDSBuffer8; //used to manage sound buffers.  
		   IDirectSoundNotify8 *m_pDSNotify;
           /**********回调函数***********/
		   pfun m_pFun;
		   /**********用户数据***********/
		   void* m_UserData;
		   bool IsClose;
		   CKKLock m_Lock;
		   volatile bool m_Stop;
		   HANDLE m_ReadAudioH;
		   unsigned m_ReadAddr;
};
#endif
