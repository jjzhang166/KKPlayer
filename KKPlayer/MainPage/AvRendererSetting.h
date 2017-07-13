/**
* ”∆µ–≈œ¢
*/
#ifndef AvRendererSetting_H_
#define AvRendererSetting_H_
#include "../stdafx.h"
#include "SDomodalwnd.h"
namespace SOUI
{
	class CAvRendererSetting: public SDomodalwnd
	{
	   public:
		       CAvRendererSetting();
		       ~CAvRendererSetting();
	protected:
		        void Init();
			   
				void OnClose();
				void OnMinimize();
				EVENT_MAP_BEGIN()
					EVENT_NAME_COMMAND(L"btn_close",OnClose)
					EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAvRendererSetting)
					CHAIN_MSG_MAP(SDomodalwnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/
	};
}
#endif