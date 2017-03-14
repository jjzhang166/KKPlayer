/**
* ”∆µ–≈œ¢
*/
#ifndef AVInfo_H
#define AVInfo_H
#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#include "../../KKPlayerCore/KKPlayer.h"
#include "SDomodalwnd.h"
namespace SOUI
{
	class CAVInfo: public SDomodalwnd
	{
	   public:
		       CAVInfo(MEDIA_INFO Info);
		       ~CAVInfo();
	protected:
		        void Init();
			    void OnLButtonDown(UINT nFlags, CPoint point);
				void SetAVInfo(MEDIA_INFO &Info);
				void OnClose();
				void OnMinimize();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAVInfo)
					MSG_WM_LBUTTONDOWN(OnLButtonDown)
					CHAIN_MSG_MAP(SDomodalwnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/
	private:
		   MEDIA_INFO m_Info;

	};
}
#endif