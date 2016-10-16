
#include <wke.h>
#include "../stdafx.h"
#ifndef KKWkeWebkit_H_
#define KKWkeWebkit_H_
namespace SOUI
{   
    class KKWkeLoader
    {
    public:
        typedef void (*FunWkeInit)();
        typedef void (*FunWkeShutdown)();
        typedef wkeWebView (*FunWkeCreateWebView)();
        typedef void (*FunWkeDestroyWebView)(wkeWebView);
    public:
        KKWkeLoader();

        ~KKWkeLoader();

        BOOL Init(LPCTSTR pszDll);

        static KKWkeLoader* GetInstance();
    public:
        FunWkeCreateWebView m_funWkeCreateWebView;
        FunWkeDestroyWebView m_funWkeDestroyWebView;
    protected:
		static DWORD WINAPI UpdateWkeWebViewTh(LPVOID lpThreadParameter);
        HMODULE     m_hModWke;
        FunWkeInit  m_funWkeInit;
        FunWkeShutdown m_funWkeShutdown;
        HANDLE m_hCheckThread;
		int m_Exit;
        static KKWkeLoader * s_pInst;
    };

   
    class KKWkeWebkit : public SWindow, protected wkeBufHandler , protected IIdleHandler
    {
        SOUI_CLASS_NAME( KKWkeWebkit, L"kkwkewebkit")
    public:
        KKWkeWebkit(void);
        ~KKWkeWebkit(void);
       
        wkeWebView	GetWebView(){return m_pWebView;}
		void onUpdated();
    protected:
        virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);
        virtual BOOL OnIdle();
		int m_nTM_TICKER;
    protected:
        int OnCreate(void *);
        void OnDestroy();
        void OnPaint(IRenderTarget *pRT);
        void OnSize(UINT nType, CSize size);
        LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam); 
        LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam); 
        LRESULT OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam);
        LRESULT OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam);
        LRESULT OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam);
        LRESULT OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam);
        void OnSetFocus(SWND wndOld);
        void OnKillFocus(SWND wndFocus);
        void OnTimer(UINT_PTR nIDEvent);

        virtual BOOL OnSetCursor(const CPoint &pt);
        virtual UINT OnGetDlgCode(){return SC_WANTALLKEYS;}
        BOOL OnAttrUrl(SStringW strValue, BOOL bLoading);
		BOOL OnAttrFile(SStringW strValue, BOOL bLoading);
        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"url",OnAttrUrl)
			ATTR_CUSTOM(L"file",OnAttrFile)
        SOUI_ATTRS_END()

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SIZE(OnSize)
            MSG_WM_TIMER2(OnTimer)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
            MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,0x209,OnMouseEvent)
            MESSAGE_HANDLER_EX(WM_MOUSEWHEEL,OnMouseWheel)
            MESSAGE_HANDLER_EX(WM_KEYDOWN,OnKeyDown)
            MESSAGE_HANDLER_EX(WM_KEYUP,OnKeyUp)
            MESSAGE_HANDLER_EX(WM_CHAR,OnChar)
            MESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,OnImeStartComposition)
        SOUI_MSG_MAP_END()

    protected:
        wkeWebView m_pWebView;
        SStringW m_strUrl;
		SStringW m_strFile;
    };
}
#endif