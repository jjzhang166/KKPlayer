#include "AVNetDown.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"


namespace SOUI
{
	CAVNetDown::CAVNetDown(CMainDlg *pDlgMain): SHostWnd(_T("LAYOUT:XML_AVNETDOWN")),m_pDlgMain(pDlgMain)
	{
        
	}
	CAVNetDown::~CAVNetDown()
	{
        
	}
	
	void CAVNetDown::OnLButtonDown(UINT nFlags, CPoint point)
	{
		
		 this->OnMouseEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));
		
         m_pDlgMain->ShowMiniUI(false);
	}

	void CAVNetDown::OnClose()
	{
		ShowWindow(SW_HIDE);
	}
	void CAVNetDown::OnOpenUrl()
	{
		SOUI::SStringW url=FindChildByName("EditKKV")->GetWindowText();
		if(url.GetLength()<2)
		{
			::MessageBox(m_hWnd,L"请输入有效的URL地址",L"提示",MB_ICONHAND);
		}else
		{
			char urlx[1024];
			CChineseCode::wcharTochar(url.GetBuffer(1024),urlx,1024);
			url.ReleaseBuffer();
            m_pDlgMain->DownMedia(urlx);
            ShowWindow(SW_HIDE);
		}
	}
	void CAVNetDown::OnMinimize()
	{
		
	}
}