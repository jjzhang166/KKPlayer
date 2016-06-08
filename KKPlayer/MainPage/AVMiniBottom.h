#include "../stdafx.h"
#ifndef AVMiniBottom_H
#define AVMiniBottom_H
namespace SOUI
{
	class CAVMiniBottom: public SHostWnd
	{
	   public:
		       CAVMiniBottom();
		       ~CAVMiniBottom();
			   

			   bool OnSliderVideo(EventArgs *pEvtBase);
				EVENT_MAP_BEGIN()
				        EVENT_NAME_HANDLER(L"slider_video_Seek",EventSliderPos::EventID,OnSliderVideo)
				EVENT_MAP_END()	
				BEGIN_MSG_MAP_EX(CAVMiniBottom)
					
					CHAIN_MSG_MAP(SHostWnd)
					REFLECT_NOTIFICATIONS_EX()
				END_MSG_MAP()/**/

	};
}
#endif