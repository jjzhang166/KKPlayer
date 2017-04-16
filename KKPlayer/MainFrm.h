#ifndef CMainFrame_H_
#define CMainFrame_H_
#include "atlmisc.h"
#include "KKPlayer.h"
#include "render/render.h"
#include "../KKPlayerCore/IKKPlayUI.h"
#include <vector>
#include "KKSound.h"
#include "SDLSound.h"

#ifndef LIBKKPLAYER
#include "MainPage/AVMenu.h"
#else
#include "Resource.h"
#endif

struct SWaitPicInfo
{
    unsigned char* Buf;
	int Len;
	short Time;
	int StartTime;
	int Index;
};
#define  WM_MediaClose  WM_USER+100
#define  WM_OpenErr     WM_USER+101


/*************以下用于文件的分片播放***************/
typedef struct AVFILE_SEG_ITEM
{
   short                 SegId;
   int                   segsize;           ///文件大小。
   unsigned  int         milliseconds;      ///文件时间。 
   unsigned  int         seektime;
   char url[1024];        ///文件路径.
   bool                  bIsAd;
   AVFILE_SEG_ITEM*      pre;
   AVFILE_SEG_ITEM*      next;
   AVFILE_SEG_ITEM()
   {
       memset(this,0,sizeof(AVFILE_SEG_ITEM));
   }
}AVFILE_SEG_ITEM;
typedef struct AVFILE_SEGS_INFO
{
   unsigned int            milliseconds;
   unsigned long long      FileSize;
   AVFILE_SEG_ITEM*        pItemHead;
   AVFILE_SEG_ITEM*        pItemTail;
   AVFILE_SEG_ITEM*        pCurItem;
   int                     ItemCount;
   AVFILE_SEGS_INFO()
   {
       memset(this,0,sizeof(AVFILE_SEGS_INFO));
   }
}AVFILE_SEGS_INFO;


//errcode 参考EKKPlayerErr
typedef void (*fpKKPlayerErrNotify)(void *UserData,int errcode);
class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public IKKPlayUI
{
public:
	CMainFrame(bool NeedDel=false);
	~CMainFrame();
protected:
/********** IKKPlayUI实现*************/
	virtual unsigned char* GetErrImage(int &length,int ErrType);
	virtual unsigned char* GetWaitImage(int &len,int curtime);
	virtual unsigned char* GetBkImage(int &len);
	virtual unsigned char* GetCenterLogoImage(int &length);
	virtual void           OpenMediaFailure(char* strURL,EKKPlayerErr err);
	/*******视频流结束调用*******/
	virtual void           AutoMediaCose(void *playerIns,int Stata,int quesize,KKPlayerNextAVInfo &NextInfo);
	virtual void           AVReadOverThNotify(void *playerIns);
	virtual void           AVRender(); 
/********播放器相关操作*********/
public:
	int          GetCurTime();
	int          Pause();
	int          PktSerial();
	void         OnDecelerate();
	void         OnAccelerate();
	void         SetErrNotify(void *UserData,fpKKPlayerErrNotify m_ErrNotify);

	void         GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue);
	int          GetRealtime();
    MEDIA_INFO   GetMediaInfo();
	void         SetVolume(long value);
	void         AvSeek(int value);
    int          OpenMedia(std::string url);
	int          DownMedia(char *KKVURL,bool Down=true);
    void         CloseMedia();
	//全屏
	void         FullScreen();
private:
	void         OnDraw(HDC& memdc,RECT& rt);
private:
    IKKAudio*        m_pSound;
#ifndef LIBKKPLAYER
	SOUI::CAVMenu*   m_pAVMenu;
#endif
	IkkRender*       m_pRender;
	   
	int              LeftWidth;
	int              LeftNavigationBarWidth;
    
	AVFILE_SEGS_INFO m_FileInfos;///文件分片信息
	short            m_nCurSegId;
	unsigned int     m_nMilTimePos;
	KKPlayer*        m_pPlayerInstance;

	KKPlayer*        m_pPlayerInstanceNext;
	unsigned int     m_nPlayerInsCount;///播放器实例个数
	/*********默认背景图片**********/
	unsigned char*   m_pBkImage;

	unsigned char*   m_pCenterLogoImage;
    int              m_CenterLogoLen;

	unsigned char*   m_pErrOpenImage;
	int              m_ErrOpenImgLen;
	std::vector<SWaitPicInfo*>         m_WaitPicList;
	SWaitPicInfo*                      m_CurWaitPic;
    bool                               m_bOpen;
	CPoint                             m_lastPoint;
	UINT                               m_AVwTimerRes;
	UINT                               m_AVtimerID;
    bool                               m_bFullScreen;
	int                                m_nFullLastTick;
	int                                m_nCursorCount;
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	BEGIN_MSG_MAP(CMainFrame)
			MESSAGE_HANDLER(WM_CREATE, OnCreate)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_PAINT,OnPaint);
			MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd);
			MESSAGE_HANDLER(WM_SIZE,OnSize);
			MESSAGE_HANDLER(WM_TIMER,OnTimer);
			MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown);
			MESSAGE_HANDLER(WM_CLOSE,OnClose);
			MESSAGE_HANDLER(WM_MediaClose,OnMediaClose);
			MESSAGE_HANDLER(WM_OpenErr,OnOpenMediaErr);
			MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove);
			MESSAGE_HANDLER(WM_RBUTTONUP,OnRbuttonUp);
			MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLbuttonDown);
	END_MSG_MAP()
protected:
	    void     OnFinalMessage(HWND /*hWnd*/);
	    LRESULT  OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	    LRESULT  OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		LRESULT  OnMediaClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnOpenMediaErr(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
	    LRESULT  OnPaint(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnEraseBkgnd(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnSize(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnTimer(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		
		LRESULT  OnKeyDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnRbuttonUp(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnMouseMove(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
		LRESULT  OnLbuttonDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
private:
	   bool                  m_bNeedDel;
	   fpKKPlayerErrNotify   m_ErrNotify;
	   void*                 m_pErrNotifyUserData;
};
#endif