#ifndef KKPLATFORMS_H_
#define KKPLATFORMS_H_
#ifndef WIN32_KK
    
    #ifdef Android_Plat
         #include <android/log.h>
         #include <pthread.h>
         #include <unistd.h>
         // #define  LOGE(...) ; 
         #define  LOGE_KK(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
    #else
        #include <Windows.h>
		#include <assert.h>
        #include "../WinPthread/include/pthread.h"
        #pragma comment (lib,"..\\WinPthread\\lib\\pthreadVC2.lib")
        #define LOGE_KK(...) ;
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
        typedef unsigned char       BYTE;
		typedef unsigned long ULONG_PTR;
		typedef ULONG_PTR DWORD_PTR;
		typedef unsigned short      WORD;

        #define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
		#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
		#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
		#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
		#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

		void assert(int i);
		void Sleep(int ms);
		BOOL GetClientRect(HWND hWnd,RECT *lpRect);
	#endif
	
	//unsigned int _beginthreadex(void * _Security,unsigned _StackSize,unsigned (__stdcall * _StartAddress) (void *),void * _ArgList, unsigned _InitFlag,unsigned * _ThrdAddr);
#endif
		char**  KKCommandLineToArgv(const char* CmdLine,int* _argc);
#endif