#ifndef CMainDlg_H_
#define CMainDlg_H_
#include "../stdafx.h"
#include "AVMiniTittle.h"
#include "AVMiniBottom.h"

#include "AVNetDown.h"
#include "AVDownManageDlg.h"
#include "../WHRoundRectFrameHelper.h"
namespace SOUI
{
class CMainDlg : public SHostWnd,public CWHRoundRectFrameHelper<CMainDlg>
{
	public:
		CMainDlg();
		~CMainDlg();

		void OnClose();
		
		void OnMaximize();
		void OnRestore()
		{

			SendMessage(WM_SYSCOMMAND,SC_RESTORE);
			m_bFullScreen=false;
		}
		void OnMinimize()
		{
			ShowMiniUI(false);
			SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
			m_bFullScreen=false;
		}
		void OnRButtonUp(UINT nFlags, CPoint point);
 //virtual BOOL _HandleEvent(SOUI::EventArgs *pEvt){return FALSE;}
		void OnSize(UINT nType, CSize size);
		void OnBtnMsgBox();
	    
		int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		void OnShowWindow(BOOL bShow, UINT nStatus);
		BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

        void OnFolder();
		void OnFileList();
		void OnAVPlay();
		
		void OnTimer(char cTimerID);
		bool OnSliderVideo(EventArgs *pEvtBase);
		bool OnSliderAudio(EventArgs *pEvtBase);
		//打开音视频文件
        int  OpenMedia(char *KKVURL);
		/*********下载文件***************/
		int  DownMedia(char *path);
		void OnDecelerate();
		void OnAccelerate();

		//DUI菜单响应函数
		void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);

		void ShowMiniUI(bool show);
		//全屏
		bool GetFullScreen()
		{
             return m_bFullScreen;
		}
		
		bool GetScreenModel()
		{
			return m_bScreenNormal;
		}
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		void SetSeekControlValue(int Value,int CacheValue);
		void SetSeekCacheControlValue(int CacheValue);
		void SetSeekControlRange(int Value);
		void OnShowWinPlay();
		void OnShowWinRes();

		void OnWebResSearch();
		void WinTabShow(int Tab);
		void ShowAVDownManageDlg();
		int m_MenuId;
	protected:
		EVENT_MAP_BEGIN()
			EVENT_NAME_COMMAND(L"btn_TopSearch",OnWebResSearch)
			EVENT_NAME_COMMAND(L"btn_TopPlay_Opt",OnShowWinPlay)
			EVENT_NAME_COMMAND(L"btn_TopRes_Opt",OnShowWinRes)
			EVENT_NAME_COMMAND(L"btn_close",OnClose)
			EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
			EVENT_NAME_COMMAND(L"btn_max",OnMaximize)
			EVENT_NAME_COMMAND(L"btn_restore",OnRestore)
			EVENT_NAME_COMMAND(L"btn_msgbox",OnBtnMsgBox)
			EVENT_NAME_COMMAND(L"AVFolderBtn",OnFolder)
			EVENT_NAME_COMMAND(L"AVFileListBtn",OnFileList)
			EVENT_NAME_COMMAND(L"AVPlayBtn",OnAVPlay)
			EVENT_NAME_COMMAND(L"BtnDecelerate",OnDecelerate)
			EVENT_NAME_COMMAND(L"BtnAccelerate",OnAccelerate)
			EVENT_NAME_HANDLER(L"slider_video_Seek",EventSliderPos::EventID,OnSliderVideo)
			EVENT_NAME_HANDLER(L"AvAudio",EventSliderPos::EventID,OnSliderAudio)
		EVENT_MAP_END()	

		BEGIN_MSG_MAP_EX(CMainDlg)
		    CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CMainDlg>)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_TIMER_EX(OnTimer) 
			MSG_WM_CLOSE(OnClose)
			MSG_WM_SIZE(OnSize)
			MSG_WM_SHOWWINDOW(OnShowWindow)
			MSG_WM_COMMAND(OnCommand)
			MSG_WM_RBUTTONUP(OnRButtonUp)

            MSG_WM_KEYDOWN(OnKeyDown) 
			CHAIN_MSG_MAP(SHostWnd)
			REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
	private:
		
		void UpdateAVSeek();
		void FullScreen();
		void ScreenNormal();
		void ScreenMini();

		void showAV_List();
		void AV_Title();
		BOOL			m_bLayoutInited;
		int             m_PlayerState;
		int             m_CumrTime;
		int             m_lastSeekTime;
		int             m_SeekTimer;
		//包序列号
		int             m_serial;

		//全屏
		bool            m_bFullScreen;
		bool            m_bScreenNormal;
        //得到
		int             m_nDownSpeed;
		CAVMiniTittle   m_AVMiniTittle;
		CAVMiniBottom   m_AVMiniBottom;
		CAVNetDown      m_KKVNetDownDlg;
		int                m_showAV_List;
		std::string        m_urlStr;
	};
}
#endif