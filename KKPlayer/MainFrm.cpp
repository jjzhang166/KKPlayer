#include "stdafx.h"
#include<Mmsystem.h>
#include "MainFrm.h"
#include <ObjIdl.h>
#include "KKSound.h"


#ifndef LIBKKPLAYER
#include "MainPage/MainDlg.h"
#include "Tool/cchinesecode.h"
#include "Tool/CFileMgr.h"
extern SOUI::CMainDlg* m_pDlgMain;
#endif

#define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
	(((y) + (1L << 15)) >> 16))
extern CreateRender pfnCreateRender;
extern DelRender pfnDelRender;

std::basic_string<char> g_strModuleFileNameA;
const std::basic_string<char>& XGetModuleFilenameA()
{

	if (g_strModuleFileNameA.empty())
	{
		if(g_strModuleFileNameA.empty())
		{
			char filename[MAX_PATH] = { 0 };
			::GetModuleFileNameA(NULL, filename, _countof(filename));
			g_strModuleFileNameA = filename;
		}
	}
	return g_strModuleFileNameA;
}

std::basic_string<char> GetModulePathA()
{
	std::basic_string<char> strModuleFileName = XGetModuleFilenameA();
	unsigned int index = strModuleFileName.find_last_of("\\");
	if (index != std::string::npos)
	{
		return strModuleFileName.substr(0, index);
	}
	return "";
}

std::basic_string<TCHAR> GetModulePath();

CMainFrame::CMainFrame(bool NeedDel):
m_pBkImage(NULL),m_pCenterLogoImage(NULL),
m_pErrOpenImage(NULL),m_ErrOpenImgLen(NULL)
,m_bFullScreen(false)
,m_nFullLastTick(false)
,m_nCursorCount(0)
,m_ErrNotify(0)
,m_pErrNotifyUserData(0)
,m_bNeedDel(NeedDel)
,m_nCurSegId(0)
,m_nMilTimePos(0)
{
#ifndef LIBKKPLAYER
	m_pAVMenu=NULL;
#endif
	//m_bFullScreen=true;
	m_pSound=NULL;
	m_pPlayerInstance=NULL;
	m_CenterLogoLen=0;
	std::string basePath=GetModulePathA();
	m_bOpen=false;
	int Giftime=120;
	{
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\wait1.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (fp)
		{
			SWaitPicInfo * info =(SWaitPicInfo*)::malloc(sizeof(SWaitPicInfo));
			info->Index=0;
			fseek(fp,0,SEEK_END); //定位到文件末 
			info->Len= ftell(fp); 
			info->Buf = (unsigned char*)::malloc(info->Len);
			memset(info->Buf,0,info->Len);
			info->Time=Giftime;
			fseek(fp,0,SEEK_SET);
			size_t tt=fread(info->Buf,1,info->Len,fp);
			m_WaitPicList.push_back(info);
			fclose(fp);
		}
		
	}

	{
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\wait2.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (fp)
		{
			SWaitPicInfo * info =(SWaitPicInfo*)::malloc(sizeof(SWaitPicInfo));
			info->Index=1;
			fseek(fp,0,SEEK_END); //定位到文件末 
			info->Len= ftell(fp); 
			info->Buf = (unsigned char*)::malloc(info->Len);
		    memset(info->Buf,0,info->Len);
			info->Time=Giftime;
			fseek(fp,0,SEEK_SET);
			size_t tt=fread(info->Buf,1,info->Len,fp);
			m_WaitPicList.push_back(info);
			fclose(fp);
		}
		
	}

	{
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\wait3.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (fp)
		{
			SWaitPicInfo * info =(SWaitPicInfo*)::malloc(sizeof(SWaitPicInfo));
			info->Index=2;
			fseek(fp,0,SEEK_END); //定位到文件末 
			info->Len= ftell(fp); 
			info->Buf = (unsigned char*)::malloc(info->Len);
			memset(info->Buf,0,info->Len);
			info->Time=Giftime;
			fseek(fp,0,SEEK_SET);
			size_t tt=fread(info->Buf,1,info->Len,fp);
			m_WaitPicList.push_back(info);
			fclose(fp);
		}
		
	}

	{
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\wait4.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (fp)
		{
			SWaitPicInfo * info =(SWaitPicInfo*)::malloc(sizeof(SWaitPicInfo));
			info->Index=3;
			fseek(fp,0,SEEK_END); //定位到文件末 
			info->Len= ftell(fp); 
			info->Buf = (unsigned char*)::malloc(info->Len);
			memset(info->Buf,0,info->Len);
			info->Time=Giftime;
			fseek(fp,0,SEEK_SET);
			size_t tt=fread(info->Buf,1,info->Len,fp);
			m_WaitPicList.push_back(info);
			fclose(fp);
		}
		
	}

	{
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\wait5.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (fp)
		{
			SWaitPicInfo * info =(SWaitPicInfo*)::malloc(sizeof(SWaitPicInfo));
			info->Index=4;
			fseek(fp,0,SEEK_END); //定位到文件末 
			info->Len= ftell(fp); 
			info->Buf = (unsigned char*)::malloc(info->Len);
			memset(info->Buf,0,info->Len);
			info->Time=Giftime;
			fseek(fp,0,SEEK_SET);
			size_t tt=fread(info->Buf,1,info->Len,fp);
			m_WaitPicList.push_back(info);
			fclose(fp);
		}
		
	}

	{
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\wait6.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (fp)
		{
			SWaitPicInfo * info =(SWaitPicInfo*)::malloc(sizeof(SWaitPicInfo));
			info->Index=5;
			fseek(fp,0,SEEK_END); //定位到文件末 
			info->Len= ftell(fp); 
			info->Buf = (unsigned char*)::malloc(info->Len);
			memset(info->Buf,0,info->Len);
			info->Time=Giftime;
			fseek(fp,0,SEEK_SET);
			size_t tt=fread(info->Buf,1,info->Len,fp);
			m_WaitPicList.push_back(info);
			fclose(fp);
		}
		
	}
	m_CurWaitPic=NULL;
}

CMainFrame::      ~CMainFrame()
{
    m_pPlayerInstance->CloseMedia();
	pfnDelRender(m_pRender,0);
	m_pRender=NULL;
	m_pSound->CloseAudio();
	delete m_pSound;
	delete m_pPlayerInstance;
}
int               CMainFrame::GetRealtime()
{
	if(m_pPlayerInstance!=NULL)
		return m_pPlayerInstance->GetRealtime();

	return 0;
}

void              CMainFrame::AvSeek(int value)
{
	if(m_FileInfos.ItemCount<2)
	{
	     m_pPlayerInstance->AVSeek(value);
	}else{
		 AVFILE_SEG_ITEM*  pItemHead=m_FileInfos.pItemHead;
		 int seektime=0;
		 while(pItemHead)
		 {
			 seektime+=pItemHead->milliseconds/1000;
			 if(value<seektime){
				short CurSegId=m_pPlayerInstance->GetCurSegId();
				short SegId=m_pPlayerInstance->GetSegId();
                int va=seektime-value;
				if(CurSegId!=SegId||SegId!=pItemHead->SegId ){
					m_pPlayerInstance->AVSeek(va,pItemHead->SegId);
				}else{
				    m_pPlayerInstance->AVSeek(va);
				}
				break;
			 }
			 pItemHead=m_FileInfos.pItemHead->next;
		 }
	}
}
void              CMainFrame::SetVolume(long value)
{
	m_pPlayerInstance->SetVolume(value);
}
bool              CMainFrame::GetMediaInfo(MEDIA_INFO& info)
{

   if(m_pPlayerInstance->GetMediaInfo(info)){
	   m_nMilTimePos=0;
	   info.TotalTime=m_FileInfos.milliseconds/1000;
	   if(m_FileInfos.ItemCount>1)
	   {
		   AVFILE_SEG_ITEM* pItemHead=m_FileInfos.pItemHead;
		   m_nCurSegId=info.SegId;
		   m_nMilTimePos=0;
		   while(pItemHead){
			   if(pItemHead->SegId<info.SegId)
		           m_nMilTimePos=m_nMilTimePos+(m_FileInfos.pCurItem->pre->milliseconds/1000); 
			   
			   if(pItemHead->SegId==info.SegId)
				    break;
			   pItemHead=pItemHead->next;
		   }
	   }
	   info.CurTime+=m_nMilTimePos;
	   return true;
   }
   
   return 	false;
}
int               CMainFrame::GetCurTime()
{
	return m_pPlayerInstance->GetCurTime();
}




int               CMainFrame::DownMedia(char *KKVURL,bool Down)
{
	if(m_pPlayerInstance!=NULL)
	{
		KKPluginInfo KKPl;
		memset(&KKPl,0,sizeof(KKPl));
		std::string urlx=KKVURL;
        int ret=m_pPlayerInstance->KKProtocolAnalyze(KKVURL,KKPl);
		if(ret==1){
            if(KKPl.KKDownAVFile!=NULL){
				 //kkv:e8a486a4e28480ad18bd5041c2ad34fa|mp4.cc
				 CAVInfoManage* m_pAVInfomanage=CAVInfoManage::GetInance();
				 if(m_pAVInfomanage!=NULL){

					 char *strInfo=KKVURL;
					 char* alias="xx";
					 char *category="xx";
					 int FileSize=-1;
					 int AcSize=0;
					 int DownOk=0;
					 m_pAVInfomanage->UpdateDownAVinfo(strInfo,alias,category,FileSize,AcSize,DownOk);
					
				 }
				 if(Down)
				 {
                     KKPl.KKDownAVFile(KKVURL);
					 return 1;
				 }
                   
				 else
					 return 2;
			}
		}
	}
	
    return -1;
}
int               CMainFrame::OpenMedia(std::string url)
{
	RECT rt;
	::GetClientRect(m_hWnd,&rt);
	 m_pRender->resize(rt.right-rt.left,rt.bottom-rt.top);
	 if(m_pRender!=NULL)
	 {
		   m_pRender->ShowErrPic(false);
	 }
	 int  ret=0;

	 //if(0)
	 {
		 AVFILE_SEGS_INFO &infos=m_FileInfos;
		 int SegId=0;
		 AVFILE_SEG_ITEM *Seg1= new AVFILE_SEG_ITEM();
		 Seg1->milliseconds=194397;
		 Seg1->segsize=25072982;
		 Seg1->SegId=SegId++;
		 strcpy(Seg1->url,"D:/avseg/1.flv");
		 infos.FileSize+=Seg1->segsize;
		 infos.milliseconds+=Seg1->milliseconds;
		 infos.pCurItem= Seg1;
		 infos.ItemCount++;


		 AVFILE_SEG_ITEM *Seg2= new AVFILE_SEG_ITEM();
		 Seg2->milliseconds=197781;
		 Seg2->segsize=32653311;
		 Seg2->SegId=SegId++;
		 strcpy(Seg2->url,"D:/avseg/2.flv");
		 infos.FileSize+=Seg2->segsize;
		 infos.milliseconds+=Seg2->milliseconds;
		 Seg1->next=Seg2;
		 Seg2->pre=Seg1;
		 infos.ItemCount++;

		 AVFILE_SEG_ITEM *Seg3= new AVFILE_SEG_ITEM();
		 Seg3->milliseconds=180514;
		 Seg3->segsize=29149088;
		 Seg3->SegId=SegId++;
		 strcpy(Seg3->url,"D:/avseg/3.flv");
		 infos.FileSize+=Seg3->segsize;
		 infos.milliseconds+=Seg3->milliseconds;
		 Seg2->next=Seg3;
		 Seg3->pre=Seg2;
		 infos.ItemCount++;

		 AVFILE_SEG_ITEM *Seg4= new AVFILE_SEG_ITEM();
		 Seg4->milliseconds=203870;
		 Seg4->segsize=29475132;
		 Seg4->SegId=SegId++;
		 strcpy(Seg4->url,"D:/avseg/4.flv");
		 infos.FileSize+=Seg4->segsize;
		 infos.milliseconds+=Seg4->milliseconds;
		 Seg3->next=Seg4;
		 Seg4->pre=Seg3;
		 infos.ItemCount++;

		 infos.pItemHead=Seg1;
		 infos.pItemTail=Seg4;
	 }

	
	
	ret=m_pPlayerInstance->OpenMedia((char*)url.c_str());//,"-pause");
	if(ret>=0){
         m_bOpen=true;
		 char abcd[1024]="";
		 strcpy(abcd,(char*)url.c_str());
		 int Retx= DownMedia(abcd,false);
		 if(Retx==2)
		 {
		   return Retx;
		 }
	}
	m_nCurSegId=0;
	m_nMilTimePos=0;
	return ret;
}
void              CMainFrame::CloseMedia()
{
   m_pPlayerInstance->CloseMedia();
   m_bOpen=false;
   m_nCurSegId=0;
   m_nMilTimePos=0;

   AVFILE_SEG_ITEM* item=m_FileInfos.pItemHead;
   while(item!=0)
   {
	   AVFILE_SEG_ITEM* item2=item->next;
	   delete item;
	   item=item2;
   }
}
void              CMainFrame::FullScreen()
{
	if(m_bFullScreen){
		m_bFullScreen=false;
	}else{
		m_nFullLastTick=::GetTickCount();
		m_bFullScreen=true;
		m_nCursorCount=0;
		
	}/**/
}

int               CMainFrame::PktSerial()
{
  return m_pPlayerInstance->GetPktSerial();
}
void              CMainFrame::OnDecelerate()
{
    m_pPlayerInstance->OnDecelerate();
    int Rate=m_pPlayerInstance->GetAVRate();
}
void              CMainFrame::OnAccelerate()
{
   m_pPlayerInstance->OnAccelerate();
   int Rate=m_pPlayerInstance->GetAVRate();
}
int               CMainFrame::Pause()
{
	m_pPlayerInstance->Pause();
	return 0;
}


LRESULT           CMainFrame::OnMediaClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
            CloseMedia();
			return 1;
}
void              CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUsers,DWORD dw1,DWORD dw2)
{
          CMainFrame *Pts=(CMainFrame *)dwUsers;
		 // Pts->Render();
}
void              CMainFrame::OnFinalMessage(HWND /*hWnd*/)
 {
	 if(m_bNeedDel)
     delete this;
 }
LRESULT           CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	

	m_pSound =new CSDLSound();//
	m_pSound->SetWindowHAND((int)m_hWnd);
	m_nPlayerInsCount=1;
	m_pPlayerInstance = new KKPlayer(this,m_pSound);
	
	//m_pPlayerInstance = new KKPlayer(NULL,m_pSound);
	

	m_pRender=NULL;
	
	this->SetFocus();
	this->EnableWindow(true);
	::SetTimer(this->m_hWnd,10010,40,NULL);

	char Out=1;
	m_pRender=(IkkRender*) pfnCreateRender(m_hWnd,&Out);
	if(Out==0)
	{
         m_pPlayerInstance->SetBGRA();
	}
	m_pSound->SetWindowHAND((int)m_hWnd);


	 //m_AVwTimerRes=0;
	 //m_AVtimerID=0;
	 //TIMECAPS ts;
	 //////确定多媒体定时器提供的最大和最小定时器事件周期
	 //::timeGetDevCaps(&ts, sizeof(ts));

	 //m_AVwTimerRes=1;
	 //////建立最小定时器精度
	 //timeBeginPeriod(m_AVwTimerRes);

	 //////启动定时器事件，设置定时周期为100ms，分辨率是10毫秒
	 //m_AVtimerID = timeSetEvent(10,1,TimeProc,(DWORD)this,TIME_PERIODIC);
	 //
	//OpenMedia("rtmp://live.hkstv.hk.lxdns.com/live/hks live=1");
    return 0;
}

LRESULT           CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	
	if( m_bOpen)
	{
		m_bOpen=false;
		m_pPlayerInstance->CloseMedia();
	}
	//DWORD Id=GetCurrentThreadId();
	//::PostThreadMessage(Id, WM_QUIT, 0, 0);
	bHandled = TRUE;
	return 1;
}


LRESULT           CMainFrame::OnPaint(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	 
	 bHandled=true;
	 if(!m_bOpen)
	 {
		 PAINTSTRUCT ps = { 0 };
		 ::BeginPaint(m_hWnd, &ps);
		 ::EndPaint(m_hWnd, &ps);
		 m_pPlayerInstance->RenderImage(m_pRender,true);
	 }else if(0){
		 PAINTSTRUCT ps = { 0 };
		 ::BeginPaint(m_hWnd, &ps);
		 HDC MemDC=::CreateCompatibleDC(ps.hdc);
		 RECT rcWindow;
		 ::GetClientRect(m_hWnd,&rcWindow);
		 HBITMAP bmp;
		 bmp=::CreateCompatibleBitmap(ps.hdc,rcWindow.right-rcWindow.left,rcWindow.bottom-rcWindow.top);
		 ::SelectObject(MemDC,bmp);
		 ::SetBkColor(MemDC,RGB(0,0,0));
		 ::SetBkMode(MemDC,TRANSPARENT);

#ifdef QY_GDI
		 OnDraw(MemDC,rcWindow);
#endif

         ::BitBlt(ps.hdc,0,0,rcWindow.right-rcWindow.left,rcWindow.bottom-rcWindow.top,MemDC,0,0,SRCCOPY);
		 ::DeleteObject(bmp);
		 ::DeleteDC(MemDC);
		
		 ::EndPaint(m_hWnd, &ps);
	 }else{
		  PAINTSTRUCT ps = { 0 };
		  ::BeginPaint(m_hWnd, &ps);
		  ::EndPaint(m_hWnd, &ps);
		  m_pPlayerInstance->RenderImage(m_pRender,true);
	 }
	
	return 1;
}

void              CMainFrame::OnDraw(HDC& memdc,RECT& rt)
{
     HBRUSH m_SelectDotHbr=CreateSolidBrush(RGB(86, 147, 44));
	::FillRect(memdc,&rt,m_SelectDotHbr);
	::DeleteObject(m_SelectDotHbr);

	#ifdef WIN32_KK
	    m_pPlayerInstance->OnDrawImageByDc(memdc);
    #endif
}

LRESULT           CMainFrame::OnSize(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{ 	
     ::DefWindowProc(this->m_hWnd,uMsg, wParam, lParam);
	 int h=HIWORD(lParam);
	 int w=LOWORD(lParam);
	
	 if(m_pRender!=NULL)
	 {
          m_pRender->resize(w,h);
	 }
	
	 return 1;
}
LRESULT           CMainFrame::OnEraseBkgnd(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
    bHandled=true;
	return 1;
}
LRESULT           CMainFrame::OnClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	KillTimer(10010);
	if( m_bOpen)
	{
		m_bOpen=false;
          m_pPlayerInstance->CloseMedia();
	}
	//::MessageBox(0,L"CLOSE",L"",0);
	CFrameWindowImpl::DefWindowProc(uMsg, wParam,lParam);
	bHandled=true;

	return 1;
}
LRESULT           CMainFrame::OnKeyDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	long ll=0;
	switch(wParam)
	{
	    case VK_UP:
                           ll=-1;//m_pPlayerInstance->GetVolume()-10;
						   m_pPlayerInstance->SetVolume(ll);
		        	       break;
		case VK_DOWN:
			               break;
		case VK_LEFT:
			              m_pPlayerInstance->KKSeek(SeekEnum::Left,-60);
			               break;
		case 80:           /******P键********/
			               m_pPlayerInstance->Pause();
						   break;
		case VK_RIGHT:
			               m_pPlayerInstance->KKSeek(SeekEnum::Right,60);
			               break;
		
	}
	
	return 1;
}

extern HINSTANCE GhInstance;
LRESULT           CMainFrame::OnTimer(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	if(m_bFullScreen){
		if(::GetTickCount()-m_nFullLastTick>2000)
		{
			::SetCursor(NULL);//
			//::SetCursor(LoadCursor (GhInstance,MAKEINTRESOURCE(IDC_CURSOR1)));//
			 m_nFullLastTick=::GetTickCount();
		}
	}
	if(wParam==10010)
	{
          AVRender();
	}else{
	     
	}
	bHandled=true;
	return 1;
}


LRESULT           CMainFrame::OnLbuttonDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	bHandled=true;
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);
	
#ifndef LIBKKPLAYER
	::PostMessage(::GetParent(m_hWnd) ,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(xPos, yPos)); 

	if(!m_pDlgMain->GetScreenModel())
	{
	   m_pDlgMain->ShowMiniUI(false);
	}
#endif
	return 1;
}
LRESULT           CMainFrame::OnRbuttonUp(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	bHandled=true;
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);

	#ifndef LIBKKPLAYER
	RECT rt;
	::GetWindowRect(m_hWnd,&rt);
	xPos+=rt.left;
	yPos+=rt.top;

	SOUI::SMenuEx me;
	BOOL xx=me.LoadMenu(_T("SMENUEX:avmenuex"));


	if(m_pDlgMain->GetFullScreen()||m_bOpen)
	{
		 SOUI:: SWindow *pItem=(SOUI:: SWindow *) me.GetMenuItem(3);
		 if(m_pDlgMain->GetFullScreen())
		 pItem->SetAttribute(_T("check"),_T("1"));
		 pItem->SetAttribute(_T("enable"),_T("1"));
		 pItem->SetAttribute(_T("colorText"),_T("#000000"));
	}else {
		SOUI:: SWindow *pItem=(SOUI:: SWindow *) me.GetMenuItem(3);
		pItem->SetAttribute(_T("check"),_T("0"));
		pItem->SetAttribute(_T("enable"),_T("0"));
		pItem->SetAttribute(_T("colorText"),_T("#C5C5C7"));
	}

	if(m_pDlgMain->GetScreenModel())
	{
		{
			SOUI:: SWindow *pItem=(SOUI:: SWindow *) me.GetMenuItem(4);
			pItem->SetAttribute(_T("check"),_T("1"));
		}
		{
			SOUI:: SWindow *pItem=(SOUI:: SWindow *) me.GetMenuItem(5);
			pItem->SetAttribute(_T("check"),_T("0"));
		}  
	}else{
		{
			SOUI:: SWindow *pItem=(SOUI:: SWindow *) me.GetMenuItem(4);
			pItem->SetAttribute(_T("check"),_T("0"));
		}
		{
			SOUI:: SWindow *pItem=(SOUI:: SWindow *) me.GetMenuItem(5);
			pItem->SetAttribute(_T("check"),_T("1"));
		}  
	}
	me.TrackPopupMenu(0,xPos,yPos,::GetParent(m_hWnd));
#endif
	return 1;
}

LRESULT           CMainFrame::OnMouseMove(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{

   bHandled=true;
   int xPos = GET_X_LPARAM(lParam); 
   int yPos = GET_Y_LPARAM(lParam);
   
   #ifndef LIBKKPLAYER
   if(m_bFullScreen&&m_lastPoint.x!=xPos&&m_lastPoint.y!=yPos){
	   m_nFullLastTick=::GetTickCount();
	   ::SetCursor(LoadCursor(NULL,IDC_ARROW));
	   ::OutputDebugStringA("鼠标移动全部 \n");
   }
	//迷你模式
	if(!m_pDlgMain->GetScreenModel()&&xPos!=m_lastPoint.x&&yPos!=m_lastPoint.y&&xPos>1&&yPos>1)
	{
		int ll=0x8000 &GetAsyncKeyState(VK_LBUTTON);
		if(!ll)
		{
			m_pDlgMain->ShowMiniUI(true);
		}else
		{
			m_pDlgMain->ShowMiniUI(false);
		}
	}
#endif
	m_lastPoint.x=xPos;
	m_lastPoint.y=yPos;
	return 0;
}

LRESULT           CMainFrame::OnOpenMediaErr(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
   bHandled=true;
  /* char *err=(char*)wParam;
   ::MessageBoxA(m_hWnd,err,"错误",MB_ICONHAND);
   ::free(err);
   CloseMedia();*/
   return 1;
}




void              CMainFrame::SetErrNotify(void *UserData,fpKKPlayerErrNotify ErrNotify)
{
    m_ErrNotify=ErrNotify;
	m_pErrNotifyUserData=UserData;
}
void              CMainFrame::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
	m_pPlayerInstance->GetAVHistoryInfo(slQue);
}

unsigned char*    CMainFrame::GetErrImage(int &length,int ErrType)
{
	if(m_pErrOpenImage==NULL)
	{
		std::string basePath=GetModulePathA();
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\ErrOpen.png";
		fp=fopen(PicPath.c_str(),"rb");
		if (!fp)
		{
			return NULL;
		}
		fseek(fp,0,SEEK_END); //定位到文件末 
		length= ftell(fp); 

		m_pErrOpenImage = (unsigned char*)::malloc(length);
		memset(m_pErrOpenImage,0,length);
		fseek(fp,0,SEEK_SET);
		size_t tt=fread(m_pErrOpenImage,1,length,fp);
		fclose(fp);
		m_ErrOpenImgLen=length;
	}
	length=m_ErrOpenImgLen;
	return m_pErrOpenImage;
}
unsigned char*    CMainFrame::GetWaitImage(int &len,int curtime)
{
	if(m_WaitPicList.size()<=0)
		return NULL;
	int t=::GetTickCount();
	if(m_CurWaitPic==NULL)
	{
		m_CurWaitPic=m_WaitPicList[0];
		m_CurWaitPic->StartTime=t;
	}else 
	{
           if(m_CurWaitPic->Time<(t-m_CurWaitPic->StartTime))
		   {
			   if(m_CurWaitPic->Index>=5)
			   {
                    m_CurWaitPic=m_WaitPicList[0];
					m_CurWaitPic->StartTime=t;
			   }else
			   {
				   m_CurWaitPic=m_WaitPicList[m_CurWaitPic->Index+1];
				   m_CurWaitPic->StartTime=t;
			   }
		   }
	}
	len=m_CurWaitPic->Len;
	curtime=m_CurWaitPic->StartTime;
	return m_CurWaitPic->Buf;
}



unsigned char*    CMainFrame::GetCenterLogoImage(int &len)
 {
         if(m_pCenterLogoImage==NULL)
		 {
			 std::string basePath=GetModulePathA();
			 FILE*fp=NULL;
			 std::string PicPath=basePath;
			 PicPath+="\\Skin\\mediaCtrl_Img.png";
			 fp=fopen(PicPath.c_str(),"rb");
			 if (!fp)
			 {
				 return NULL;
			 }
			 fseek(fp,0,SEEK_END); //定位到文件末 
			 len = ftell(fp); 

			 m_pCenterLogoImage = (unsigned char*)::malloc(len);
			 memset(m_pCenterLogoImage,0,len);
			 fseek(fp,0,SEEK_SET);
			 size_t tt=fread(m_pCenterLogoImage,1,len,fp);
			 fclose(fp);
			m_pCenterLogoImageLen=len;
			len=m_pCenterLogoImageLen;
			m_CenterLogoLen=len;
		 }
		 len=m_CenterLogoLen;
		 return m_pCenterLogoImage;
 }
unsigned char*    CMainFrame::GetBkImage(int &len)
{
	
	if(m_pBkImage==NULL)
	{
		std::string basePath=GetModulePathA();
		FILE*fp=NULL;
		std::string PicPath=basePath;
		PicPath+="\\Skin\\Bk.jpg";
		fp=fopen(PicPath.c_str(),"rb");
		if (!fp)
		{
           return NULL;
		}
		fseek(fp,0,SEEK_END); //定位到文件末 
		len = ftell(fp); 

		m_pBkImage = (unsigned char*)::malloc(len);
		memset(m_pBkImage,0,len);
		fseek(fp,0,SEEK_SET);
		size_t tt=fread(m_pBkImage,1,len,fp);
		fclose(fp);
		m_pBkImageLen=len;
	}
	len=m_pBkImageLen;
	return m_pBkImage;
}



void              CMainFrame::OpenMediaFailure(char* strURL,EKKPlayerErr err)
{
     int length=0;
	 unsigned char* img=GetErrImage(length,0);
	 if(err==KKOpenUrlOkFailure&&img!=NULL&&length>0&&m_pRender!=NULL)
	 {
		 m_pRender->SetErrPic(img,length);
		 m_pRender->ShowErrPic(true);
	 }
	 if(m_ErrNotify!=NULL)
	 {
	    m_ErrNotify(m_pErrNotifyUserData,err);
	 }
}
void              CMainFrame::AutoMediaCose(void *playerIns,int Stata,int quesize,KKPlayerNextAVInfo &NextInfo)
{

	if(Stata==-109)//管道已结束。
	{
		int length=0;
		unsigned char* img=GetErrImage(length,0);
		if(img!=NULL&&length>0&&m_pRender!=NULL)
		{
			m_pRender->SetErrPic(img,length);
			m_pRender->ShowErrPic(true);
		}
	}

	if(m_FileInfos.ItemCount>1)
	 { 
		 if(m_pPlayerInstance==(KKPlayer*)playerIns){
			 
			 if(m_FileInfos.pCurItem!=NULL&&m_FileInfos.pCurItem->next!=NULL&&NextInfo.SegId==-1){
					 m_FileInfos.pCurItem=m_FileInfos.pCurItem->next;	
					 memset(&NextInfo,0,sizeof(NextInfo));
					 strcpy(NextInfo.url, m_FileInfos.pCurItem->url);
					 NextInfo.SegId=m_FileInfos.pCurItem->SegId;
					 NextInfo.NeedRead=true;
					 return  ;
			 }else if(NextInfo.SegId>-1){
				    int timepos=0;
					AVFILE_SEG_ITEM* pItemHead=m_FileInfos.pItemHead;
					while(pItemHead!=NULL){
						if(pItemHead->SegId==NextInfo.SegId){
							  memset(&NextInfo,0,sizeof(NextInfo));
					          strcpy(NextInfo.url, pItemHead->url);
						      NextInfo.SegId=pItemHead->SegId;
					          NextInfo.NeedRead=true;
							  m_FileInfos.pCurItem=pItemHead;
							  return;
						}
						pItemHead=pItemHead->next;
					}
			 }
		 }
	 }
	 return;
}
void              CMainFrame::AVReadOverThNotify(void *playerIns)
{

}
void              CMainFrame::AVRender()
{
                  m_pPlayerInstance->RenderImage(m_pRender,false);
}
