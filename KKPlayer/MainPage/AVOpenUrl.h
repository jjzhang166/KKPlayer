#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#include "../../KKPlayerCore/KKPlayer.h"
#ifndef AVOpenUrl_H
#define AVOpenUrl_H
namespace SOUI
{
	class CAVOpenUrl: public SHostWnd,public CWHRoundRectFrameHelper<CAVOpenUrl>
	{
	   public:
		      CAVOpenUrl();
		       ~CAVOpenUrl();
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
				BEGIN_MSG_MAP_EX(CAVOpenUrl)
				    CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CAVOpenUrl>)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SHostWnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	};
}
#endif