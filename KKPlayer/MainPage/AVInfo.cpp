#include "AVInfo.h"
#include "MainDlg.h"
extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CAVInfo::CAVInfo(): SHostWnd(_T("LAYOUT:XML_AVINFO"))
	{
           m_bMini=0;
	}
	CAVInfo::~CAVInfo()
	{
        
	}
	void CAVInfo::OnLButtonDown(UINT nFlags, CPoint point)
	{
		
		 this->OnMouseEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));
		
         m_pDlgMain->ShowMiniUI(false);
	}

	void CAVInfo::OnClose()
	{
		ShowWindow(SW_HIDE);
	}
	void CAVInfo::OnMinimize()
	{
		
	}
}