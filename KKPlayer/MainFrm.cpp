#include "stdafx.h"
#include<Mmsystem.h>
#include "MainFrm.h"
#include <ObjIdl.h>
#include "KKSound.h"
#include "json/json.h"


#ifdef _DEBUG
#pragma comment (lib,"../Debug/libx86/KKPlayerCored.lib")
#pragma comment (lib,"../Debug/libx86/libmfxd.lib")
#pragma comment (lib,"../WinUI/lib/debug/souid.lib")
#pragma comment (lib,"../WinUI/lib/debug/utilitiesd.lib")
#pragma comment (lib,"../Debug/jsoncppd.lib")
#pragma comment (lib,"../Debug/libx86/librtmpd.lib")
#else
#pragma comment (lib,"../Release/libx86/KKPlayerCore.lib")
#pragma comment (lib,"../Release/libx86/libmfx.lib")
#pragma comment (lib,"../WinUI/lib/Release/soui.lib")
#pragma comment (lib,"../WinUI/lib/Release/utilities.lib")
#pragma comment (lib,"../Release/jsoncpp.lib")
#pragma comment (lib,"../Release/libx86/librtmp.lib")
#endif

#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib,"winmm.lib")
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

CMainFrame::CMainFrame(bool yuv420p,bool NeedDel):
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
,m_nTipTick(0)
,m_nSeekTip(0)
,m_bYuv420p(yuv420p)
,m_nDuiDraw(0)
,m_pDuiDrawCall(0)
,m_pRenderUserData(0)
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
	if(m_pPlayerInstance!=NULL)
       m_pPlayerInstance->CloseMedia();
	if(pfnDelRender)
	   pfnDelRender(m_pRender,0);
	m_pRender=NULL;
	if(m_pSound)
	m_pSound->CloseAudio();
	delete m_pSound;
	delete m_pPlayerInstance;

}
///设置成无窗口渲染
void CMainFrame::SetDuiDraw(HWND h,fpRenderImgCall DuiDrawCall,void *RenderUserData)
{
   m_hWnd=h;
   m_nDuiDraw=1;
   BOOL Ok=false;
   m_pDuiDrawCall=DuiDrawCall;
   m_pRenderUserData=RenderUserData;
   this->OnCreate(0,0,0,Ok);
}
int   CMainFrame::GetDuiDraw()
{
	return m_nDuiDraw;
}
int               CMainFrame::GetRealtime()
{
	if(m_pPlayerInstance!=NULL)
		return m_pPlayerInstance->GetRealtime();

	return 0;
}

void              CMainFrame::AvSeek(int value)
{ 
	if(m_pPlayerInstance==NULL)
		return;
	int seektime=0;
	if(m_FileInfos.ItemCount<2)
	{
	     m_pPlayerInstance->AVSeek(value);
	}else{
		 m_FileSegLock.Lock();
		 AVFILE_SEG_ITEM*  pItemHead=m_FileInfos.pItemHead;
		
		 while(pItemHead)
		 {
			 seektime+=pItemHead->milliseconds/1000;
			 if(value<seektime){
				short CurSegId=m_pPlayerInstance->GetCurSegId();
				short SegId=m_pPlayerInstance->GetSegId();
                int va=seektime-value;
				if(CurSegId!=SegId||SegId!=pItemHead->SegId ){
					///发生了切片强制清空
					/*if(abs((pItemHead->SegId-SegId))>0)
					{
						m_pPlayerInstance->ForceFlushQue();
					}*/
					m_pPlayerInstance->AVSeek(va,pItemHead->SegId);
				}else{
				    m_pPlayerInstance->AVSeek(va);
				}
				break;
			 }
			 pItemHead=pItemHead->next;
		 }
		 m_FileSegLock.Unlock();
	}
}
void              CMainFrame::SetVolume(long value,bool tip)
{
	if(value>1000)
		value=1000;
	if(m_pRender!=NULL&&tip){
		char Tip[1024]="";
		sprintf(Tip,"音量：%d%%",value);
		m_nTipTick=GetTickCount();
		m_pRender->SetLeftPicStr(Tip);
	}
	if(m_pPlayerInstance!=NULL)
	       m_pPlayerInstance->SetVolume(value);
}
long              CMainFrame::GetVolume()
{
	if(m_pPlayerInstance==NULL)
		return 0;
   return m_pPlayerInstance->GetVolume();
}
bool              CMainFrame::GetMediaInfo(MEDIA_INFO& info)
{

   if(m_pPlayerInstance!=NULL&&m_pPlayerInstance->GetMediaInfo(info)){
	   m_nMilTimePos=0;
	   
	   if(m_FileInfos.ItemCount>1)
	   {
		   info.TotalTime=m_FileInfos.milliseconds/1000;
		   AVFILE_SEG_ITEM* pItemHead=m_FileInfos.pItemHead;
		   m_nCurSegId=info.SegId;
		   m_nMilTimePos=0;
		   while(pItemHead){
			   if(pItemHead->SegId<info.SegId)
		           m_nMilTimePos=m_nMilTimePos+pItemHead->milliseconds/1000; 
			   
			   if(pItemHead->SegId==info.SegId)
				    break;
			   pItemHead=pItemHead->next;
		   }
	   }
	   info.CurTime+=m_nMilTimePos;

	    if(m_nSeekTip)
		{
			int h=(info.CurTime/(60*60));
			int m=(info.CurTime%(60*60))/(60);
			int s=((info.CurTime%(60*60))%(60)); 
			char strtmp[1024]="";
			std::string CurTimeStr;
			if(h<10){
				sprintf(strtmp,"0%d:",h);
				CurTimeStr=strtmp;
			}
			else{
				sprintf(strtmp,"%d:",h);
				CurTimeStr=strtmp;
			}
			if(m<10){
				sprintf(strtmp,"0%d:",m);
				CurTimeStr+=strtmp;
			}
			else{				  
				sprintf(strtmp,"%d:",m);
				CurTimeStr+=strtmp;
			}

			if(s<10){
				sprintf(strtmp,"0%d",s);
				CurTimeStr+=strtmp;
			}
			else{
				sprintf(strtmp,"%d",s);
				CurTimeStr+=strtmp;
			}
			if(m_nSeekTip==1)
				sprintf(strtmp,"快退  %s",CurTimeStr.c_str());
			else if(m_nSeekTip==2)
			   sprintf(strtmp,"快进  %s",CurTimeStr.c_str());
			
			m_nTipTick=::GetTickCount();
			m_pRender->SetLeftPicStr(strtmp);
			m_nSeekTip=0;
		}
	   return true;
   }
   
   return 	false;
}



int               CMainFrame::DownMedia(char *KKVURL,bool Down)
{
	if(m_pPlayerInstance!=NULL)
	{
		
	}
	
    return -1;
}
int               CMainFrame::OpenMedia(std::string url)
{
	if(!m_pPlayerInstance)
		return -2;
	RECT rt;
	::GetClientRect(m_hWnd,&rt);
	 m_pRender->resize(rt.right-rt.left,rt.bottom-rt.top);
	 if(m_pRender!=NULL)
	 {
		   m_pRender->ShowErrPic(false);
	 }
	 int  ret=0;

	 if(0)
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
		/* char abcd[1024]="";
		 strcpy(abcd,(char*)url.c_str());
		 int Retx= DownMedia(abcd,false);
		 if(Retx==2)
		 {
		   return Retx;
		 }*/
	}
	m_nSeekTip=0;
	m_nCurSegId=0;
	m_nMilTimePos=0;
	return ret;
}
void              CMainFrame::CloseMedia()
{
	if(!m_pPlayerInstance)
		return;
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
   m_FileInfos.pItemHead=NULL;
   m_FileInfos.ItemCount=0;
   m_FileInfos.milliseconds=0;
   m_FileInfos.pItemTail=0;
   m_FileInfos.pCurItem=0;
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
	if(m_pPlayerInstance==NULL)
		return 0;
  return m_pPlayerInstance->GetPktSerial();
}
void              CMainFrame::OnDecelerate()
{
	if(m_pPlayerInstance==NULL)
		return ;
    m_pPlayerInstance->OnDecelerate();
    int Rate=m_pPlayerInstance->GetAVRate();
}
void              CMainFrame::OnAccelerate()
{
   if(m_pPlayerInstance==NULL)
		return;
   m_pPlayerInstance->OnAccelerate();
   int Rate=m_pPlayerInstance->GetAVRate();
}
int               CMainFrame::Pause()
{
   if(m_pPlayerInstance==NULL)
		return 0;
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

int SetHardCtx(void* d3d,void* dev,int ver);
LRESULT           CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	m_pSound =new CSDLSound();//
	m_pSound->SetWindowHAND((int)m_hWnd);
	m_nPlayerInsCount=1;
	m_pPlayerInstance = new KKPlayer(this,m_pSound);
	m_pRender=NULL;
	char Out=1;
	HWND hh=m_hWnd;
	if(m_nDuiDraw){
		m_bYuv420p=false;
		hh=0;
	}else{
	    this->SetFocus();
		this->EnableWindow(true);
		::SetTimer(this->m_hWnd,10010,50,NULL); 
	}
	
	if(!m_bYuv420p){
	    m_pPlayerInstance->SetBGRA();
		 Out=0;
	}
	m_pRender=(IkkRender*) pfnCreateRender(hh,&Out);
	if(Out==0){
         m_pPlayerInstance->SetBGRA();
	}
	else{
	    void* d3d=NULL;void* dev=NULL;int ver=0;
	    m_pRender->GetHardInfo(&d3d,&dev,&ver);
	    SetHardCtx(d3d,dev,ver);/**/
	}/**/
	if(m_nDuiDraw){
	   m_pRender->SetRenderImgCall(m_pDuiDrawCall,m_pRenderUserData);
	}
	
    return 0;
}

LRESULT           CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	
	if(m_bOpen)
	{
		m_bOpen=false;
        
		if(m_pPlayerInstance==NULL)
		     return 0;
		m_pPlayerInstance->CloseMedia();
	}
	bHandled = TRUE;
	return 1;
}


LRESULT           CMainFrame::OnPaint(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	 

	 bHandled=true;
	 if(m_nDuiDraw)
		 return 1;
     if(m_pPlayerInstance==NULL)
		return 0;
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

	int h=0,w=0;
	if(m_nDuiDraw){
	    w=wParam;
        h=lParam;
	}else{
     ::DefWindowProc(this->m_hWnd,uMsg, wParam, lParam);
	  h=HIWORD(lParam);
	  w=LOWORD(lParam);
	}
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
	int seek=60;
	switch(wParam)
	{
	case VK_UP:
		case VK_DOWN:
			{
				HWND h=::GetParent(m_hWnd);
			            ::PostMessage(h,WM_KEYDOWN,wParam,lParam);
			               break;
			}
		case VK_LEFT:
			{
				           m_nSeekTip=1;
		                   seek=-seek;
			               if(m_pPlayerInstance->KKSeek(SeekEnum::Left,seek)){
						       m_nSeekTip=0;
						   }
			               break;
			}
		case 80:           /******P键********/
			               m_pPlayerInstance->Pause();
						   break;
		case VK_RIGHT:
			{
				           m_nSeekTip=2;
						   if(m_pPlayerInstance->KKSeek(SeekEnum::Right,seek)){
						        m_nSeekTip=0;
						   }
			               break;
			}
		
	}
	bHandled=true;
	return 0;
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
	if(wParam==10010){
		
         AVRender();
	}else{
	     
	}
	this->SetMsgHandled(FALSE);
	return 0;
}


LRESULT           CMainFrame::OnLbuttonDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	bHandled=true;
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);
	
	//::PostMessage(m_hWnd ,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(xPos, yPos)); 
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

	this->SetMsgHandled(false);
	return 1;
}

LRESULT           CMainFrame::OnOpenMediaErr(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
   bHandled=true;
   return 1;
}



int      CMainFrame::SetMaxRealtimeDelay(int Delay)
{
       if(m_pPlayerInstance!=NULL){
		   m_pPlayerInstance->SetMaxRealtimeDelay(Delay);
	   }
	   return 0;
}
void              CMainFrame::SetErrNotify(void *UserData,fpKKPlayerErrNotify ErrNotify)
{
    m_ErrNotify=ErrNotify;
	m_pErrNotifyUserData=UserData;
}

bool         CMainFrame::GrabAvPicBGRA(void* buf,int len,int &w,int &h,bool keepscale)
{
	if(m_pPlayerInstance!=NULL){
	        return m_pPlayerInstance->GrabAvPicBGRA(buf,len,w,h,keepscale);
	}
	return 0;
}
int          CMainFrame::GetPlayTime()
{
    if(m_pPlayerInstance!=NULL){
	        return m_pPlayerInstance->GetPlayTime();
	}
	return 0;
}
int          CMainFrame::GetTotalTime()
{
    if(m_pPlayerInstance!=NULL){
	        return m_pPlayerInstance->GetTotalTime();
	}
	return 0;
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


int               CMainFrame:: PreOpenUrlCallForSeg(char *InOutUrl,int *AvIsSeg,int *Interrupt)
{
	int ret=0;
	std::list<KKPluginInfo>& listx=KKPlayer::GetKKPluginInfoList();
	std::list<KKPluginInfo>::iterator It=listx.begin();
	for(;It!=listx.end();++It)
	{
	          KKPluginInfo &Info=*It;
		      if(Info.KKUrlParser!=NULL){
					  char* strurls=Info.KKUrlParser(InOutUrl,Interrupt);
					  if(strurls!=NULL){
						  Json::Reader rd;
						  Json::Value value;
						  std::string aacc=strurls;
						  if(rd.parse(aacc,value)){

							    if(value["Streams"].isNull()){
									if(!value["url"].isNull()){
										strcpy(InOutUrl, value["url"].asString().c_str());
										ret=1;
									}
								}else{
											Json::Value JsonStreams=value["Streams"];
											ret=1;
											*AvIsSeg=1;
											m_FileSegLock.Lock();
											AVFILE_SEGS_INFO &infos=m_FileInfos;
											for(int i=0;i<JsonStreams.size();i++){  
												 for(int j=0;j<JsonStreams[i]["segs"].size();j++){
														 Json::Value &seg=JsonStreams[i]["segs"][j];
												       
														 AVFILE_SEG_ITEM *Seg1= new AVFILE_SEG_ITEM();
														 Seg1->milliseconds=seg["milliseconds"].asInt();
														 Seg1->segsize=seg["segsize"].asInt();
														 Seg1->SegId=j;
														 std::string rll=seg["url"].asString();
														 strcpy(Seg1->url,rll.c_str());
														
														 infos.FileSize+=Seg1->segsize;
														 infos.milliseconds+=Seg1->milliseconds;
														 m_FileInfos.ItemCount++;
														 if(infos.pCurItem==NULL){
															infos.pCurItem=Seg1;
															infos.pItemHead=Seg1;
															strcpy(InOutUrl,Seg1->url);
														 }

														 if(infos.pItemHead==NULL){
															 infos.pItemHead=Seg1;
														 }
														 if(infos.pItemTail==NULL){
															 infos.pItemTail=Seg1;
														 }else{
															 infos.pItemTail->next=Seg1;   
															 Seg1->pre=infos.pItemTail;
															 infos.pItemTail=Seg1;
														 }

														
												 }
												 break;
											}
											m_FileSegLock.Unlock();
								}
						  }
			  }
              if(strurls!=NULL)
		        Info.KKFree(strurls);
		   }
          
	}
    return ret;
}
void              CMainFrame::OpenMediaStateNotify(char* strURL,EKKPlayerErr err)
{
     int length=0;
	 unsigned char* img=GetErrImage(length,0);
	 if((err==KKOpenUrlOkFailure) &&img!=NULL&&length>0&&m_pRender!=NULL)
	 {
		 m_pRender->SetErrPic(img,length);
		 m_pRender->ShowErrPic(true);
	 }
	 if(m_ErrNotify!=NULL)
	 {
	    m_ErrNotify(m_pErrNotifyUserData,err);
	 }
}
void              CMainFrame::GetNextAVSeg(void *playerIns,int Stata,int quesize,KKPlayerNextAVInfo &NextInfo)
{

    m_FileSegLock.Lock();
	if(m_FileInfos.ItemCount>1)
	 { 
		 if(m_pPlayerInstance==(KKPlayer*)playerIns){
			 
			 if(m_FileInfos.pCurItem!=NULL&&m_FileInfos.pCurItem->next!=NULL&&NextInfo.SegId==-1){
					 m_FileInfos.pCurItem=m_FileInfos.pCurItem->next;	
					 memset(&NextInfo,0,sizeof(NextInfo));
					 strcpy(NextInfo.url, m_FileInfos.pCurItem->url);
					 NextInfo.SegId=m_FileInfos.pCurItem->SegId;
					 NextInfo.NeedRead=true;
					 m_FileSegLock.Unlock();
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
							  m_FileSegLock.Unlock();
							  return;
						}
						pItemHead=pItemHead->next;
				   }
				 
			 }
		 }
	 }
	 m_FileSegLock.Unlock();
	 return;
}
void              CMainFrame::AVReadOverThNotify(void *playerIns)
{

}
void              CMainFrame::AVRender()
{
	if(m_pRender){
                  m_pPlayerInstance->RenderImage(m_pRender,false);
				  if(::GetTickCount()-m_nTipTick>5000){
			          m_pRender->SetLeftPicStr("");
		          }
	}
}
void CMainFrame::RenderLock()
{
if(m_pRender){
	m_pRender->renderLock();
}
}
void CMainFrame::RenderUnLock()
{
if(m_pRender){
	m_pRender->renderUnLock();
}
}
IkkRender* CMainFrame::GetRender()
{
   return m_pRender;
}
