#include "AvStreamSetting.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"
#include <helper/SAdapterBase.h>
#include <control/SComboView.h>
#include "KKPlayer.h"
namespace SOUI
{

	

	CAvStreamSetting::CAvStreamSetting(): SDomodalwnd(_T("LAYOUT:XML_AvStreamSetting"),0)
	{
           
	}
	CAvStreamSetting::~CAvStreamSetting()
	{
        
	}
	void CAvStreamSetting::Init()
	{

		 CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
	 
		
		
		 int UserLibRtmp= InfoMgr->GetUseLibRtmp();
		 
		 int Rtmpdelay= InfoMgr->GetRtmpDelay();
		  SCheckBox *pCheckBox=( SCheckBox *)this->FindChildByName("chk_UseLibRtmp");
		  if(UserLibRtmp==1){
		     
			  pCheckBox->SetCheck(1);
		  }else{
		      pCheckBox-> SetCheck(0);
		  }
		  SStringT sfo;
		  sfo.Format(L"%d",Rtmpdelay);
		  SRichEdit *pEdt_MaxDelay=(SRichEdit*)this->FindChildByName("edt_MaxDelay");
	      pEdt_MaxDelay->SetWindowText(sfo);

		
	/*   char temp[256]="";
	   sprintf(temp,"%d",Delvalue);
	   InfoMgr->UpdataConfig("Rtmpdelay",temp);*/

	}
	void CAvStreamSetting::OnSave()
	{

		  SRichEdit *pEdt_MaxDelay=(SRichEdit*)this->FindChildByName("edt_MaxDelay");
		  SStringT del= pEdt_MaxDelay->GetWindowText();
		  int Delvalue=_wtoi(del);
		  if(Delvalue<1){
			SMessageBox(m_hWnd, L"rtmp延迟值必须大于0", L"警告", MB_ICONEXCLAMATION);
		    return ;
		  }

		bool nUseLibRtmp =false;
	    SCheckBox *pCheckBox=( SCheckBox *)this->FindChildByName("chk_UseLibRtmp");
		if(pCheckBox){
			 if(pCheckBox->IsChecked()){
				  nUseLibRtmp=true;
			 }else{
				  nUseLibRtmp=false;
			 } 
		}

	   CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
	   if(nUseLibRtmp)
		   InfoMgr->UpdataUseLibRtmp(1);
	   else
		   InfoMgr->UpdataUseLibRtmp(0);

	 
	   InfoMgr->UpdataRtmpDelay(Delvalue);
	    this->PostMessage(WM_CLOSE,0,0);
	}
	void CAvStreamSetting::OnUseLibRtmp()
	{
	     SCheckBox *pCheckBox=( SCheckBox *)this->FindChildByName("chk_UseLibRtmp");
		 SWindow* pWin=( SWindow*)this->FindChildByName("librtmpMaxDelay");
		 if(pCheckBox){
			 if(pCheckBox->IsChecked()){
				  pWin->SetVisible(1,1);
			 }else{
			      pWin->SetVisible(0,1);
			 } 
		 }

	}
	void CAvStreamSetting::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAvStreamSetting::OnMinimize()
	{
		
	}
}