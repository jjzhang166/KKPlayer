#ifndef AVMenu_H_
#define AVMenu_H_
#include "../stdafx.h"

//SMenuEx menu;
//menu.LoadMenu(_T("smenuex:menuex_test"));
//menu.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
namespace SOUI
{
	class CAVMenu :public SMenuEx
	{
		public:
			CAVMenu(CAVMenu** Own);
			~CAVMenu();

          /*  void OnKillFocus(CWindow wndFocus);
			BEGIN_MSG_MAP_EX(CMainDlg)
				MSG_WM_KILLFOCUS(OnKillFocus)
				CHAIN_MSG_MAP(SHostWnd)
				REFLECT_NOTIFICATIONS_EX()
			END_MSG_MAP()*/
	   private:
			CAVMenu** m_Own;
	};
}
#endif