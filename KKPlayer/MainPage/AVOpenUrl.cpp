#include "AVOpenUrl.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"

extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CAVOpenUrl::CAVOpenUrl(): SHostWnd(_T("LAYOUT:XML_AVOPENURL"))
	{
           m_bMini=0;
	}
	CAVOpenUrl::~CAVOpenUrl()
	{
        
	}
	
	void CAVOpenUrl::OnLButtonDown(UINT nFlags, CPoint point)
	{
		
		 this->OnMouseEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));
		
         m_pDlgMain->ShowMiniUI(false);
	}

	void CAVOpenUrl::OnClose()
	{
		ShowWindow(SW_HIDE);
	}
	void CAVOpenUrl::OnOpenUrl()
	{
		SOUI::SStringW url=FindChildByName("AVInfoUrl")->GetWindowText();
		if(url.GetLength()<2)
		{
			::MessageBox(m_hWnd,L"请输入有效的URL地址",L"提示",MB_ICONHAND);
		}else
		{
			char urlx[1024];
			CChineseCode::wcharTochar(url.GetBuffer(1024),urlx,1024);
			url.ReleaseBuffer();
            m_pDlgMain->OpenMedia(urlx);
           ShowWindow(SW_HIDE);
		}
	}
	void CAVOpenUrl::OnMinimize()
	{
		
	}
}