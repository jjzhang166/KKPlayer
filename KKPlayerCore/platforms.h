#ifndef WIN32_KK
    
    #ifdef Android_Plat
         #include <android/log.h>
         #include <pthread.h>
         #include <unistd.h>
         #define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
    #else
        #include <Windows.h>
		#include <assert.h>
        #include "../WinPthread/include/pthread.h"
        #pragma comment (lib,"..\\WinPthread\\lib\\pthreadVC2.lib")
        #define LOGE(...) ;
    #endif
    typedef pthread_rwlock_t KKCRITICAL_SECTION;
    #define NULL 0
	#define ANDROID_KKPAYER 1
	#define INFINITE            0xFFFFFFFF  // Infinite timeout
	#define FALSE               0
	#define TRUE                1
	#ifndef WIN32
		 #define __stdcall __attribute__((__stdcall__))
	#endif
    #define  LOG_TAG    "libgl2jni"
   
	
	/*BOOL ResetEvent(HANDLE hEvent);
	BOOL SetEvent(HANDLE hEvent);

	DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds);
	DWORD WaitForMultipleObjects(DWORD nCount,HANDLE *lpHandles,BOOL bWaitAll,DWORD dwMilliseconds);
	HANDLE CreateEvent(int lpEventAttributes,BOOL bManualReset,BOOL bInitialState,void* lpName);
	BOOL TerminateThread(HANDLE hThread,DWORD dwExitCode);
	BOOL CloseHandle(HANDLE hObject);*/
	#ifndef WIN32
		typedef int                 BOOL;
		typedef void *HANDLE;
		typedef HANDLE HWND;
		typedef unsigned long       DWORD;

		typedef unsigned int UINT;
		typedef int HDC;
		typedef long LONG;
		typedef void            *LPVOID;
		typedef long long      INT64;
		typedef struct tagRECT
		{
			LONG    left;
			LONG    top;
			LONG    right;
			LONG    bottom;
		} RECT;

		void assert(int i);
		void Sleep(int ms);
		BOOL GetClientRect(HWND hWnd,RECT *lpRect);
	#endif
	//unsigned int _beginthreadex(void * _Security,unsigned _StackSize,unsigned (__stdcall * _StartAddress) (void *),void * _ArgList, unsigned _InitFlag,unsigned * _ThrdAddr);
#endif