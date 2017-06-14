/**
* ”∆µ–≈œ¢
*/
#ifndef AvCodecSetting_H
#define AvCodecSetting_H
#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#include "../../KKPlayerCore/KKPlayer.h"
#include "SDomodalwnd.h"
namespace SOUI
{
	class CAvCodecSetting: public SDomodalwnd
	{
	   public:
		       CAvCodecSetting();
		       ~CAvCodecSetting();
	protected:
		        void Init();
			   
				void OnClose();
				void OnMinimize();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAvCodecSetting)
				
					CHAIN_MSG_MAP(SDomodalwnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/
	};
}
#endif