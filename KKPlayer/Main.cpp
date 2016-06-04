// MangageTrade.cpp : main source file for MangageTrade.exe
//

#include "stdafx.h"

#include "resource.h"
#include "MainFrm.h"
#include <GdiPlus.h>
#include <Gdiplusinit.h>
#include "MainPage/MainDlg.h"
#include "MainPage/SUIVideo.h"
#pragma comment (lib,"Gdiplus.lib")
CAppModule _Module;
std::basic_string<TCHAR> g_strModuleFileName;
SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
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

//	CMainFrame GWndMain;
//	std::string strErr;
////WS_CHILD |WS_VISIBLE| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
//	RECT rt={0,100,200,300};
//	if(GWndMain.CreateEx(G_Parent,NULL, WS_OVERLAPPEDWINDOW| WS_CLIPSIBLINGS|WS_CLIPCHILDREN) == NULL)
//	{
//		ATLTRACE(_T("Main window creation failed!\n"));
//		return 0;
//	}
//    pWnd=&GWndMain;
//	
//	    GWndMain.ShowWindow(nCmdShow);
    
	int nRet = theLoop.Run();
    _Module.RemoveMessageLoop();
	
	return nRet;
}

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif
SOUI::CMainDlg *m_pDlgMain=NULL;

void skpngZhuc();
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{

	skpngZhuc();

	int ll=sizeof(ULONG_PTR);
	HRESULT hRes = ::CoInitialize(NULL);
    G_Parent =(HWND)_wtoi(lpstrCmdLine);
	wchar_t abcd[100]=L"";
	wsprintf(abcd,L"%d",G_Parent);
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


	using namespace SOUI;
	SComMgr * pComMgr = new SComMgr;

	SOUI::CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	
	pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
    
	
	pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
	
	SApplication *theApp=new SApplication(pRenderFactory,hInstance);
    theApp->RegisterWndFactory(TplSWindowFactory<CSuiVideo>());

	CAutoRefPtr<ITranslatorMgr> trans;
	pComMgr->CreateTranslator((IObjRef**)&trans);
	if(trans)
	{
	
	}


	CAutoRefPtr<IResProvider>   pResProvider;
	CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
	BOOL ret=pResProvider->Init((WPARAM)hInstance,0);


	theApp->AddResProvider(pResProvider);

	//加载系统资源
	HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
	if(hSysResource)
	{
		CAutoRefPtr<IResProvider> sysSesProvider;
		CreateResProvider(RES_PE,(IObjRef**)&sysSesProvider);
		sysSesProvider->Init((WPARAM)hSysResource,0);
		theApp->LoadSystemNamedResource(sysSesProvider);
	}

	std::wstring path=GetModulePath();
	
	SOUI::CMainDlg dlgMain;
	m_pDlgMain=&dlgMain;
	dlgMain.Create(GetActiveWindow(),0,0,0,0);
	dlgMain.GetNative()->SendMessage(WM_INITDIALOG);
	dlgMain.CenterWindow(dlgMain.m_hWnd);
	dlgMain.ShowWindow(SW_SHOWNORMAL);

	int nRet =  nRet = theApp->Run(dlgMain.m_hWnd);// Run(lpstrCmdLine, nCmdShow);

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	_Module.Term();
	::CoUninitialize();

	return nRet;
}
