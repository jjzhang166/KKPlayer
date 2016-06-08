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

	bool CAVMiniBottom::OnSliderVideo(EventArgs *pEvt)
	{
		m_pDlgMain->OnSliderVideo(pEvt);
		return true;
	}
}