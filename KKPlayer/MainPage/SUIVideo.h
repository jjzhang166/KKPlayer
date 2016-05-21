#include "../stdafx.h"
#include <core/swnd.h>
#include "../MainFrm.h"
#include "../../KKPlayerCore/IKKPlayUI.h"
#include <string>
#ifndef SUIVIDEO_H_
#define SUIVIDEO_H_
namespace SOUI
{
      class CSuiVideo: public SWindow
	  {
		  SOUI_CLASS_NAME( CSuiVideo, L"SuiVideo")
	  public:
		  CSuiVideo(void);
		  ~CSuiVideo(void);
		  int OpenMeida(const char *str);
		  void SetPlayStat(int state);
		  MEDIA_INFO GetMeadiaInfo();
		  void SetVolume(long value);
		  void AvSeek(int value);
		  int PktSerial();

		   void OnDecelerate();
		   void OnAccelerate();
		   //获取放播的历史信息
		   void GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue);
	  protected:
		  CMainFrame m_VideoWnd;
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
	  };
}
#endif