#include "../stdafx.h"
#ifndef AVMiniTittle_H
#define AVMiniTittle_H
namespace SOUI
{
	class  CMainDlg ;
	class CAVMiniTittle: public SHostWnd
	{
	   public:
		       CAVMiniTittle( CMainDlg* pDlgMain);
		       ~CAVMiniTittle();
			    void OnLButtonDown(UINT nFlags, CPoint point);

				void OnClose();
				void OnMinimize();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAVMiniTittle)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SHostWnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/
	private:
				 CMainDlg*  m_pDlgMain;

	};
}
#endif