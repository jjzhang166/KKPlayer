#include "AVTransferDlg.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../Model/AvTransferAdapter.h"
#include "../Control/kkmclv.h"
namespace SOUI
{
	CAVTransferDlg::CAVTransferDlg(CMainDlg *pDlgMain): SDomodalwnd(_T("LAYOUT:XML_AVDownManage"),0),m_pDlgMain(pDlgMain)
	{
        
	}
	CAVTransferDlg::~CAVTransferDlg()
	{
		
	}
	
	
    void CAVTransferDlg::Init()
	{
	    SMCListView *KKmclv=(SMCListView*)FindChildByName(L"mclv_AVDownList");
		if(KKmclv!=NULL)
		{
			CDownAVListMcAdapterFix *Fix = new CDownAVListMcAdapterFix(KKmclv);
		    KKmclv->SetAdapter(Fix);
		    Fix->Release();
		}
		this->SetTimer(1,1000);
	}
	void CAVTransferDlg::OnTimer(char cTimerID)
	{
	 CKKmclv *KKmclv=(CKKmclv*)FindChildByName(L"mclv_AVDownList");
		if(KKmclv!=NULL)
		{
	      CDownAVListMcAdapterFix *Fix = ( CDownAVListMcAdapterFix *)KKmclv->GetAdapter();
		  if(Fix!=NULL)
		  {
		     Fix->Refresh(); 
		     KKmclv->DataSetInvalidated();
		  }
		}
	}
	 
	void CAVTransferDlg::OnClose()
	{
		this->KillTimer(1);
		this->PostMessage(WM_CLOSE,0,0);
	}
	
	void CAVTransferDlg::OnMinimize()
	{
		
	}
}