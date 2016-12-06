#include "stdafx.h"
#include "MainPage/MainDlg.h"
#include "MainFrm.h"
#include <ObjIdl.h>
#include "KKSound.h"
#include "Tool/cchinesecode.h"
#include "Tool/CFileMgr.h"
#include "DownManage/AVDownManage.h"
#pragma comment(lib, "winmm.lib")
//#include <Windows.h>
extern SOUI::CMainDlg* m_pDlgMain;
extern CreateRender pfnCreateRender;
extern DelRender pfnDelRender;
//#define QY_GDI
Gdiplus::Bitmap* CoverPic(int destWidth,int destHeight,Gdiplus::Image* srcBmp)
{
	Gdiplus::Bitmap* pDestBmp= new Gdiplus::Bitmap(destWidth, destHeight, PixelFormat32bppARGB); //新建缩放后的位图  
	unsigned char* pOutData=new unsigned char[destWidth * destHeight * 4]; //新建缩放后的rgb数据  
	if (pDestBmp)  
	{  
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(pDestBmp);
		if (g)  
		{  
			// 使用高质量模式(相对比较耗时)，可以查看msdn，替换为其他mode   
			g->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);  
			g->DrawImage(srcBmp, 0, 0, destWidth, destHeight);  
			delete g;   
		}  
	}  

	Gdiplus::BitmapData* pBitmapData = new Gdiplus::BitmapData;  
	Gdiplus::Rect rect( 0, 0, destWidth, destHeight);  
	pDestBmp->LockBits(&rect , Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, pBitmapData);  
	memcpy(pOutData, (unsigned char*)pBitmapData->Scan0, destWidth * destHeight * 4); //得到缩放后的rgb数据  
	pDestBmp->UnlockBits(pBitmapData);  
	return pDestBmp;
}
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
CMainFrame::CMainFrame():
m_pBkImage(NULL),m_pCenterLogoImage(NULL),m_pAVMenu(NULL),
m_pErrOpenImage(NULL),m_ErrOpenImgLen(NULL)
{
	m_pSound=NULL;
	m_pPlayerInstance=NULL;
	m_CenterLogoLen=0;
	std::string basePath=GetModulePathA();
	m_BkGidPulsBitmap=NULL;
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
		}
		fclose(fp);
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
		}
		fclose(fp);
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
		}
		fclose(fp);
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
		}
		fclose(fp);
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
		}
		fclose(fp);
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
		}
		fclose(fp);
	}
	m_CurWaitPic=NULL;
	
	
	
}

CMainFrame::~CMainFrame()
{
    m_pPlayerInstance->CloseMedia();
	pfnDelRender(m_pRender,0);
	m_pRender=NULL;
	delete m_pSound;
	delete m_pPlayerInstance;
}
int CMainFrame::GetRealtime()
{
	if(m_pPlayerInstance!=NULL)
		return m_pPlayerInstance->GetRealtime();

	return 0;
}
void CMainFrame::UpdateLayout(BOOL bResizeBars)
{
   	   
}
void  CMainFrame::AvSeek(int value)
{
	m_pPlayerInstance->AVSeek(value);
}
void CMainFrame::SetVolume(long value)
{
	m_pPlayerInstance->SetVolume(value);
}
MEDIA_INFO  CMainFrame::GetMediaInfo()
{
   return 	m_pPlayerInstance->GetMediaInfo();
}
int CMainFrame::GetCurTime()
{
	return m_pPlayerInstance->GetCurTime();
}
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;
	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

#   define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
	(((y) + (1L << 15)) >> 16))
std::basic_string<TCHAR> GetModulePath();

int CMainFrame::DownMedia(char *KKVURL,bool Down)
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
					 CAVDownManage* pinxx=CAVDownManage::GetInstance();
					 pinxx->Add(KKVURL,KKPl);
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
int  CMainFrame::OpenMedia(std::string url,std::string FilePath)
{
	RECT rt;
	::GetClientRect(m_hWnd,&rt);
	 m_pRender->resize(rt.right-rt.left,rt.bottom-rt.top);
	 if(m_pRender!=NULL)
	 {
		   m_pRender->ShowErrPic(false);
	 }
	  
	int  ret=m_pPlayerInstance->OpenMedia((char*)url.c_str(),(char*)FilePath.c_str());
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

	
	return ret;
}

#include<Mmsystem.h>
void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUsers,DWORD dw1,DWORD dw2)
{
          CMainFrame *Pts=(CMainFrame *)dwUsers;
		 // Pts->Render();
}
LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	

	m_pSound =new CSDLSound();//
	m_pSound->SetWindowHAND((int)m_hWnd);
	
	m_pPlayerInstance = new KKPlayer(this,m_pSound);
	

	m_pRender=NULL;
	
	this->SetFocus();
	this->EnableWindow(true);
	::SetTimer(this->m_hWnd,10010,40,NULL);

	char Out=0;
	m_pRender=(CRender*) pfnCreateRender(m_hWnd,&Out);
	if(Out==0)
	{
         m_pPlayerInstance->SetBGRA();
	}
	
	

	 m_pPlayerInstance->InitSound();
	 m_pPlayerInstance->SetWindowHwnd(m_hWnd);


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

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	delete m_BkGidPulsBitmap;
	m_BkGidPulsBitmap=NULL;
	if( m_bOpen)
	{
		m_bOpen=false;
		m_pPlayerInstance->CloseMedia();
	}
	DWORD Id=GetCurrentThreadId();
	::PostThreadMessage(Id, WM_QUIT, 0, 0);
	bHandled = TRUE;
	return 1;
}


LRESULT  CMainFrame::OnPaint(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	 
	 bHandled=true;
	 if(!m_bOpen)
	 {
		 /*PAINTSTRUCT ps = { 0 };
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
		 ::DeleteDC(MemDC);*/

		 //::InvalidateRect(m_hWnd,NULL,FALSE);
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
//#else
		/* Gdiplus::Graphics graphics(MemDC);
		 {  
			 // 使用高质量模式(相对比较耗时)，可以查看msdn，替换为其他mode   
			 //graphics(SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);  
			 graphics.DrawImage(m_BkGidPulsBitmap, 0, 0, rcWindow.right-rcWindow.left,rcWindow.bottom-rcWindow.top);  
		 }  */
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
		
		 //::ValidateRect(m_hWnd,NULL);
		// ::InvalidateRect(m_hWnd,NULL,NULL);
	 }
	
      //m_pPlayerInstance->RenderImage(m_pRender);
	// return ::DefWindowProc(m_hWnd,uMsg,wParam,lParam);
	return 1;
}

void CMainFrame::OnDraw(HDC& memdc,RECT& rt)
{
     HBRUSH m_SelectDotHbr=CreateSolidBrush(RGB(86, 147, 44));
	::FillRect(memdc,&rt,m_SelectDotHbr);
	::DeleteObject(m_SelectDotHbr);

	#ifdef WIN32_KK
	    m_pPlayerInstance->OnDrawImageByDc(memdc);
    #endif
}
void CMainFrame::AVRender()
{
 m_pPlayerInstance->RenderImage(m_pRender,false);
}
LRESULT  CMainFrame::OnSize(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
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
LRESULT  CMainFrame::OnEraseBkgnd(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
    bHandled=true;
	return 1;
}
LRESULT  CMainFrame::OnTimer(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{

	if(wParam==10010)
	{
          AVRender();
	}
	/*#ifndef QY_GDI
         AVRender();
    #else
	    RECT rcWindow;
	    ::GetClientRect(m_hWnd,&rcWindow);
	    this->InvalidateRect(&rcWindow);
    #endif*/
	bHandled=true;
	return 1;
}
void CMainFrame::CloseMedia()
{
  
   m_pPlayerInstance->CloseMedia();
   m_bOpen=false;
}
int CMainFrame::PktSerial()
{
  return m_pPlayerInstance->GetPktSerial();
}
void  CMainFrame::OnDecelerate()
{
   m_pPlayerInstance->OnDecelerate();

    int Rate=m_pPlayerInstance->GetAVRate();
   /* CRenderD3D *pRender=( CRenderD3D *)m_pRender;
	wchar_t abcd[256]=L"";
	float aa=(float)Rate/100;
	swprintf(abcd,L"%.1f倍",aa);
    pRender->SetLeftPicStr(abcd);*/
}
void  CMainFrame::OnAccelerate()
{
   m_pPlayerInstance->OnAccelerate();
   int Rate=m_pPlayerInstance->GetAVRate();
  /* CRenderD3D *pRender=( CRenderD3D *)m_pRender;
   wchar_t abcd[256]=L"";
   float aa=(float)Rate/100;
   swprintf(abcd,L"%.1f倍",aa);
   pRender->SetLeftPicStr(abcd);*/
}
void CMainFrame::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
	m_pPlayerInstance->GetAVHistoryInfo(slQue);
}
LRESULT  CMainFrame::OnClose(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
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
LRESULT  CMainFrame::OnKeyDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
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

unsigned char*  CMainFrame::GetErrImage(int &length,int ErrType)
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
unsigned char*  CMainFrame::GetWaitImage(int &len,int curtime)
{
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


int  CMainFrame::Pause()
{
	m_pPlayerInstance->Pause();
	return 0;
}
 unsigned char* CMainFrame::GetCenterLogoImage(int &len)
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
unsigned char*  CMainFrame::GetBkImage(int &len)
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

LRESULT CMainFrame::OnOpenMediaErr(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
   bHandled=true;
   char *err=(char*)wParam;
   ::MessageBoxA(m_hWnd,err,"错误",MB_ICONHAND);
   ::free(err);
   CloseMedia();
   return 1;
}
void CMainFrame::OpenMediaFailure(char* strURL,int err)
{
	//char *err=(char*)::malloc(1024);
	//memset(err,0,1024);
	//std::string abcd="无法打开路径：";
	//abcd+=strURL;
	//memcpy(err,abcd.c_str(),abcd.length());
	////::MessageBoxA(m_hWnd,abcd.c_str(),"错误",MB_ICONHAND);
	//::PostMessage(m_hWnd,WM_OpenErr ,(WPARAM )err,0);

     int length=0;
	 unsigned char* img=GetErrImage(length,0);
	 if(img!=NULL&&length>0&&m_pRender!=NULL)
	 {
		 m_pRender->SetErrPic(img,length);
		 m_pRender->ShowErrPic(true);
	 }
}
void  CMainFrame::AutoMediaCose(int Stata)
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
	
}
LRESULT  CMainFrame::OnLbuttonDown(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	bHandled=true;
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);
	
	//if(!m_pDlgMain->GetScreenModel())
	::PostMessage(::GetParent(m_hWnd) ,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(xPos, yPos)); 

	if(!m_pDlgMain->GetScreenModel())
	{
	   m_pDlgMain->ShowMiniUI(false);
	}
	return 1;
}
LRESULT  CMainFrame::OnRbuttonUp(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{
	bHandled=true;
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);

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
	return 1;
}

LRESULT  CMainFrame::OnMouseMove(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
{

   bHandled=true;
   int xPos = GET_X_LPARAM(lParam); 
   int yPos = GET_Y_LPARAM(lParam);
   
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

	m_lastPoint.x=xPos;
	m_lastPoint.y=yPos;
	return 0;
}