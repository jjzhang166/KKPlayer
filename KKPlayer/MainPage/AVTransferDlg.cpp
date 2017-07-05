#include "AVTransferDlg.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"


namespace SOUI
{
	CAVTransferDlg::CAVTransferDlg(CMainDlg *pDlgMain): SDomodalwnd(_T("LAYOUT:XML_AVDownManage"),0),m_pDlgMain(pDlgMain)
	{
        
	}
	CAVTransferDlg::~CAVTransferDlg()
	{
        
	}
	
	

	void CAVTransferDlg::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAVTransferDlg::OnOpenUrl()
	{
		
	}
	void CAVTransferDlg::OnMinimize()
	{
		
	}
}