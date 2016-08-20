#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#include "../../KKPlayerCore/KKPlayer.h"
#ifndef CAVNetDown_H
#define CAVNetDown_H
namespace SOUI
{
	class CAVNetDown: public SHostWnd,public CWHRoundRectFrameHelper<CAVNetDown>
	{
	   public:
		      CAVNetDown();
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
				    CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CAVNetDown>)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SHostWnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	};
}
#endif