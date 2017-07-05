
#ifndef AVTransferDlg_H_
#define AVTransferDlg_H_
#include "../stdafx.h"
#include "../../KKPlayerCore/KKPlayer.h"
#include "SDomodalwnd.h"
namespace SOUI
{
	class CMainDlg;
	class CAVTransferDlg: public SDomodalwnd
	{
	   public:
		       CAVTransferDlg(CMainDlg *m_DlgMain);
		       ~CAVTransferDlg();
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
				BEGIN_MSG_MAP_EX(CAVTransferDlg)
					CHAIN_MSG_MAP(SDomodalwnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	private:			
              CMainDlg *m_pDlgMain;

	};
}
#endif