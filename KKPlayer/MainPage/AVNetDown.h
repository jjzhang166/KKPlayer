#include "../stdafx.h"
#include "../../KKPlayerCore/KKPlayer.h"
#ifndef CAVNetDown_H
#define CAVNetDown_H
namespace SOUI
{
	class CMainDlg;
	class CAVNetDown: public SHostWnd
	{
	   public:
		      CAVNetDown(CMainDlg *m_DlgMain);
		       ~CAVNetDown();
			    void OnLButtonDown(UINT nFlags, CPoint point);

				void OnClose();
				void OnMinimize();
				void OnOpenUrl();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btnOk",OnOpenUrl)
					EVENT_NAME_COMMAND(L"btnCancell",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAVNetDown)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SHostWnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	private:			
              CMainDlg *m_pDlgMain;

	};
}
#endif