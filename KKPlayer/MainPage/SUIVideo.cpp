#include "stdafx.h"


#include "SUIVideo.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"

#include <string>

ICallKKplayer* CreateICallKKplayer(HWND ph,DWORD stlye);
namespace SOUI
{
	CSuiVideo::CSuiVideo(void):m_pIKKplayer(0)
	{
              
	}
	CSuiVideo::~CSuiVideo(void)
	{
		if(m_pIKKplayer!=0)
              delete  m_pIKKplayer;
	}
	void CSuiVideo::SetAVVisible(BOOL bVisible)
	{
		if(m_pIKKplayer!=0)
		if(bVisible==TRUE)
		{
			if(!::IsWindowVisible(m_pIKKplayer->GetPlayerWnd()))
			   ::ShowWindow(m_pIKKplayer->GetPlayerWnd(),SW_SHOW);
		}else
		{
			if(::IsWindowVisible(m_pIKKplayer->GetPlayerWnd()))
			   ::ShowWindow(m_pIKKplayer->GetPlayerWnd(),SW_HIDE);
		}
	}
	
	HWND CSuiVideo::GetPlayerHwnd()
	{
		if(m_pIKKplayer!=0)
	          return m_pIKKplayer->GetPlayerWnd();
		return 0;
	}
	void CSuiVideo::FullScreen()
	{
		if(m_pIKKplayer!=0)
	    m_pIKKplayer->FullScreen();
	}
	void CSuiVideo::SetPlayStat(int state)
	{
		if(m_pIKKplayer!=0)
        m_pIKKplayer->Pause();
	}
	void CSuiVideo::OnDestroy()
	{
		if(m_pIKKplayer!=0)
		{
          m_pIKKplayer->CloseMedia();
		  ::DestroyWindow(m_pIKKplayer->GetPlayerWnd());
		}
	}
	int  CSuiVideo::OnCreate(void* p)
	{
		int ll= __super::OnCreate(NULL);
		HWND h=GetContainer()->GetHostHwnd();
		
		
		m_pIKKplayer=CreateICallKKplayer(h,WS_CHILDWINDOW| WS_CLIPCHILDREN);
		if(m_pIKKplayer == NULL)//WS_VISIBLE|| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
		{
				return 0;
		}/**/
		
        return ll;
	}
	long  CSuiVideo::GetVolume()
	{
		if(m_pIKKplayer!=0)
	       return m_pIKKplayer->GetVolume();
		return 0;
	}
	 void CSuiVideo::SetVolume(long value)
	 {
		 if(m_pIKKplayer!=0)
           m_pIKKplayer->SetVolume(value);
	 }
	 void CSuiVideo::AvSeek(int value)
	 {
		 if(m_pIKKplayer!=0)
          m_pIKKplayer->AvSeek(value);
	 }
	bool   CSuiVideo::GetMeadiaInfo(MEDIA_INFO &info)
	{
		if(m_pIKKplayer!=0)
            return     m_pIKKplayer->GetMediaInfo(info);
		return false;
	}
	bool CSuiVideo::GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale)
	{
		if(m_pIKKplayer!=0)
	    return     m_pIKKplayer->GrabAvPicBGRA(buf,len,w,h,keepscale);
		return false;
	}
	void CSuiVideo::OnPaint(IRenderTarget *pRT)
	{
          __super::OnPaint(pRT);
	}
	void  CSuiVideo::OnSize(UINT nType, CSize size)
	{
        __super::OnSize(nType,size);
		if(m_pIKKplayer!=0)
		{
				HWND H=m_pIKKplayer->GetPlayerWnd();
				if(::IsWindow(H))
				{
					RECT rt;
					this->GetWindowRect(&rt);

					int  cx   =   GetSystemMetrics(SM_CXSCREEN);   
					int  cy   =   GetSystemMetrics(SM_CYSCREEN);
					RECT rtx;
					::GetWindowRect(::GetParent(H),&rtx);
					int h=rtx.bottom-rtx.top;
					int w=rtx.right-rtx.left;
					if(w>=cx&&h>=cy)
					{
						   ::SetWindowPos(H,0,0,0,cx,cy,SWP_NOZORDER);
					}else{
						   ::SetWindowPos(H,0,rt.left,rt.top,size.cx,size.cy,SWP_NOZORDER);
					}
					
				}
		}
	}
	int CSuiVideo::PktSerial()
	{
		if(m_pIKKplayer!=0)
           return m_pIKKplayer->PktSerial();
		return 0;
	}
	void CSuiVideo::OnMouseHover(WPARAM wParam, CPoint ptPos)
	{

	}
	void CSuiVideo::OnMouseLeave()
	{

	}
	void CSuiVideo::Close()
	{
		if(m_pIKKplayer!=0)
		m_pIKKplayer->CloseMedia();
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
		int ret=0;
		if(m_pIKKplayer!=0)
		   ret= m_pIKKplayer->DownMedia(KKVURL);
	    return ret;
	}
	void CSuiVideo::SaveSnapshoot()
	{
		if(m_pIKKplayer!=0)
		{
	         int BufLen=128*128*4;
			 void* Buf=::malloc(BufLen);
			 int w=128,h=128;
			 if(m_pIKKplayer->GrabAvPicBGRA(Buf,BufLen,w,h)){
				 int curTime=m_pIKKplayer->GetPlayTime();
				 int TotalTime=m_pIKKplayer->GetTotalTime();
				 CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
				 InfoMgr->UpDataAVinfo(m_url.c_str(),curTime,TotalTime,(unsigned char*)Buf,BufLen,w,h);
			 }
			 free(Buf);
		}
	}
	int CSuiVideo::OpenMedia(const char *str,const char* avname)
	{
		if(m_pIKKplayer==0)
			return -1;
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

		 int ret= m_pIKKplayer->OpenMedia(avpathstr.c_str());
		 if(ret==-1)
		 {
			 SaveSnapshoot();
			 m_pIKKplayer->CloseMedia();
			 ret= m_pIKKplayer->OpenMedia(str);
			 if(ret<0)
				 return -1;
		 }
		 if(NeedDelay){
			 int delay=InfoMgr->GetRtmpDelay();
		     m_pIKKplayer->SetMaxRealtimeDelay(delay);
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
	
		 HWND P=GetContainer()->GetHostHwnd();
		 ::SendMessage(P,WM_UI_SetAvTilte,(WPARAM)title.c_str(),0);
		 return ret;
	 }
	 void CSuiVideo::OnDecelerate()
	 {
		 if(m_pIKKplayer!=0)
          m_pIKKplayer->OnDecelerate();
	 }
	 void CSuiVideo::OnAccelerate()
	 {
		 if(m_pIKKplayer!=0)
         m_pIKKplayer->OnAccelerate();
	 }
}