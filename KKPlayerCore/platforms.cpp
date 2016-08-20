#include "stdafx.h"
#ifndef Platforms_H_
	#ifndef WIN32_KK
		#ifndef WIN32
			void Sleep(int ms)
			{
				usleep(ms*1000);
			}

		#endif



		
	/*		 void __cdecl LOGE(const char* _Format, ...) 
			{

				char abcd[1024];
				sprintf_s(abcd,1024,_Format,__VA_ARGS__);
			     ::OutputDebugStringA(abcd);
			}*/


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
#ifndef WIN32
	   BOOL GetClientRect(HWND hWnd,RECT *lpRect)
	   {
            return TRUE;
	   } 
	   void assert(int i)
	   {

	   }
#endif
	  
#endif
#endif