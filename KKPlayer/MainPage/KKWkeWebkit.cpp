#include "StdAfx.h"
#include "KKWkeWebkit.h"
#include <Imm.h>
#include <map>
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"..\\wke\\lib\\wke.lib")
#include "../Tool/cchinesecode.h"
#include "MainDlg.h"
#include <string>
extern SOUI::CMainDlg *m_pDlgMain;
jsValue JS_CALL  FunCallAVPlayer(jsExecState es)
{
     char *pp=(char*)jsToString(es,jsArg(es,0));
	 if(pp!=NULL){
		 std::string Out="";
		 CChineseCode::UTF_8ToGBK(Out,pp);
		 if(m_pDlgMain!=NULL){
			 m_pDlgMain->WinTabShow(1);
			 m_pDlgMain->OpenMedia((char*)Out.c_str());
		 }
		 
	 }
	 	return 0;
}
namespace SOUI
{

	
    //////////////////////////////////////////////////////////////////////////
    // KKWkeLoader
    KKWkeLoader * KKWkeLoader::s_pInst=0;
    
    KKWkeLoader* KKWkeLoader::GetInstance()
    {
        return s_pInst;
    }

    KKWkeLoader::KKWkeLoader() :m_hModWke(0)
    {
        SASSERT(!s_pInst);
        s_pInst=this;
		m_hCheckThread=INVALID_HANDLE_VALUE;
		m_Exit=-1;
    }


    KKWkeLoader::~KKWkeLoader()
    {
		if(m_Exit>-1)
		{
			m_Exit=1;
			while(m_Exit==1)
			{
				Sleep(10);
				break;
			}
		}
		
		if(m_hCheckThread!=INVALID_HANDLE_VALUE)
			::CloseHandle(m_hCheckThread);
        if(m_hModWke) 
			FreeLibrary(m_hModWke);
    }
    
    BOOL KKWkeLoader::Init( LPCTSTR pszDll )
    {
        if(m_hModWke) 
			return TRUE;
        HMODULE m_hModWke=LoadLibrary(pszDll);
        if(!m_hModWke) 
			return FALSE;
        m_funWkeInit = (FunWkeInit)GetProcAddress(m_hModWke,"wkeInit");
        m_funWkeShutdown = (FunWkeShutdown)GetProcAddress(m_hModWke,"wkeShutdown");
        m_funWkeCreateWebView = (FunWkeCreateWebView)GetProcAddress(m_hModWke,"wkeCreateWebView");
        m_funWkeDestroyWebView = (FunWkeDestroyWebView)GetProcAddress(m_hModWke,"wkeDestroyWebView");
        if(!m_funWkeInit 
            || !m_funWkeShutdown
            || !m_funWkeCreateWebView
            || !m_funWkeDestroyWebView )
        {
            FreeLibrary(m_hModWke);
            return FALSE;
        }
		jsBindFunction("CallAVPlayer",FunCallAVPlayer,1);
        m_funWkeInit();
		m_Exit=0;
		m_hCheckThread=::CreateThread(NULL, 0, UpdateWkeWebViewTh,this , 0, NULL);
        return TRUE;
    }

	CriSecLock GwkeViewLock;
	std::map<wkeWebView,KKWkeWebkit*> GUpdateViewMap;
	void AddWkeViewToMap(wkeWebView p,KKWkeWebkit* w)
	{
		/*if(p!=NULL)
		{
			GwkeViewLock.Lock();
			if(GUpdateViewMap.find(p)==GUpdateViewMap.end())
			{
				GUpdateViewMap.insert(std::pair<wkeWebView,KKWkeWebkit*>(p,w));
			}

			GwkeViewLock.Unlock();
		}*/
	}
	void DelWkeViewToMap(wkeWebView p)
	{
		/*if(p!=NULL)
		{
			GwkeViewLock.Lock();
			std::map<wkeWebView,KKWkeWebkit*>::iterator It=GUpdateViewMap.find(p);
			if(It!=GUpdateViewMap.end())
			{
				GUpdateViewMap.erase(It);
			}
			GwkeViewLock.Unlock();
		}*/
	}

	DWORD WINAPI KKWkeLoader::UpdateWkeWebViewTh(LPVOID lpThreadParameter)
	{
		KKWkeLoader *pIns=(KKWkeLoader *)lpThreadParameter;
		while(pIns->m_Exit==0)
		{
			Sleep(20);
			//GwkeViewLock.Lock();
			//std::map<wkeWebView,KKWkeWebkit*>::iterator It=GUpdateViewMap.begin();
			//for(;It!=GUpdateViewMap.end();++It)
			//{
			//	It->first->setDirty(true);
			////	It->second->onUpdated();
			//}
			//GwkeViewLock.Unlock();
		}
		return 0;
	}

	

	


    //////////////////////////////////////////////////////////////////////////
    // KKWkeWebkit
    
    KKWkeWebkit::KKWkeWebkit(void):m_pWebView(NULL)
    {
		m_strUrl=L"";
		m_strFile=L"";
		int ii=0;
		ii++;
    }

    KKWkeWebkit::~KKWkeWebkit(void)
    {
		if(m_pWebView)
		{
			DelWkeViewToMap(m_pWebView);
			KKWkeLoader::GetInstance()->m_funWkeDestroyWebView(m_pWebView);
			m_pWebView=NULL;
		}
    }

    void KKWkeWebkit::OnPaint(IRenderTarget *pRT)
    {
        CRect rcClip;
        pRT->GetClipBox(&rcClip);
        CRect rcClient;
        GetClientRect(&rcClient);
        CRect rcInvalid;
        rcInvalid.IntersectRect(&rcClip,&rcClient);
        HDC hdc=pRT->GetDC();
        if(GetStyle().m_byAlpha!=0xff)
        {
            BLENDFUNCTION bf={AC_SRC_OVER,0,GetStyle().m_byAlpha,AC_SRC_ALPHA };
            AlphaBlend(hdc,rcInvalid.left,rcInvalid.top,rcInvalid.Width(),rcInvalid.Height(),m_pWebView->getViewDC(),rcInvalid.left-rcClient.left,rcInvalid.top-rcClient.top,rcInvalid.Width(),rcInvalid.Height(),bf);
        }else
        {
            BitBlt(hdc,rcInvalid.left,rcInvalid.top,rcInvalid.Width(),rcInvalid.Height(),m_pWebView->getViewDC(),rcInvalid.left-rcClient.left,rcInvalid.top-rcClient.top,SRCCOPY);            
        }
        pRT->ReleaseDC(hdc);
    }

    void KKWkeWebkit::OnSize( UINT nType, CSize size )
    {
        __super::OnSize(nType,size);		
        m_pWebView->resize(size.cx,size.cy);
        m_pWebView->tick();
    }

    int KKWkeWebkit::OnCreate( void * )
    {
        m_pWebView = KKWkeLoader::GetInstance()->m_funWkeCreateWebView();
        if(!m_pWebView) 
			return 1;
	
		m_nTM_TICKER=212;
        m_pWebView->setBufHandler(this);
		if(m_strUrl.GetLength()>5)
           m_pWebView->loadURL(m_strUrl);
		else
			m_pWebView->loadFile(m_strFile);
		AddWkeViewToMap(m_pWebView,this);
		BOOL xx=this->SetTimer2(m_nTM_TICKER,20);
        return 0;
	}

	void KKWkeWebkit::OnDestroy()
	{
		if(m_pWebView)
		{
			DelWkeViewToMap(m_pWebView);
			KKWkeLoader::GetInstance()->m_funWkeDestroyWebView(m_pWebView);
			m_pWebView=NULL;
		}
	}

	LRESULT KKWkeWebkit::OnMouseEvent( UINT message, WPARAM wParam,LPARAM lParam)
	{
		if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
		{
			SetFocus();
			SetCapture();
		}
		else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
		{
			ReleaseCapture();
		}

		CRect rcClient;
		GetClientRect(&rcClient);

		int x = GET_X_LPARAM(lParam)-rcClient.left;
		int y = GET_Y_LPARAM(lParam)-rcClient.top;

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		bool bHandled = m_pWebView->mouseEvent(message, x, y, flags);
		SetMsgHandled(bHandled);
		return 0;
	}

	LRESULT KKWkeWebkit::OnKeyDown( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(m_pWebView->keyDown(wParam, flags, false));
		return 0;
	}

	LRESULT KKWkeWebkit::OnKeyUp( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(m_pWebView->keyUp(wParam, flags, false));
		return 0;
	}

	LRESULT KKWkeWebkit::OnMouseWheel( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		CRect rc;
		GetWindowRect(&rc);
		pt.x -= rc.left;
		pt.y -= rc.top;

		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;

		BOOL handled = m_pWebView->mouseWheel(pt.x, pt.y, delta, flags);
		SetMsgHandled(handled);

		return handled;
	}

	LRESULT KKWkeWebkit::OnChar( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int charCode = wParam;
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		SetMsgHandled(m_pWebView->keyPress(charCode, flags, false));
		return 0;
	}

	LRESULT KKWkeWebkit::OnImeStartComposition( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		wkeRect caret = m_pWebView->getCaret();

		CRect rcClient;
		GetClientRect(&rcClient);

		CANDIDATEFORM form;
		form.dwIndex = 0;
		form.dwStyle = CFS_EXCLUDE;
		form.ptCurrentPos.x = caret.x + rcClient.left;
		form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
		form.rcArea.top = caret.y + rcClient.top;
		form.rcArea.bottom = caret.y + caret.h + rcClient.top;
		form.rcArea.left = caret.x + rcClient.left;
		form.rcArea.right = caret.x + caret.w + rcClient.left;
		COMPOSITIONFORM compForm;
		compForm.ptCurrentPos=form.ptCurrentPos;
		compForm.rcArea=form.rcArea;
		compForm.dwStyle=CFS_POINT;

		HWND hWnd=GetContainer()->GetHostHwnd();
		HIMC hIMC = ImmGetContext(hWnd);
		ImmSetCandidateWindow(hIMC, &form);
		ImmSetCompositionWindow(hIMC,&compForm);
		ImmReleaseContext(hWnd, hIMC);
		return 0;
	}

	void KKWkeWebkit::OnSetFocus(SWND wndOld)
	{
	    __super::OnSetCursor(wndOld);
		m_pWebView->focus();
	}

	void KKWkeWebkit::OnKillFocus(SWND wndFocus)
	{
		m_pWebView->unfocus();
		__super::OnKillFocus(wndFocus);
	}

	void KKWkeWebkit::OnTimer(UINT_PTR cTimerID )
	{
		if(cTimerID==m_nTM_TICKER)
		{
			m_pWebView->tick();
		}
		
	
	}
void  KKWkeWebkit::onUpdated()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	
	InvalidateRect(rcClient);
}
	void KKWkeWebkit::onBufUpdated( const HDC hdc,int x, int y, int cx, int cy )
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcInvalid(CPoint(x,y),CSize(cx,cy));
		rcInvalid.OffsetRect(rcClient.TopLeft());
		InvalidateRect(rcInvalid);
	}

    BOOL KKWkeWebkit::OnIdle()
    {
		if(m_pWebView!=NULL)
		     m_pWebView->tick();
        return TRUE;
    }

	BOOL KKWkeWebkit::OnSetCursor( const CPoint &pt )
	{
		return TRUE;
	}

	BOOL KKWkeWebkit::OnAttrUrl( SStringW strValue, BOOL bLoading )
	{
		m_strUrl=strValue;
		//m_strUrl=L"http://www.baidu.com/";//strValue;
		if(!bLoading)
			m_pWebView->loadURL(m_strUrl);
		return !bLoading;
	}
    BOOL  KKWkeWebkit::OnAttrFile(SStringW strValue, BOOL bLoading)
	{
		m_strFile=strValue;
		//m_strUrl=L"http://www.baidu.com/";//strValue;
		if(!bLoading)
			m_pWebView->loadFile(m_strFile);
		return !bLoading;
	}


}

