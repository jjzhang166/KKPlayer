#ifndef SDomodalwnd_H_
#define SDomodalwnd_H_
#include "../ui.h"
//Ä£Ì¬´°¿Ú
namespace SOUI{
  class SDomodalwnd : protected SHostWnd
  {
	  SOUI_CLASS_NAME(SDomodalwnd,L"domodalwnd")
      public:
		   SDomodalwnd(LPCTSTR pszResName = NULL,bool AutoClose=TRUE,bool NoBKey=TRUE,DWORD style=WS_EX_TOOLWINDOW|WS_EX_TOPMOST);
		   virtual  ~SDomodalwnd();
		   virtual  void Init();
	       virtual  int DoModel(int x,int y,HWND hOwner);
      protected:

		   BEGIN_MSG_MAP_EX(SDomodalwnd)
		       CHAIN_MSG_MAP(SHostWnd)
		       REFLECT_NOTIFICATIONS_EX()
	       END_MSG_MAP()
		   int m_ModelState;
		   HWND m_hOwner;
	  private:
		   DWORD m_style;
		   bool m_bAutoClose;
		   bool m_NoBKey;
	       void MsgRun(HWND hOwner);
		   
  };
}
#endif