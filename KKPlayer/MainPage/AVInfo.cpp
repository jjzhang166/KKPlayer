#include "AVInfo.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CAVInfo::CAVInfo(MEDIA_INFO Info): SDomodalwnd(_T("LAYOUT:XML_AVINFO"))
		,m_Info(Info)
	{
           
	}
	CAVInfo::~CAVInfo()
	{
        
	}
	void CAVInfo::Init()
	{
	     SetAVInfo(m_Info);
	}
	void  CAVInfo::SetAVInfo(MEDIA_INFO &Info)
	{
		//
		//richedit
        SRichEdit* pSRichEdit= (SRichEdit*)this->FindChildByName("EditAVInfo");
		
		WCHAR abcd[1024]=L"";
		CChineseCode::charTowchar(Info.AVinfo,abcd,1024);
		pSRichEdit->SetWindowText(abcd);


		CChineseCode::charTowchar(Info.AVRes,abcd,1024);
		this->FindChildByName("TxtAvRes")->SetWindowText(abcd);


		std::wstring CurTimeStr;
		int h=(Info.TotalTime/(60*60));
		int m=(Info.TotalTime%(60*60))/(60);
		int s=((Info.TotalTime%(60*60))%(60));
		wchar_t strtmp[100]=L"";
		if(h<10){
			swprintf_s(strtmp,L"0%d:",h);
			CurTimeStr=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d:",h);
			CurTimeStr=strtmp;
		}
		if(m<10){
			swprintf_s(strtmp,L"0%d:",m);
			CurTimeStr+=strtmp;
		}
		else{				  
			swprintf_s(strtmp,L"%d:",m);
			CurTimeStr+=strtmp;
		}

		if(s<10){
			swprintf_s(strtmp,L"0%d",s);
			CurTimeStr+=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d",s);
			CurTimeStr+=strtmp;
		}

		
		this->FindChildByName("TxtAvTime")->SetWindowText(CurTimeStr.c_str());


		if(Info.AvFile!=NULL){
			CChineseCode::charTowchar(Info.AvFile,abcd,1024);
			this->FindChildByName("TxtFilePath")->SetWindowText(abcd);
		}
		
	    CurTimeStr=abcd;
		int index=CurTimeStr.find_last_of(L".");
		if(index>-1)
		{
            CurTimeStr=CurTimeStr.substr(index+1,CurTimeStr.length()-index-1);
			this->FindChildByName("AVFileType")->SetWindowText(CurTimeStr.c_str());
		}
		

		if(Info.FileSize>1024*1024*1024)
		{
			swprintf_s(abcd,L"%.3fGb", (double)Info.FileSize/1024/1024/1024);
		}else
		if(Info.FileSize>1024*1024)
		{
			swprintf_s(abcd,L"%dMb", Info.FileSize/1024/1024);
		}else
		{
             swprintf_s(abcd,L"%dKb", Info.FileSize/1024);
		}
		
		this->FindChildByName("TxtFileLen")->SetWindowText(abcd);
		
		
	}
	void CAVInfo::OnLButtonDown(UINT nFlags, CPoint point)
	{
		
		 this->OnMouseEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));
		
         m_pDlgMain->ShowMiniUI(false);
	}

	void CAVInfo::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAVInfo::OnMinimize()
	{
		
	}
}