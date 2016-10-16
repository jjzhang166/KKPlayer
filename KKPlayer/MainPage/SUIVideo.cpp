#include "stdafx.h"
#include "SUIVideo.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
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
			::ShowWindow(m_VideoWnd.m_hWnd,SW_SHOW);
		}else
		{
			::ShowWindow(m_VideoWnd.m_hWnd,SW_HIDE);
		}
	}
	//获取放播的历史信息
	void CSuiVideo::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
	{
         m_VideoWnd.GetAVHistoryInfo(slQue);
	}
	void CSuiVideo::SetPlayStat(int state)
	{
        m_VideoWnd.Pause();
	}
	void CSuiVideo::OnDestroy()
	{
          m_VideoWnd.CloseMedia();
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
	 void CSuiVideo::SetVolume(long value)
	 {
           m_VideoWnd.SetVolume(value);
	 }
	 void CSuiVideo::AvSeek(int value)
	 {
          m_VideoWnd.AvSeek(value);
	 }
	MEDIA_INFO   CSuiVideo::GetMeadiaInfo()
	{
       return     m_VideoWnd.GetMediaInfo();
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

			
			::SetWindowPos(m_VideoWnd.m_hWnd,0,rt.left,rt.top,size.cx,size.cy,SWP_NOZORDER);
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
		m_pDlgMain->FindChildByName("TxtAVTitle")->SetWindowText(L"KK影音");
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
	 int CSuiVideo::OpenMedia(const char *str)
	 {
         int ret= m_VideoWnd.OpenMedia(str);
		 if(ret==-1)
		 {
			 m_VideoWnd.CloseMedia();
			 ret= m_VideoWnd.OpenMedia(str);
			 if(ret!=0)
				 return -1;
			
		 }
		 WCHAR abcd[1024];
		 std::string title2;
		 
		 std::wstring title=L"KK影音-";
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

		 CChineseCode::charTowchar(title2.c_str(),abcd,1024);
		 title+=abcd;
		 m_pDlgMain->FindChildByName("TxtAVTitle")->SetWindowText(title.c_str());
		  return 0;
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