#include "AVMaxBottom.h"
#include "MainDlg.h"

namespace SOUI
{
	CAVMaxBottom::CAVMaxBottom(CMainDlg *pDlgMain): SHostWnd(_T("LAYOUT:XML_MaxButton")),m_pDlgMain(pDlgMain)
	{

	}
	CAVMaxBottom::~CAVMaxBottom()
	{

	}

	void CAVMaxBottom::SetPlayState(bool s)
	{
		SImageButton* pPlay=(SImageButton*)FindChildByName(L"AVPlayBtn");
		if(s){
		   pPlay->SetAttribute(L"skin",L"_skin.PLAY30",TRUE);
		}else{
		   pPlay->SetAttribute(L"skin",L"_skin.Pause30",TRUE);
		}
	}
	void CAVMaxBottom::SetVolume(int volume)
	{
	    SSliderBar *VolBar=(SSliderBar *)FindChildByName(L"AvAudio");
		VolBar->SetValue(volume);
	}
	bool CAVMaxBottom::OnSliderAudio(EventArgs *pEvt)
	{
	    EventSliderPos *pEvt2 = sobj_cast<EventSliderPos>(pEvt);
		SASSERT(pEvt2);
		long vol=pEvt2->nPos;
		
        m_pDlgMain->SetVolume(vol,true);
		return true;
	}
	 void CAVMaxBottom::OnMuteAudio()
	 {
	     m_pDlgMain->OnMuteAudio();
	 }
	void CAVMaxBottom::OnAVPlay()
	{
	     m_pDlgMain->OnAVPlay();
	}
	bool CAVMaxBottom::OnSliderVideo(EventArgs *pEvt)
	{
		m_pDlgMain->OnSliderVideo(pEvt);
		return true;
	}
}