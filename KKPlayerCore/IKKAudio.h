/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#ifndef IKKAudio_H_
#define IKKAudio_H_
typedef  void (*pfun)(void *UserData,char *buf,int buflen);
/*********/
class IKKAudio
{
  public:
	      virtual void SetWindowHAND(int m_hwnd)=0;
	      virtual void SetUserData(void* UserData)=0;
	      /********设置音频回到函数*********/
	      virtual void SetAudioCallBack(pfun fun)=0;
	      virtual void InitAudio()=0;
          virtual void PlayAudio()=0;	   
		  virtual void CloseAudio()=0;	
		  virtual void SetVolume(long value)=0;
		  virtual long GetVolume()=0;
};
#endif