#include "stdafx.h"
#include "SUIVideo.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"

#include <string>
extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CSuiVideo::CSuiVideo(void)
	{

	}
	CSuiVideo::~CSuiVideo(void)
	{

	}
	void CSuiVideo::SetAVVisible(BOOL bVisible)
	{
		if(bVisible==TRUE)
		{
			if(!::IsWindowVisible(m_VideoWnd.m_hWnd))
			   ::ShowWindow(m_VideoWnd.m_hWnd,SW_SHOW);
		}else
		{
			if(::IsWindowVisible(m_VideoWnd.m_hWnd))
			   ::ShowWindow(m_VideoWnd.m_hWnd,SW_HIDE);
		}
	}
	
	HWND CSuiVideo::GetPlayerHwnd()
	{
	          return m_VideoWnd.m_hWnd;
	}
	void CSuiVideo::FullScreen()
	{
	    m_VideoWnd.FullScreen();
	}
	void CSuiVideo::SetPlayStat(int state)
	{
        m_VideoWnd.Pause();
	}
	void CSuiVideo::OnDestroy()
	{
          m_VideoWnd.CloseMedia();
		  ::DestroyWindow(m_VideoWnd.m_hWnd);
	}
	int  CSuiVideo::OnCreate(void* p)
	{
		int ll= __super::OnCreate(NULL);
		HWND h=GetContainer()->GetHostHwnd();
		RECT rt={0,100,200,300};
		//m_VideoWnd.Create()
		if(m_VideoWnd.CreateEx(h,rt, WS_CHILDWINDOW| WS_CLIPCHILDREN ) == NULL)//WS_VISIBLE|| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
		{
				return 0;
		}/**/
		
        return ll;
	}
	long  CSuiVideo::GetVolume()
	{
	  return m_VideoWnd.GetVolume();
	}
	 void CSuiVideo::SetVolume(long value)
	 {
           m_VideoWnd.SetVolume(value);
	 }
	 void CSuiVideo::AvSeek(int value)
	 {
          m_VideoWnd.AvSeek(value);
	 }
	bool   CSuiVideo::GetMeadiaInfo(MEDIA_INFO &info)
	{
       return     m_VideoWnd.GetMediaInfo(info);
	}
	bool CSuiVideo::GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale)
	{
	    return     m_VideoWnd.GrabAvPicBGRA(buf,len,w,h,keepscale);
	}
	void CSuiVideo::OnPaint(IRenderTarget *pRT)
	{
          __super::OnPaint(pRT);
	}
	void  CSuiVideo::OnSize(UINT nType, CSize size)
	{
        __super::OnSize(nType,size);
		if(m_VideoWnd.IsWindow())
		{
			RECT rt;
			this->GetWindowRect(&rt);

			int  cx   =   GetSystemMetrics(SM_CXSCREEN);   
			int  cy   =   GetSystemMetrics(SM_CYSCREEN);
			RECT rtx;
			::GetWindowRect(::GetParent(m_VideoWnd.m_hWnd),&rtx);
			int h=rtx.bottom-rtx.top;
			int w=rtx.right-rtx.left;
			if(w>=cx&&h>=cy)
			{
                   ::SetWindowPos(m_VideoWnd.m_hWnd,0,0,0,cx,cy,SWP_NOZORDER);
			}else{
                   ::SetWindowPos(m_VideoWnd.m_hWnd,0,rt.left,rt.top,size.cx,size.cy,SWP_NOZORDER);
			}
			
		}
	}
	int CSuiVideo::PktSerial()
	{
       return m_VideoWnd.PktSerial();
	}
	void CSuiVideo::OnMouseHover(WPARAM wParam, CPoint ptPos)
	{

	}
	void CSuiVideo::OnMouseLeave()
	{

	}
	void CSuiVideo::Close()
	{
		m_VideoWnd.CloseMedia();
		//GetContainer()->GetHostHwnd();
		m_pDlgMain->FindChildByName("TxtAVTitle")->SetWindowText(L"KKÓ°Òô");
	}

	int is_realtime2(char *str)
	{
		if(   !strcmp(str, "rtp")    || 
			!strcmp(str, "rtsp")   || 
			!strcmp(str, "sdp")
			)
			return 1;


		if( !strncmp(str, "rtmp:",5))
		{
			return 1;
		}
		if(   !strncmp(str, "rtp:", 4)|| 
			!strncmp(str, "udp:", 4) 
			)
			return 1;
		return 0;
	}
	int CSuiVideo::DownMedia(char *KKVURL)
	{
		int ret= m_VideoWnd.DownMedia(KKVURL);
	    return ret;
	}
	void CSuiVideo::SaveSnapshoot()
	{
	         int BufLen=128*128*4;
			 void* Buf=::malloc(BufLen);
			 int w=128,h=128;
			 if(m_VideoWnd.GrabAvPicBGRA(Buf,BufLen,w,h)){
				 int curTime=m_VideoWnd.GetPlayTime();
				 int TotalTime=m_VideoWnd.GetTotalTime();
				 CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
				 InfoMgr->UpDataAVinfo(m_url.c_str(),curTime,TotalTime,(unsigned char*)Buf,BufLen,w,h);
			 }
			 free(Buf);
	}
	int CSuiVideo::OpenMedia(const char *str)
	{
         int ret= m_VideoWnd.OpenMedia(str);
		 if(ret==-1)
		 {
			 SaveSnapshoot();
			 m_VideoWnd.CloseMedia();
			 ret= m_VideoWnd.OpenMedia(str);
			 if(ret<0)
				 return -1;
		 }
		 
		 std::string title2;
		 m_url=str;
		 std::wstring title=L"KKÓ°Òô-";
         title2=str;
		 if(! is_realtime2((char*)str))
		 {		
				 int index=title2.find_last_of(".");
				 if(index>-1)
				 {
					 title2=title2.substr(0,index);
					 index=title2.find_last_of("/");
					 if(index<0)
						 index=title2.find_last_of("\\");
					 if(index>-1)
					 {
						 title2=title2.substr(index+1,title2.length()-index-1);
					 }else{
						  title2=str;
					 }
				 }
		 }
		 
		 /*CChineseCode::charTowchar(title2.c_str(),abcd,1024);
		 title+=abcd;
		 m_pDlgMain->FindChildByName("TxtAVTitle")->SetWindowText(title.c_str());*/
		  return ret;
	 }
	 void CSuiVideo::OnDecelerate()
	 {
          m_VideoWnd.OnDecelerate();
	 }
	 void CSuiVideo::OnAccelerate()
	 {
         m_VideoWnd.OnAccelerate();
	 }
}