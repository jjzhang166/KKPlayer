#include "stdafx.h"
#include "SUIVideo.h"
namespace SOUI
{
	CSuiVideo::CSuiVideo(void)
	{

	}
	CSuiVideo::~CSuiVideo(void)
	{

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
		if(m_VideoWnd.CreateEx(h,rt, WS_CHILD| WS_VISIBLE ) == NULL)//| WS_CLIPSIBLINGS|WS_CLIPCHILDREN
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