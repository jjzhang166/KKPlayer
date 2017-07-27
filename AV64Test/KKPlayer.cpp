#include "stdafx.h"
#include "KKPlayer.h"
static HMODULE hSysResource=0;
fpCreateKKPlayer    m_FnCreatePlayer=0;
fpCreateDuiKKPlayer m_FnCreateDuiKKPlayer=0;
fpCreateDuiRawKKPlayer  m_FnCreateDuiRawKKPlayer=0;

fpKKOpenMedia     m_FnOpenMedia=0;
fpKKCloseMedia    m_FnKKCloseMedia=0;
fpSetKKVolume     m_FnSetKKVolume=0;
fpKKDelPlayer     m_FnKKDelPlayer=0;
fpKKSetErrNotify  m_FnKKSetErrNotify=0;
fpKKDuiOnSize     m_FpKKDuiOnSize=0;
fpRefreshDuiKKPlayer  m_FpRefreshDuiKKPlayer=0;
fpSetMaxRealtimeDelay m_FpSetMaxRealtimeDelay=0;
CKKPlayer::CKKPlayer():m_hWnd(0),m_pPlayerh(0)
,m_pNty(0)
,m_UserData(0)
,m_FnRenderImgCal(0)
,m_nDui(0)
{
     //CWinDir dir;
	//std::wstring path=dir.GetModulePath();
	if(m_FnCreatePlayer)
		return;
	if(hSysResource==NULL)
	    hSysResource=LoadLibrary(_T("libkkplayer.dll"));
	 if(hSysResource){
         m_FnCreatePlayer = (fpCreateKKPlayer )GetProcAddress(hSysResource, "CreateKKPlayer");
		 m_FnCreateDuiKKPlayer= (fpCreateDuiKKPlayer )GetProcAddress(hSysResource, "CreateDuiKKPlayer");
		 m_FnCreateDuiRawKKPlayer=(fpCreateDuiRawKKPlayer )GetProcAddress(hSysResource, "CreateDuiRawKKPlayer");
		 m_FpRefreshDuiKKPlayer=(fpRefreshDuiKKPlayer)GetProcAddress(hSysResource, "RefreshDuiKKPlayer");
		 m_FpKKDuiOnSize= (fpKKDuiOnSize)GetProcAddress(hSysResource, "KKDuiOnSize");
		 m_FnOpenMedia= (fpKKOpenMedia )GetProcAddress(hSysResource, "KKOpenMedia");
         m_FnKKCloseMedia= (fpKKCloseMedia )GetProcAddress(hSysResource, "KKCloseMedia");
         m_FnSetKKVolume= (fpSetKKVolume )GetProcAddress(hSysResource, "KKSetVolume");
		 m_FnKKDelPlayer= (fpKKDelPlayer )GetProcAddress(hSysResource, "KKDelPlayer");
		 m_FnKKSetErrNotify= (fpKKSetErrNotify )GetProcAddress(hSysResource, "KKSetErrNotify");
		 m_FpSetMaxRealtimeDelay=(fpSetMaxRealtimeDelay )GetProcAddress(hSysResource, "SetMaxRealtimeDelay");
	 }else{
		 ::MessageBox(0,L"libkkplayer.dll err",L"",0);
	 }
}
CKKPlayer::~CKKPlayer()
{
	if(m_pPlayerh!=NULL){
		bool Dui=false;
		if(m_nDui)
			Dui=true;
	    m_FnKKDelPlayer(m_pPlayerh,Dui);
		m_pPlayerh=NULL;
	}
}
enum EKKPlayerErr
{
     KKOpenUrlOk=0,          /***播发器打开成功**/
	 KKOpenUrlOkFailure=1,  /**播发器打开失败***/
	 KKAVNotStream=2,
	 KKAVReady=3,          ///缓冲已经准备就绪
	 KKAVWait=4,           ///需要缓冲
	 KKRealTimeOver=5,     ///流媒体结束
};
void CKKPlayer::ErrNotify(void *UserData,int errcode)
{
	CKKPlayer *Player=(CKKPlayer *)UserData;
	if(!Player->m_pNty)
		return;
	if(errcode==KKOpenUrlOkFailure || errcode==KKRealTimeOver){
	    Player->m_pNty->OnPlayerState( Player->m_UserData,AVOpenFailed);
	}else if(errcode==KKAVReady){
		Player->m_pNty->OnPlayerState( Player->m_UserData,AVLoadingOver);
   }
}
HWND CKKPlayer::CreateKKPlayer(HWND h,RECT Rt,DWORD style,bool yuv420)
{
	if( m_FnCreatePlayer){
	      m_pPlayerh = m_FnCreatePlayer(h,Rt,style,&m_hWnd,yuv420);
		  m_FnKKSetErrNotify(m_pPlayerh,ErrNotify,this);
		  return m_hWnd;
	}
	return NULL;
}
void CKKPlayer::DuiSize(int w,int h)
{
     if(m_FpKKDuiOnSize)
	 {
	      m_FpKKDuiOnSize(m_pPlayerh,w,h);
	 }
}
void   CKKPlayer::SetMaxRealtimeDelay(int Delay)
{
    if(m_FpSetMaxRealtimeDelay&&m_pPlayerh)
	{
	   m_FpSetMaxRealtimeDelay(m_pPlayerh,Delay);
	}
}
void CKKPlayer::CreateDuiKKPlayer(HWND hAudio,fpRenderImgCall fpCall,void *UserData)
{
	if(m_FnCreateDuiKKPlayer){
		m_nDui=1;
	    m_pPlayerh =  m_FnCreateDuiKKPlayer(hAudio,fpCall,UserData);
		m_FnKKSetErrNotify(m_pPlayerh,ErrNotify,this);
	}
}
void CKKPlayer::CreateDuiRawKKPlayer(HWND hAudio,int imgType,fpRenderImgCall fpCall,void *UserData)
{
    if(m_FnCreateDuiKKPlayer)
	{
		m_nDui=1;
	    m_pPlayerh =  m_FnCreateDuiRawKKPlayer(hAudio,fpCall,UserData,imgType);
		m_FnKKSetErrNotify(m_pPlayerh,ErrNotify,this);
	}
}
void CKKPlayer::RefreshDuiPlayer()
{
	if(m_pPlayerh!=NULL)
      m_FpRefreshDuiKKPlayer(m_pPlayerh);
}

void CKKPlayer::Init()
{

}
void CKKPlayer::SetAVPlayerNotify(IAVPlayerNotify* nty)
{
   m_pNty=nty;
}
int  CKKPlayer::OpenMedia(std::string url)
{
	if(m_pPlayerh!=NULL){
	    return m_FnOpenMedia(m_pPlayerh,url.c_str(),"");
	}
	return -1;
}
void CKKPlayer::CloseMedia()
{
	if(m_pPlayerh!=NULL){
		 m_FnKKCloseMedia(m_pPlayerh);
	}
}
void CKKPlayer::SetVlcVolume(int volume,bool tip)
{
	if(m_pPlayerh!=NULL){
		m_FnSetKKVolume(m_pPlayerh,volume,tip);
	}
}
void  CKKPlayer::SetUserData(void* UserData)
{
	m_UserData=UserData;
}