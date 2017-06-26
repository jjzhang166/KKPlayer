/**
*Stream setting
*/
#ifndef AvStreamSetting_H
#define AvStreamSetting_H
#include "../stdafx.h"
#include "../WHRoundRectFrameHelper.h"
#include "../../KKPlayerCore/KKPlayer.h"
#include "SDomodalwnd.h"
namespace SOUI
{
	class CAvStreamSetting: public SDomodalwnd
	{
	   public:
		       CAvStreamSetting();
		       ~CAvStreamSetting();
	protected:
		        void Init();
			   
				void OnClose();
				void OnMinimize();
		        void OnUseLibRtmp();

				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
					EVENT_NAME_COMMAND(L"chk_UseLibRtmp",OnUseLibRtmp)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAvStreamSetting)
				
					CHAIN_MSG_MAP(SDomodalwnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/
	};
}
#endif