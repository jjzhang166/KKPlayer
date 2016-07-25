#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#ifndef AVInfo_H
#define AVInfo_H
namespace SOUI
{
	class CAVInfo: public SHostWnd,public CWHRoundRectFrameHelper<CAVInfo>
	{
	   public:
		       CAVInfo();
		       ~CAVInfo();
			    void OnLButtonDown(UINT nFlags, CPoint point);

				void OnClose();
				void OnMinimize();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAVInfo)
				    CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CAVInfo>)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SHostWnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	};
}
#endif