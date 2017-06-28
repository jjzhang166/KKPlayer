#include "stdafx.h"


#include "SUIVideo.h"
#include "MainDlg.h"
#include "../MainFrm.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"

#include <string>
extern SOUI::CMainDlg *m_pDlgMain;
namespace SOUI
{
	CSuiVideo::CSuiVideo(void)
	{
               m_pVideoWnd= new CMainFrame();
	}
	CSuiVideo::~CSuiVideo(void)
	{
              delete  m_pVideoWnd;
	}
	void CSuiVideo::SetAVVisible(BOOL bVisible)
	{
		if(bVisible==TRUE)
		{
			if(!::IsWindowVisible(m_pVideoWnd->m_hWnd))
			   ::ShowWindow(m_pVideoWnd->m_hWnd,SW_SHOW);
		}else
		{
			if(::IsWindowVisible(m_pVideoWnd->m_hWnd))
			   ::ShowWindow(m_pVideoWnd->m_hWnd,SW_HIDE);
		}
	}
	
	HWND CSuiVideo::GetPlayerHwnd()
	{
	          return m_pVideoWnd->m_hWnd;
	}
	void CSuiVideo::FullScreen()
	{
	    m_pVideoWnd->FullScreen();
	}
	void CSuiVideo::SetPlayStat(int state)
	{
        m_pVideoWnd->Pause();
	}
	void CSuiVideo::OnDestroy()
	{
          m_pVideoWnd->CloseMedia();
		  ::DestroyWindow(m_pVideoWnd->m_hWnd);
	}
	int  CSuiVideo::OnCreate(void* p)
	{
		int ll= __super::OnCreate(NULL);
		HWND h=GetContainer()->GetHostHwnd();
		RECT rt={0,100,200,300};
		//m_pVideoWnd->Create()
		if(m_pVideoWnd->CreateEx(h,rt, WS_CHILDWINDOW| WS_CLIPCHILDREN ) == NULL)//WS_VISIBLE|| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
		{
				return 0;
		}/**/
		
        return ll;
	}
	long  CSuiVideo::GetVolume()
	{
	  return m_pVideoWnd->GetVolume();
	}
	 void CSuiVideo::SetVolume(long value)
	 {
           m_pVideoWnd->SetVolume(value);
	 }
	 void CSuiVideo::AvSeek(int value)
	 {
          m_pVideoWnd->AvSeek(value);
	 }
	bool   CSuiVideo::GetMeadiaInfo(MEDIA_INFO &info)
	{
       return     m_pVideoWnd->GetMediaInfo(info);
	}
	bool CSuiVideo::GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale)
	{
	    return     m_pVideoWnd->GrabAvPicBGRA(buf,len,w,h,keepscale);
	}
	void CSuiVideo::OnPaint(IRenderTarget *pRT)
	{
          __super::OnPaint(pRT);
	}
	void  CSuiVideo::OnSize(UINT nType, CSize size)
	{
        __super::OnSize(nType,size);
		if(m_pVideoWnd->IsWindow())
		{
			RECT rt;
			this->GetWindowRect(&rt);

			int  cx   =   GetSystemMetrics(SM_CXSCREEN);   
			int  cy   =   GetSystemMetrics(SM_CYSCREEN);
			RECT rtx;
			::GetWindowRect(::GetParent(m_pVideoWnd->m_hWnd),&rtx);
			int h=rtx.bottom-rtx.top;
			int w=rtx.right-rtx.left;
			if(w>=cx&&h>=cy)
			{
                   ::SetWindowPos(m_pVideoWnd->m_hWnd,0,0,0,cx,cy,SWP_NOZORDER);
			}else{
                   ::SetWindowPos(m_pVideoWnd->m_hWnd,0,rt.left,rt.top,size.cx,size.cy,SWP_NOZORDER);
			}
			
		}
	}
	int CSuiVideo::PktSerial()
	{
       return m_pVideoWnd->PktSerial();
	}
	void CSuiVideo::OnMouseHover(WPARAM wParam, CPoint ptPos)
	{

	}
	void CSuiVideo::OnMouseLeave()
	{

	}
	void CSuiVideo::Close()
	{
		m_pVideoWnd->CloseMedia();
		//GetContainer()->GetHostHwnd();
		m_pDlgMain->FindChildByName("TxtAVTitle")->SetWindowText(L"KK”∞“Ù");
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
		int ret= m_pVideoWnd->DownMedia(KKVURL);
	    return ret;
	}
	void CSuiVideo::SaveSnapshoot()
	{
	         int BufLen=128*128*4;
			 void* Buf=::malloc(BufLen);
			 int w=128,h=128;
			 if(m_pVideoWnd->GrabAvPicBGRA(Buf,BufLen,w,h)){
				 int curTime=m_pVideoWnd->GetPlayTime();
				 int TotalTime=m_pVideoWnd->GetTotalTime();
				 CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
				 InfoMgr->UpDataAVinfo(m_url.c_str(),curTime,TotalTime,(unsigned char*)Buf,BufLen,w,h);
			 }
			 free(Buf);
	}
	int CSuiVideo::OpenMedia(const char *str,const char* avname)
	{
		
         std::string avpathstr=str;
		 CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
		 int UseLibRtmp=InfoMgr->GetUseLibRtmp();
		 bool NeedDelay=false;
		 if(UseLibRtmp){
	            if( !strncmp(str, "rtmp:",5))
				{
					avpathstr="librtmp:";
					avpathstr+=	str;
					NeedDelay=true;
				}
		 }

		 int ret= m_pVideoWnd->OpenMedia(avpathstr.c_str());
		 if(ret==-1)
		 {
			 SaveSnapshoot();
			 m_pVideoWnd->CloseMedia();
			 ret= m_pVideoWnd->OpenMedia(str);
			 if(ret<0)
				 return -1;
		 }
		 if(NeedDelay){
			 int delay=InfoMgr->GetRtmpDelay();
		     m_pVideoWnd->SetMaxRealtimeDelay(delay);
		 }
		 std::string title2;
		 m_url=str;
		 std::wstring title=L"KK”∞“Ù";
         title2=str;
		 if(! is_realtime2((char*)str))
		 {		
			 int index=title2.find_last_of("/");
			 if(index<0)
				 index=title2.find_last_of("\\");
			 if(index>-1)
			 {
				 title2=title2.substr(index+1,title2.length()-index-1);
			 }else{
				  title2=str;
			 }
				
		 }
		 wchar_t abcd[1024]=L"";
		 if(avname!=NULL){
		       CChineseCode::UTF8StrToUnicode(avname,strlen(avname),abcd,1024);
		 }else{
		       CChineseCode::charTowchar(title2.c_str(),abcd,1024);
		 }
		 title=abcd;
		 m_pDlgMain->FindChildByName("AVNameTitle")->SetWindowText(title.c_str());/**/
		 return ret;
	 }
	 void CSuiVideo::OnDecelerate()
	 {
          m_pVideoWnd->OnDecelerate();
	 }
	 void CSuiVideo::OnAccelerate()
	 {
         m_pVideoWnd->OnAccelerate();
	 }
}