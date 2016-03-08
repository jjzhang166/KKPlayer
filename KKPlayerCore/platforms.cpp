#include "stdafx.h"
#ifndef Platforms_H_
	#ifndef WIN32
	   void Sleep(int ms)
	   {
          sleep(ms);
       }
      /* BOOL ResetEvent(HANDLE hEvent)
       {
	      return TRUE;
       }
	   BOOL SetEvent(HANDLE hEvent)
	   {
		   return TRUE;
	   }
	   DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds)
	   {
		   return 0;
	   }
	   HANDLE CreateEvent(int lpEventAttributes,BOOL bManualReset,BOOL bInitialState,void* lpName)
	   {
		   return NULL;
	   }
	   BOOL TerminateThread(HANDLE hThread,DWORD dwExitCode)
	   {
		  return TRUE;
	   }
	   BOOL CloseHandle(HANDLE hObject)
	   {
           return TRUE;
	   }
	   DWORD WaitForMultipleObjects(DWORD nCount,HANDLE *lpHandles,BOOL bWaitAll,DWORD dwMilliseconds)
	   {
		    return TRUE;
	   }*/
	   BOOL GetClientRect(HWND hWnd,RECT *lpRect)
	   {
            return TRUE;
	   }
	   void assert(int i)
	   {

	   }
    #endif
#endif