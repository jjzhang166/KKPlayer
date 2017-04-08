#include "AVMenu.h"
#include <control/SCmnCtrl.h>
#include <control/SSliderBar.h>
#include <control/SListView.h>
#include <queue>
#include <map>
extern SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
#include <helper/SAdapterBase.h>
namespace SOUI
{
	CAVMenu::CAVMenu(CAVMenu** Own)
	{
		m_Own=Own;
	} 
	CAVMenu::~CAVMenu()
	{

	}
	//void CAVMenu::OnKillFocus(CWindow wndFocus)
	//{
	//	::PostMessage(this->m_hWnd,WM_CLOSE,0,0);
	//	*m_Own=NULL;
	//}
}