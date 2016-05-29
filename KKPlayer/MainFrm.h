// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CMainFrame_H_
#define CMainFrame_H_
#include "Sigslot\sigslot.h"
#include "atlmisc.h"
#include "KKPlayer.h"
#include "render/render.h"
#include "../KKPlayerCore/IKKPlayUI.h"
#include <vector>
#include <GdiPlus.h>
#include <GdiPlusHeaders.h>
#include "KKSound.h"
#include "SDLSound.h"
#include "MainPage/AVMenu.h"
struct SWaitPicInfo
{
    unsigned char* Buf;
	int Len;
	short Time;
	int StartTime;
	int Index;
};
#define  WM_MediaClose  WM_USER+100
class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	/*public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
	public CIdleHandler,*/
	public IKKPlayUI,
	public sigslot::has_slots<>
{
public:
	CMainFrame();
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	//BEGIN_UPDATE_UI_MAP(CMainFrame)
	//END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
			MESSAGE_HANDLER(WM_CREATE, OnCreate)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			/*CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
			CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)*/

			MESSAGE_HANDLER(WM_PAINT,OnPaint);
			MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd);
			MESSAGE_HANDLER(WM_SIZE,OnSize);
			MESSAGE_HANDLER(WM_TIMER,OnTimer);
			MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown);
			MESSAGE_HANDLER(WM_CLOSE,OnClose);
			MESSAGE_HANDLER(WM_MediaClose,OnMediaClose);
            
			MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove);
			MESSAGE_HANDLER(WM_RBUTTONUP,OnRbuttonUp);
			MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLbuttonDown);
	END_MSG_MAP()

	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	void Render();
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual unsigned char* GetWaitImage(int &len,int curtime);
	virtual unsigned char* GetBkImage(int &len);
	virtual unsigned char* GetCenterLogoImage(int &length);
	virtual void OpenMediaFailure(char* strURL);
	int GetCurTime();
	int Pause();
	int PktSerial();
	void OnDecelerate();
	void OnAccelerate();
	void GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue);
private:
	    //CKKSound m_Sound;
		CSDLSound m_Sound;
		SOUI::CAVMenu *m_pAVMenu;
	    Gdiplus::Bitmap *m_BkGidPulsBitmap;
	    CRender *m_pRender;
	    void OnDraw(HDC& memdc,RECT& rt);
		void ReadTask();
		int LeftWidth;
		int LeftNavigationBarWidth;
		KKPlayer m_PlayerInstance;
		/*********Ä¬ÈÏ±³¾°Í¼Æ¬**********/
		unsigned char* m_pBkImage;

		unsigned char* m_pCenterLogoImage;

		std::vector<SWaitPicInfo*> m_WaitPicList;
		SWaitPicInfo* m_CurWaitPic;
        bool m_bOpen;
		CPoint m_lastPoint;
public:
		WTL::CString m_TabName;
public:
	    MEDIA_INFO GetMediaInfo();
		void SetVolume(long value);
		void AvSeek(int value);
	    int OpenMedia(std::string url,OpenMediaEnum en=OpenMediaEnum::No,std::string FilePath="c:\\xx.mp4");
	    void CloseMedia();
		LRESULT OnMediaClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
		{
            CloseMedia();
			return 1;
		 }
	    LRESULT  OnPaint(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnEraseBkgnd(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnSize(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnTimer(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnSysCommand(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnNcCreate(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnKeyDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnRbuttonUp(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnMouseMove(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnLbuttonDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
};
#endif