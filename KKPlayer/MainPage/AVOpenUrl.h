#ifndef AVOpenUrl_H
#define AVOpenUrl_H
#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#include "../../KKPlayerCore/KKPlayer.h"
#include "SDomodalwnd.h"
namespace SOUI
{
	class CAVOpenUrl: public SDomodalwnd
	{
	   public:
		      CAVOpenUrl();
		       ~CAVOpenUrl();
	   protected:
			    void OnLButtonDown(UINT nFlags, CPoint point);
				void OnClose();
				void OnOpenUrl();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btnOk",OnOpenUrl)
					EVENT_NAME_COMMAND(L"btnCancell",OnClose)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAVOpenUrl)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SDomodalwnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	};
}
#endif