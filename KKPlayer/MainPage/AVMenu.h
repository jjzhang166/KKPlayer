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
	   private:
			CAVMenu** m_Own;
	};
}
#endif