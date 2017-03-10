#include "SDomodalwnd.h"

extern SOUI::SApplication *theApp;
namespace SOUI{
	SDomodalwnd::SDomodalwnd(LPCTSTR pszResName,bool AutoClose,bool NoBKey,DWORD style):SHostWnd(pszResName),m_bAutoClose(AutoClose),m_ModelState(0)
		,m_NoBKey(NoBKey)
		,m_style(style)
	{
	
	}
    SDomodalwnd::~SDomodalwnd()
	{
	
	}
	
	void SDomodalwnd::Init()
	{
	
	}
	int SDomodalwnd::DoModel(int x,int y,HWND hOwner)
	{
		m_hOwner=hOwner;
	    HWND hWnd = Create(hOwner,WS_POPUP,m_style,0,0,0,0);
	    if(!IsWindow()) 
			return -1;

		if(x==0&&y==0){
			HWND hx=hOwner;
		    if(!::IsWindow(hx))
			{
				
			  	 hx=GetDesktopWindow();
			}
			RECT rt={0,0,0,0};
			::GetWindowRect(hx,&rt);
			RECT rt2={0,0,0,0};
			::GetWindowRect(hWnd,&rt2);
			x=x+(rt.right-rt.left)/2+rt.left-(rt2.right-rt2.left)/2;
			y=y+(rt.bottom-rt.top)/2+rt.top-(rt2.bottom-rt2.top)/2;
			
		}
		SetWindowPos(HWND_TOPMOST,x,y,0,0,SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOSIZE );    

		
        HWND hActive = hOwner;
        if(!hActive) hActive = ::GetActiveWindow();

		       
        MsgRun(hOwner);
        if(hActive)
        {
            CPoint pt;
            GetCursorPos(&pt);
            ::ScreenToClient(hActive,&pt);
            ::PostMessage(hActive,WM_MOUSEMOVE,0,MAKELPARAM(pt.x,pt.y));/**/
        }

       
       return m_ModelState;
	}
	void SDomodalwnd::MsgRun(HWND hOwner)
    {
		//SComboBox
        HWND hOwnerParent = hOwner;
        while(::GetParent(hOwnerParent))
        {
            hOwnerParent = ::GetParent(hOwnerParent);
        }
        
		if(m_bAutoClose)
              SetForegroundWindow(m_hWnd);
       // ::SetFocus(m_hWnd);
        BOOL bMsgQuit(FALSE);
       
		HWND TipH=((STipCtrl*)m_pTipCtrl)->m_hWnd;;
	    Init();

		 SMessageLoop *loop=theApp->GetMsgLoop();
        for(;;)
        {

            MSG msg = {0};
            BOOL bInterceptOther(FALSE);

			if(m_bAutoClose&&GetForegroundWindow() != m_hWnd)
            {
				this->DestroyWindow();
                break;
            }/**/
            if(!m_NoBKey ||m_NoBKey&&PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
				if(!m_NoBKey)
				{
                    ::GetMessage(&msg,0,0,0);
					//获取消息，不从消息队列中移除。
					if(msg.message == WM_KEYDOWN
						|| msg.message == WM_SYSKEYDOWN
						|| msg.message == WM_KEYUP
						|| msg.message == WM_SYSKEYUP
						|| msg.message == WM_CHAR
						|| msg.message == WM_IME_CHAR)
					{
						bInterceptOther=1;
					}
				}
                
				if(msg.message == WM_LBUTTONDOWN
                    || msg.message  == WM_RBUTTONDOWN
                    || msg.message  == WM_NCLBUTTONDOWN
                    || msg.message  == WM_NCRBUTTONDOWN
                    || msg.message   ==WM_LBUTTONDBLCLK
                    )
                {
                    bInterceptOther=1;
                }
                else if(msg.message == WM_QUIT)
                {
                    bMsgQuit = TRUE;
                }

				if(m_NoBKey){
                //移除消息队列中当前的消息。
                MSG msgT;
                ::GetMessage(&msgT,0,0,0);
				}

                //拦截非菜单窗口的MouseMove消息
                if (msg.message == WM_MOUSEMOVE)
                {
					if(msg.hwnd!=m_hWnd)
					{
					   bInterceptOther=1;
					}
                }
				if(msg.hwnd==m_hWnd&&msg.message==WM_CLOSE ){
                   this->DestroyWindow();
                   break;
				}
				HWND ll=::GetParent(msg.hwnd);
				if(msg.hwnd==m_hWnd|| msg.message == WM_UITASK||m_hWnd==ll)//|| msg.hwnd==TipH 
				{
				   bInterceptOther=0;
				}
				if (!bInterceptOther)
                {
					if(!loop->PreTranslateMessage(&msg))
                    {
							TranslateMessage (&msg);
							DispatchMessage (&msg);
					 }
                }
				
            }
            else
            {
                MsgWaitForMultipleObjects (0, 0, 0, 10, QS_ALLINPUT);
            }
            if(bMsgQuit)
            {
                PostQuitMessage(msg.wParam);
                break;
            }
        }

    }
}