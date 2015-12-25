// MangageTrade.cpp : main source file for MangageTrade.exe
//

#include "stdafx.h"

#include "resource.h"
#include "MainFrm.h"
#include <GdiPlus.h>
#include <Gdiplusinit.h>
#pragma comment (lib,"Gdiplus.lib")
CAppModule _Module;
std::basic_string<TCHAR> g_strModuleFileName;
const std::basic_string<TCHAR>& XGetModuleFilename()
{
	
	if (g_strModuleFileName.empty())
	{
		if(g_strModuleFileName.empty())
		{
			TCHAR filename[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, filename, _countof(filename));
			g_strModuleFileName = filename;
		}
	}
	return g_strModuleFileName;
}
std::basic_string<TCHAR> GetModulePath()
{
	std::basic_string<TCHAR> strModuleFileName = XGetModuleFilename();
	unsigned int index = strModuleFileName.find_last_of(_T("\\"));
	if (index != std::string::npos)
	{
		return strModuleFileName.substr(0, index);
	}
	return _T("");
}

class CAVMainFrame : 
	public CFrameWindowImpl<CAVMainFrame>, 
	public CUpdateUI<CAVMainFrame>,
	public CMessageFilter
{
public:
	CAVMainFrame(){};
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	    BEGIN_UPDATE_UI_MAP(CAVMainFrame)
	    END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CAVMainFrame)
	END_MSG_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg)
		{
			if(CFrameWindowImpl<CAVMainFrame>::PreTranslateMessage(pMsg))
				return TRUE;
			return FALSE;
		}
	virtual BOOL OnIdle()
	{
           return TRUE;
	}
};
HWND G_Parent=NULL;
CMainFrame *pWnd;
int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame GWndMain;
	std::string strErr;
//WS_CHILD |WS_VISIBLE| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
	RECT rt={0,100,200,300};
	if(GWndMain.CreateEx(G_Parent,NULL, WS_OVERLAPPEDWINDOW| WS_CLIPSIBLINGS|WS_CLIPCHILDREN) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
    pWnd=&GWndMain;
	
	    GWndMain.ShowWindow(nCmdShow);
    
	int nRet = theLoop.Run();
    _Module.RemoveMessageLoop();
	
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
    G_Parent =(HWND)_wtoi(lpstrCmdLine);
	wchar_t abcd[100]=L"";
	wsprintf(abcd,L"%d",G_Parent);
	//MessageBox(NULL,abcd,L"XX",0);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	//HWND h=CreateMessageWindow(hInstance);
	
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();
	Gdiplus::GdiplusStartupInput StartupInput; 
	ULONG_PTR m_gdiplusToken;
	GdiplusStartup(&m_gdiplusToken,&StartupInput,NULL); 
	
	int nRet = Run(lpstrCmdLine, nCmdShow);

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	_Module.Term();
	::CoUninitialize();

	return nRet;
}
