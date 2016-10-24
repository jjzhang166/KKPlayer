// MangageTrade.cpp : main source file for MangageTrade.exe
//

#include "stdafx.h"

#include "resource.h"
#include "MainFrm.h"
#include <GdiPlus.h>
#include <Gdiplusinit.h>
#include "MainPage/MainDlg.h"
#include "MainPage/SUIVideo.h"
#include "MainPage/KKWkeWebkit.h"
#include "control/kkmclv.h"
#include "DownManage/AVDownManage.h"
#include "Dir/Dir.hpp"
#include "Tool/CFileMgr.h"
#include "Tool/cchinesecode.h"

#include "../KKPlayerCore/KKPlugin.h"
#include "../KKPlayerCore/KKPlayer.h"
#include "../KKPlayerCore/SqlOp/AVInfomanage.h"

#include < Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#pragma comment (lib,"Gdiplus.lib")
namespace NSDumpFile
{ 
	void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
	{  
		// 创建Dump文件  
		//  
		HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  


		// Dump信息  
		//  
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
		dumpInfo.ExceptionPointers = pException;  
		dumpInfo.ThreadId = GetCurrentThreadId();  
		dumpInfo.ClientPointers = TRUE;  


		// 写入Dump文件内容  
		//  
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  


		CloseHandle(hDumpFile);  
	}  


	LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
	{
		return NULL;
	}


	BOOL PreventSetUnhandledExceptionFilter()
	{
		HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
		if (hKernel32 ==   NULL)
			return FALSE;


		void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
		if(pOrgEntry == NULL)
			return FALSE;


		unsigned char newJump[ 100 ];
		DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
		dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far


		void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
		DWORD dwNewEntryAddr = (DWORD) pNewFunc;
		DWORD dwRelativeAddr = dwNewEntryAddr -  dwOrgEntryAddr;


		newJump[ 0 ] = 0xE9;  // JMP absolute
		memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
		SIZE_T bytesWritten;
		BOOL bRet = WriteProcessMemory(GetCurrentProcess(),    pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
		return bRet;
	}


	LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS *pException)
	{
		TCHAR szMbsFile[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szMbsFile, MAX_PATH);
		TCHAR* pFind = _tcsrchr(szMbsFile, '\\');
		if(pFind)
		{
			*(pFind+1) = 0;
			_tcscat(szMbsFile, _T("CrashDumpFile.dmp"));
			CreateDumpFile(szMbsFile, pException);
		}


		// TODO: MiniDumpWriteDump
		FatalAppExit(-1,  _T("Fatal Error"));
		return EXCEPTION_CONTINUE_SEARCH;
	}


	void RunCrashHandler()
	{
		SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
		PreventSetUnhandledExceptionFilter();
	}
};
#define DeclareDumpFile() NSDumpFile::RunCrashHandler();
CAppModule _Module;
std::basic_string<TCHAR> g_strModuleFileName;
SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
const std::basic_string<TCHAR>& XGetModuleFilename()
{
	if (g_strModuleFileName.empty()){
		if(g_strModuleFileName.empty()){
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
//WS_CHILD |WS_VISIBLE| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
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

//F:\ProgramTool\OpenPro\KKPlayer\KKPlayer>uiresbuilder.exe -iuires/uires.idx -puires -rres/KK_res.rc2
std::basic_string<char> GetModulePathA();
void skpngZhuc();



CreateRender pfnCreateRender = NULL;
DelRender pfnDelRender=NULL;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{

	int SDL_WindowStyle=WS_POPUP  | WS_VISIBLE|WS_CLIPCHILDREN;
    DeclareDumpFile();


	std::wstring Propath=GetModulePath();
	Propath+=L"\\Db";
	CFileMgr mgr;
	mgr.CreateDirectory(Propath.c_str());
	Propath+=L"\\mv";
	std::string pp;
	CChineseCode::UnicodeToUTF8((wchar_t*)Propath.c_str(),pp);

	CAVInfoManage AVInfoManage;
	AVInfoManage.SetPath((char *)pp.c_str());
	AVInfoManage.InitDb();

	HMODULE hRender = LoadLibraryA("Render.dll");
	if(hRender){
          pfnCreateRender = (CreateRender)GetProcAddress(hRender, "CreateRender");
		  pfnDelRender = (DelRender)GetProcAddress(hRender, "DelRender");
	}
	//装载
	std::string strPath= GetModulePathA();
	strPath+="\\Plugin";

	std::list<std::string> DllPathInfoList;
	dir::listFiles(DllPathInfoList,strPath,"dll");
	std::list<std::string>::iterator It=DllPathInfoList.begin();
	int Lenxx=sizeof( __KKPluginInfo);
	for (;It!=DllPathInfoList.end();++It)
	{

		//char ptl[32];
		///******创建一个插件******/
		//fCreateKKPlugin CreKKP;
		///**********删除一个插件**************/
		//fDeleteKKPlugin DelKKp;
		///***********下载文件*****************/
		//fKKDownAVFile   KKDownAVFile;
		///************停止下载*****************/
		//fKKStopDownAVFile KKStopDownAVFile;
		//fKKDownAVFileSpeedInfo KKDownAVFileSpeed;
		//fFree KKFree;

		HMODULE	hdll= LoadLibraryA((*It).c_str());
		fCreateKKPlugin pfn = (fCreateKKPlugin)GetProcAddress(hdll, "CreateKKPlugin");
		fGetPtlHeader pfGetPtl=(fGetPtlHeader)GetProcAddress(hdll, "GetPtlHeader");
		fDeleteKKPlugin pDel=(fDeleteKKPlugin)GetProcAddress(hdll, "DeleteKKPlugin");
        fKKDownAVFile pKKDownAVFile=(fKKDownAVFile)GetProcAddress(hdll, "KKDownAVFile");
		fKKStopDownAVFile pKKStopDownAVFile=(fKKStopDownAVFile)GetProcAddress(hdll, "KKStopDownAVFile");
		fFree pKKFree=(fFree)GetProcAddress(hdll, "KKFree");
        fKKDownAVFileSpeedInfo pKKDownAVFileSpeedInfo=(fKKDownAVFileSpeedInfo)GetProcAddress(hdll, "KKDownAVFileSpeedInfo");

		if(pfn!=NULL&&pfGetPtl!=NULL&& pDel!=NULL)
		{
			
				KKPluginInfo Info;
				pfGetPtl(Info.ptl,32);
				Info.CreKKP= pfn;
				Info.DelKKp=pDel;
                Info.Handle=hdll;
				Info.KKDownAVFile=pKKDownAVFile;
				Info.KKStopDownAVFile=pKKStopDownAVFile;
				Info.KKFree=pKKFree;
				Info.KKDownAVFileSpeedInfo=pKKDownAVFileSpeedInfo;
				KKPlayer::AddKKPluginInfo(Info);
			
		}else{
			FreeLibrary(hdll);
		}

		int i=0;
		i++;
	}

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
    theApp->RegisterWndFactory(TplSWindowFactory<CKKmclv>());

	
	 KKWkeLoader wkeLoader;
	 if(wkeLoader.Init(_T("wke.dll")))
	 {
	   theApp->RegisterWndFactory(TplSWindowFactory<KKWkeWebkit>());
	 }

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

	CAVDownManage Dow;
	Dow.Start();
	std::wstring path=GetModulePath();
	
	SOUI::SStringT str;
	SOUI::CMainDlg dlgMain;
	m_pDlgMain=&dlgMain;
	dlgMain.Create(NULL,WS_POPUP|WS_MINIMIZEBOX | WS_MAXIMIZEBOX&~WS_CAPTION,0,0,0,0,0);
	dlgMain.GetNative()->SendMessage(WM_INITDIALOG);
	dlgMain.CenterWindow(dlgMain.m_hWnd);
	dlgMain.ShowWindow(SW_SHOWNORMAL);

	//int nRet =  Run(lpstrCmdLine, nCmdShow);
	int 	nRet = theApp->Run(dlgMain.m_hWnd);

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	_Module.Term();
	::CoUninitialize();

	return nRet;
}
