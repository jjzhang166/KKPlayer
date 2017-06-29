// MangageTrade.cpp : main source file for MangageTrade.exe
//

#include "stdafx.h"

#include "resource.h"
#include "MainPage/MainDlg.h"
#include "MainPage/SUIVideo.h"
#include "MainPage/KKWkeWebkit.h"
#include "control/kkmclv.h"
#include "Control/kkseek.h"
#include "Dir/Dir.hpp"
#include "Tool/CFileMgr.h"
#include "Tool/cchinesecode.h"
#include "Tool/FileRelation.h"
#include "Tool/WinDir.h"

#include "SqlOp/HistoryInfoMgr.h"
#include "../KKPlayerCore/KKPlugin.h"
#include "../KKPlayerCore/KKPlayer.h"




void DeclareDumpFile();

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

CreateRender pfnCreateRender = NULL;
DelRender    pfnDelRender=NULL;
//F:\ProgramTool\OpenPro\KKPlayer\KKPlayer>uiresbuilder.exe -iuires/uires.idx -puires -rres/KK_res.rc2


void RelationIco()
{
    CFileRelation FileRelation;
	CWinDir windir;
	std::wstring Localpath=windir.GetModulePath();
	std::wstring pathIco=Localpath+L"KKIcons.dll,"; 
	std::wstring Propath=Localpath+_T("\\KKplayer.exe");
	
	
	bool bReIcon=FileRelation.CheckFileRelation(L".3gp",L"KKPlayer.3gp");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-101"; 
       
		FileRelation.RegisterFileRelation(_T(".3gp"),(LPTSTR)Propath.c_str(),_T("KKPlayer.3gp"),(LPTSTR)pathIco.c_str(),_T(".3gp"));
	}

	bReIcon=FileRelation.CheckFileRelation(L".avi",L"KKPlayer.avi");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-102"; 
       
		FileRelation.RegisterFileRelation(_T(".avi"),(LPTSTR)Propath.c_str(),_T("KKPlayer.avi"),(LPTSTR)pathIco.c_str(),_T(".avi"));
	}
	bReIcon=FileRelation.CheckFileRelation(L".bt",L"KKPlayer.bt");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-103"; 
       
		FileRelation.RegisterFileRelation(_T(".bt"),(LPTSTR)Propath.c_str(),_T("KKPlayer.bt"),(LPTSTR)pathIco.c_str(),_T(".bt"));
	}

    bReIcon=FileRelation.CheckFileRelation(L".flv",L"KKPlayer.flv");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-104"; 
       
		FileRelation.RegisterFileRelation(_T(".flv"),(LPTSTR)Propath.c_str(),_T("KKPlayer.flv"),(LPTSTR)pathIco.c_str(),_T(".flv"));
	}
	bReIcon=FileRelation.CheckFileRelation(L".mkv",L"KKPlayer.mkv");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-105"; 
       
		FileRelation.RegisterFileRelation(_T(".mkv"),(LPTSTR)Propath.c_str(),_T("KKPlayer.mkv"),(LPTSTR)pathIco.c_str(),_T(".mkv"));
	}
	bReIcon=FileRelation.CheckFileRelation(L".mov",L"KKPlayer.mov");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-106"; 
       
		FileRelation.RegisterFileRelation(_T(".mov"),(LPTSTR)Propath.c_str(),_T("KKPlayer.mov"),(LPTSTR)pathIco.c_str(),_T(".mov"));
	}

	bReIcon=FileRelation.CheckFileRelation(L".mp4",L"KKPlayer.mp4");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-107"; 
       
		FileRelation.RegisterFileRelation(_T(".mp4"),(LPTSTR)Propath.c_str(),_T("KKPlayer.mp4"),(LPTSTR)pathIco.c_str(),_T(".mp4"));
	}
	
	bReIcon=FileRelation.CheckFileRelation(L".mov",L"KKPlayer.mpg");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-108"; 
       
		FileRelation.RegisterFileRelation(_T(".mpg"),(LPTSTR)Propath.c_str(),_T("KKPlayer.mpg"),(LPTSTR)pathIco.c_str(),_T(".mpg"));
	}

	bReIcon=FileRelation.CheckFileRelation(L".rm",L"KKPlayer.rm");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-109"; 
       
		FileRelation.RegisterFileRelation(_T(".rm"),(LPTSTR)Propath.c_str(),_T("KKPlayer.rm"),(LPTSTR)pathIco.c_str(),_T(".rm"));
	}

	bReIcon=FileRelation.CheckFileRelation(L".rmvb",L"KKPlayer.rmvb");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-110"; 
       
		FileRelation.RegisterFileRelation(_T(".rmvb"),(LPTSTR)Propath.c_str(),_T("KKPlayer.rmvb"),(LPTSTR)pathIco.c_str(),_T(".rmvb"));
	}

	bReIcon=FileRelation.CheckFileRelation(L".wma",L"KKPlayer.wma");
	if(!bReIcon){
		pathIco=Localpath+L"KKIcons.dll,-111"; 
		FileRelation.RegisterFileRelation(_T(".wma"),(LPTSTR)Propath.c_str(),_T("KKPlayer.wma"),(LPTSTR)pathIco.c_str(),_T(".wma"));
	}
	
}
void LoadPlugin()
{
	CWinDir Dir;
//装载
	std::string strPath= Dir.GetModulePathA();
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
		fCreateKKPlugin        pfn = (fCreateKKPlugin)GetProcAddress(hdll, "CreateKKPlugin");
		fGetPtlHeader          pfGetPtl=(fGetPtlHeader)GetProcAddress(hdll, "GetPtlHeader");
		fDeleteKKPlugin        pDel=(fDeleteKKPlugin)GetProcAddress(hdll, "DeleteKKPlugin");
        fKKDownAVFile          pKKDownAVFile=(fKKDownAVFile)GetProcAddress(hdll, "KKDownAVFile");
		fKKPauseDownAVFile     pKKPauseDownAVFile=(fKKPauseDownAVFile)GetProcAddress(hdll, "KKPauseDownAVFile");
		fFree                  pKKFree=(fFree)GetProcAddress(hdll, "KKFree");
        fKKDownAVFileSpeedInfo pKKDownAVFileSpeedInfo=(fKKDownAVFileSpeedInfo)GetProcAddress(hdll, "KKDownAVFileSpeedInfo");
        fKKUrlParser           pKKUrlParser=(fKKUrlParser)GetProcAddress(hdll, "KKUrlParser");
		if(pfn!=NULL&&pfGetPtl!=NULL&& pDel!=NULL)
		{
			
				KKPluginInfo Info;
				pfGetPtl(Info.ptl,32);
				Info.CreKKP= pfn;
				Info.DelKKp=pDel;
                Info.Handle=hdll;
				Info.KKDownAVFile=pKKDownAVFile;
				Info.KKPauseDownAVFile=pKKPauseDownAVFile;
				Info.KKFree=pKKFree;
				Info.KKDownAVFileSpeedInfo=pKKDownAVFileSpeedInfo;
				Info.KKUrlParser=pKKUrlParser;
				KKPlayer::AddKKPluginInfo(Info);
			
		}else{
			FreeLibrary(hdll);
		}

		int i=0;
		i++;
	}

	DllPathInfoList.clear();
}


int APIENTRY _tWinMain2(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	int nArgs = 0;   
    DeclareDumpFile();
	
    ///电源管理
	::SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED |ES_CONTINUOUS);
	
	LPWSTR *szArglist = NULL;   
	std::wstring  urlpath;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);   
	if( NULL != szArglist)   
	{   
         for(int i=1;i<nArgs;i++)
		 {
		     urlpath+=*(szArglist+i);
			 if(i!=nArgs-1)
			   urlpath+=L" ";
		 }
	} 
	LocalFree(szArglist);

    
    
   
	FARPROC spdpia = GetProcAddress(GetModuleHandle(TEXT("user32")), "SetProcessDPIAware");  
	if(spdpia!=NULL)
	{
	   spdpia();
	}

	CWinDir windir;
	std::wstring Propath=windir.GetUserAppDir();
	Propath+=L"\\kkplayer\\";
	windir.CreateDir(Propath.c_str());
	Propath+=L"DB";
	std::string pp;
	CChineseCode::UnicodeToUTF8((wchar_t*)Propath.c_str(),pp);/**/

	///关联图标
	RelationIco();
	
	CHistoryInfoMgr *hismgr=CHistoryInfoMgr::GetInance();
	hismgr->SetPath(pp.c_str());
	hismgr->InitDb();
	hismgr->GetH264Codec();
	hismgr->GetH265Codec();
	//
	
	//
 //   //插件
	LoadPlugin();
	
	HMODULE hRender = LoadLibraryA("Render.dll");
	if(hRender){
          pfnCreateRender = (CreateRender)GetProcAddress(hRender, "CreateRender");
		  pfnDelRender = (DelRender)GetProcAddress(hRender, "DelRender");
	}

	using namespace SOUI;
	SComMgr * pComMgr = new SComMgr;

	
    SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory=NULL;
	///GDI渲染器
	pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
	
	///图片解码器
	SOUI::CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
	
	
	SOUI::SApplication *theApp=new SApplication(pRenderFactory,hInstance);

    theApp->RegisterWindowClass<CSuiVideo>();
    theApp->RegisterWindowClass<CKKmclv>();
    theApp->RegisterWindowClass<SAVSeekBar>();
	
	 KKWkeLoader wkeLoader;
	 if(wkeLoader.Init(_T("wke.dll")))
	 {
	   theApp->RegisterWindowClass<KKWkeWebkit>();
	 }/**/
    
	
	 ///不知道什么原因，在win10下，会导致百度网盘等闪烁
	 HMODULE hui=LoadLibrary(_T("kkui.dll"));
	if(hui){
		CAutoRefPtr<IResProvider>   pResProvider;
		CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
		BOOL ret=pResProvider->Init((WPARAM)hui,0);
		theApp->AddResProvider(pResProvider);
	}else{
       assert(0);
	}
	//加载系统资源
	HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
	if(hSysResource){
		CAutoRefPtr<IResProvider> sysSesProvider;
		CreateResProvider(RES_PE,(IObjRef**)&sysSesProvider);
		sysSesProvider->Init((WPARAM)hSysResource,0);
		theApp->LoadSystemNamedResource(sysSesProvider);
	}

	
	
	CAutoRefPtr<ITranslatorMgr> trans;
	pComMgr->CreateTranslator((IObjRef**)&trans);
	if(trans)
	{
	        theApp->SetTranslator(trans);
            pugi::xml_document xmlLang;
            if(theApp->LoadXmlDocment(xmlLang,_T("lang_cn"),_T("translator"))){
                CAutoRefPtr<ITranslator> langCN;
                trans->CreateTranslator(&langCN);
                langCN->Load(&xmlLang.child(L"language"),1);//1=LD_XML
                trans->InstallTranslator(langCN);
            }
	}


	

	int 	nRet=0;
	SOUI::SStringT str;
	SOUI::CMainDlg dlgMain;
	dlgMain.Create(NULL,WS_POPUP,0,0,0,0,0);
	dlgMain.GetNative()->SendMessage(WM_INITDIALOG);
	dlgMain.CenterWindow(dlgMain.m_hWnd);
	dlgMain.ShowWindow(SW_SHOWNORMAL);
	if(urlpath.length()>3)
	{
	    char localurl[1024]="";
	    CChineseCode::wcharTochar(urlpath.c_str(), localurl,1024);
		dlgMain.WinTabShow(1);
	    dlgMain.OpenMedia(localurl);
	}
	nRet = theApp->Run(dlgMain.m_hWnd);/**/

 
	
	
	::SetThreadExecutionState(ES_CONTINUOUS);
	return nRet;
}
