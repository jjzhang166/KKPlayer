#include "../stdafx.h"
#include "../../KKPlayerCore/KKPlayer.h"
#ifndef AVDownManageDlg_H
#define AVDownManageDlg_H
namespace SOUI
{

	class CAVDownManageDlg: public SHostWnd//,public CWHRoundRectFrameHelper<CAVDownManageDlg>
	{
	private:
		int m_nTM_TICKER;
	public:
		CAVDownManageDlg();
		~CAVDownManageDlg();
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnClose();
		void OnMinimize();
		void OnTimer(UINT_PTR nIDEvent);
	    
		BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
		EVENT_MAP_BEGIN()
			EVENT_NAME_COMMAND(L"btn_close",OnClose)
		EVENT_MAP_END()	
		
		BEGIN_MSG_MAP_EX(CAVDownManageDlg)
		           MSG_WM_INITDIALOG(OnInitDialog)
				   MSG_WM_TIMER_EX(OnTimer) 
				   MSG_WM_LBUTTONDOWN(OnLButtonDown)
				   CHAIN_MSG_MAP(SHostWnd)
				   REFLECT_NOTIFICATIONS_EX()
	    END_MSG_MAP()

	};
}
#endif