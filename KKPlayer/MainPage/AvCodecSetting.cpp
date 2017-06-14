#include "AvCodecSetting.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
namespace SOUI
{
	CAvCodecSetting::CAvCodecSetting(): SDomodalwnd(_T("LAYOUT:XML_AvCodecSetting"))
	{
           
	}
	CAvCodecSetting::~CAvCodecSetting()
	{
        
	}
	void CAvCodecSetting::Init()
	{
	    
	}
	
	void CAvCodecSetting::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAvCodecSetting::OnMinimize()
	{
		
	}
}