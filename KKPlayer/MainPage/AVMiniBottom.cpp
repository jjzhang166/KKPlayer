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
	void CAVMiniBottom::SetVolume(int volume)
	{
	    SSliderBar *VolBar=(SSliderBar *)FindChildByName(L"AvAudio");
		VolBar->SetValue(volume);
	}
	bool CAVMiniBottom::OnSliderAudio(EventArgs *pEvt)
	{
	    EventSliderPos *pEvt2 = sobj_cast<EventSliderPos>(pEvt);
		SASSERT(pEvt2);
		long vol=pEvt2->nPos;
		
        m_pDlgMain->SetVolume(vol,true);
		return true;
	}
	 void CAVMiniBottom::OnMuteAudio()
	 {
	     m_pDlgMain->OnMuteAudio();
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