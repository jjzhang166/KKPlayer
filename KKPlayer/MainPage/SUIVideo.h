
#ifndef SUIVIDEO_H_
#define SUIVIDEO_H_
#include "../stdafx.h"
#include <core/swnd.h>
#include "../../KKPlayerCore/IKKPlayUI.h"
#include <string>
#include "../ICallKKplayer.h"
struct MEDIA_INFO ;
namespace SOUI
{
      class CSuiVideo: public SWindow
	  {
		  SOUI_CLASS_NAME( CSuiVideo, L"SuiVideo")
	  public:
			  CSuiVideo(void);
			  ~CSuiVideo(void);
			   void SetAVVisible(BOOL bVisible);
			   int  OpenMedia(const char *str,const char* avname=NULL);
			   int  DownMedia(char *KKVURL);
			   void Close();
			   void SetPlayStat(int state);
			   bool GetMeadiaInfo(MEDIA_INFO &info);
			   bool GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale=true);
			   void SaveSnapshoot();
			   void SetVolume(long value);
			   long GetVolume();
			   void AvSeek(int value);
			   int  PktSerial();

			   void OnDecelerate();
			   void OnAccelerate();
			   HWND GetPlayerHwnd();
			   void FullScreen();
	  protected:
			
			  void OnDestroy();
			  int  OnCreate(void*);
			  void OnPaint(IRenderTarget *pRT);
			  void OnSize(UINT nType, CSize size);
			  void OnMouseHover(WPARAM wParam, CPoint ptPos);
			  void OnMouseLeave();
	         
			  SOUI_MSG_MAP_BEGIN()
				 
				  MSG_WM_CREATE(OnCreate)
				  MSG_WM_DESTROY(OnDestroy)
				  MSG_WM_PAINT_EX(OnPaint)
				  MSG_WM_SIZE(OnSize)
				  MSG_WM_MOUSEHOVER(OnMouseHover)
				  MSG_WM_MOUSELEAVE(OnMouseLeave)
			 SOUI_MSG_MAP_END()
	  private:
		  std::string       m_url; 
		  ICallKKplayer* m_pVideoWnd;
	  };
}
#endif