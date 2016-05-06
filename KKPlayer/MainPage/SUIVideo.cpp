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

	void CSuiVideo::OnDestroy()
	{
          m_VideoWnd.CloseMedia();
	}
	int  CSuiVideo::OnCreate(void* p)
	{
		int ll= __super::OnCreate(NULL);
		HWND h=GetContainer()->GetHostHwnd();
		
		RECT rt={0,100,200,300};
		if(m_VideoWnd.CreateEx(h,rt, WS_CHILDWINDOW| WS_VISIBLE | WS_CLIPSIBLINGS|WS_CLIPCHILDREN) == NULL)
		{
			
				return 0;
		}/**/
        return ll;
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
				:: SetWindowPos(m_VideoWnd.m_hWnd,0,rt.left+3,rt.top,size.cx,size.cy,SWP_NOZORDER);
			}
	}
	void CSuiVideo::OnMouseHover(WPARAM wParam, CPoint ptPos)
	{

	}
	void CSuiVideo::OnMouseLeave()
	{

	}
	 int CSuiVideo::OpenMeida(const char *str)
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
}