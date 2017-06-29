#include "AVMiniTittle.h"
#include "MainDlg.h"

namespace SOUI
{
	CAVMiniTittle::CAVMiniTittle( CMainDlg* pDlgMain): SHostWnd(_T("LAYOUT:XML_MINI_TITTLE")), m_pDlgMain(pDlgMain)
	{

	}
	CAVMiniTittle::~CAVMiniTittle()
	{

	}
	void CAVMiniTittle::OnLButtonDown(UINT nFlags, CPoint point)
	{
		
		 this->OnMouseEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));
		
         m_pDlgMain->ShowMiniUI(false);
	}

	void CAVMiniTittle::OnClose()
	{
		m_pDlgMain->OnClose();
	}
	void CAVMiniTittle::OnMinimize()
	{
		m_pDlgMain->OnMinimize();
	}
	//OnLButtonDown
	//
}