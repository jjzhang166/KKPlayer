// libkkplayer.cpp : 定义 DLL 应用程序的导出函数。
//
#include "../KKPlayer/stdafx.h"
#include "../KKPlayer/MainFrm.h"
CreateRender pfnCreateRender=NULL;
DelRender pfnDelRender=NULL;


extern "C"{
	void __declspec(dllexport) *createPlayer(HWND h)
{
        RECT rt={0,100,200,300};

		CMainFrame *m_pVideoWnd = new CMainFrame() ;
		if(m_pVideoWnd->CreateEx(h,rt, WS_CHILDWINDOW| WS_CLIPCHILDREN ) == NULL)//WS_VISIBLE|| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
		{
				return 0;
		}
		return m_pVideoWnd;
}
}
