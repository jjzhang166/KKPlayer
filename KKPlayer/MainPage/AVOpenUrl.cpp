#include "AVOpenUrl.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"

extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CAVOpenUrl::CAVOpenUrl(): SDomodalwnd(_T("LAYOUT:XML_AVOPENURL"),false)
	{
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
			this->PostMessage(WM_CLOSE,0,0);
	}
	void CAVOpenUrl::OnOpenUrl()
	{
		SOUI::SStringW url=FindChildByName("AVInfoUrl")->GetWindowText();
        url=url.TrimRight();
		url=url.TrimLeft();
		if(url.GetLength()<2)
		{
			::MessageBox(m_hWnd,L"请输入有效的URL地址",L"提示",MB_ICONHAND);
		}else{
			int len=url.GetLength()+100;
			char *urlx=(char*)::malloc(len);
			memset(urlx,0,len);
			CChineseCode::wcharTochar(url.GetBuffer(len),urlx,len);
			url.ReleaseBuffer();
            m_pDlgMain->OpenMedia(urlx);
			free(urlx);
            OnClose();
		}
	}
}