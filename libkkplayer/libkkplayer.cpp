// libkkplayer.cpp : 定义 DLL 应用程序的导出函数。
//
#include "../KKPlayer/stdafx.h"
#include "../KKPlayer/MainFrm.h"
CreateRender pfnCreateRender=NULL;
DelRender pfnDelRender=NULL;


////
void Init(){

	if(pfnCreateRender==NULL || pfnDelRender==NULL){
		HMODULE hRender = LoadLibraryA("Render.dll");
		if(hRender){
			  pfnCreateRender = (CreateRender)GetProcAddress(hRender, "CreateRender");
			  pfnDelRender = (DelRender)GetProcAddress(hRender, "DelRender");
		}
	}
}
extern "C"{
	
	///创建一个带窗口的播放器
	void __declspec(dllexport) *CreateKKPlayer(HWND h,RECT Rt,DWORD style,HWND *OutHwnd,bool yuv420)
	{
			Init();
			RECT rt={0,100,200,300};
			//WS_CHILDWINDOW| WS_CLIPCHILDREN 
			CMainFrame *m_pVideoWnd = new CMainFrame(yuv420,true);
			
			if(m_pVideoWnd->CreateEx(h,Rt, style) == NULL){
					return 0;
			}
			
			*OutHwnd=m_pVideoWnd->m_hWnd;
			return m_pVideoWnd;
	}

	void __declspec(dllexport) *CreateKKPlayerDui()
	{
			Init();
			

			return NULL;
	}

   void __declspec(dllexport) KKSetErrNotify(void* player,fpKKPlayerErrNotify noti,void* UserData)
   {
         CMainFrame *Player = static_cast<CMainFrame *>(player);
		 if(Player!=NULL){
		    Player-> SetErrNotify(UserData,noti);
		 }
   }

	

   int __declspec(dllexport) KKOpenMedia(void* player,const char* url,int Vol)
   {
	   
         CMainFrame *Player = static_cast<CMainFrame *>(player);
		 if(Player!=NULL){
		 Player->SetVolume(Vol);
	     return	 Player->OpenMedia(url);
		 }
		 return -2;
   }
   void __declspec(dllexport) KKCloseMedia(void* player)
   {
          CMainFrame *Player = static_cast<CMainFrame *>(player);
		  if(Player!=NULL){
		      Player->CloseMedia();
		  }
   }
   void __declspec(dllexport) KKSetVolume(void* player,int volume)
   {
        CMainFrame *Player = static_cast<CMainFrame *>(player);
		if(Player!=NULL){
		 Player->SetVolume(volume);
		}
   }
   void __declspec(dllexport) KKDelPlayer(void* player)
   {
        CMainFrame *Player = static_cast<CMainFrame *>(player);
		if(Player!=NULL){
			if(::IsWindow(Player->m_hWnd))
			::SendMessage(Player->m_hWnd,WM_CLOSE,0,0);
			// delete Player;
		}
   }
}
