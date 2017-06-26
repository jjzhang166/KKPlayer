#include "AvStreamSetting.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"
#include <helper/SAdapterBase.h>
#include <control/SComboView.h>
#include "KKPlayer.h"
namespace SOUI
{

	

	CAvStreamSetting::CAvStreamSetting(): SDomodalwnd(_T("LAYOUT:XML_AvStreamSetting"))
	{
           
	}
	CAvStreamSetting::~CAvStreamSetting()
	{
        
	}
	void CAvStreamSetting::Init()
	{

	}
	
	void CAvStreamSetting::OnUseLibRtmp()
	{
		 bool nUseLibRtmp=false;
	     SCheckBox *pCheckBox=( SCheckBox *)this->FindChildByName("chk_UseLibRtmp");
		 SWindow* pWin=( SCheckBox *)this->FindChildByName("librtmpMaxDelay");
		 if(pCheckBox){
			 if(pCheckBox->IsChecked()){
				  pWin->SetVisible(1,1);
				  nUseLibRtmp=true;
			 }else{
			      pWin->SetVisible(0,1);
				  nUseLibRtmp=false;
			 } 
		 }

		 CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
		 if(nUseLibRtmp)
		    InfoMgr->UpdataConfig("UseLibRtmp","1");
		 else
			InfoMgr->UpdataConfig("UseLibRtmp","0");

	}
	void CAvStreamSetting::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAvStreamSetting::OnMinimize()
	{
		
	}
}