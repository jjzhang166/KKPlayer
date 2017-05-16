#include "AVMiniBottom.h"
#include "MainDlg.h"
extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CAVMiniBottom::CAVMiniBottom(): SHostWnd(_T("LAYOUT:XML_MINI_BOTTOM"))
	{

	}
	CAVMiniBottom::~CAVMiniBottom()
	{

	}

	void CAVMiniBottom::SetPlayState(bool s)
	{
		SImageButton* pPlay=(SImageButton*)FindChildByName(L"AVPlayBtn");
		if(s){
		   pPlay->SetAttribute(L"skin",L"_skin.PLAY30",TRUE);
		}else{
		   pPlay->SetAttribute(L"skin",L"_skin.Pause30",TRUE);
		}
	}
	
	void CAVMiniBottom::OnAVPlay()
	{
	     m_pDlgMain->OnAVPlay();
	}
	bool CAVMiniBottom::OnSliderVideo(EventArgs *pEvt)
	{
		m_pDlgMain->OnSliderVideo(pEvt);
		return true;
	}
}