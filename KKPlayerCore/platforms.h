#ifndef WIN32
    #define NULL 0
	#define ANDROID_KKPAYER 1
	#define INFINITE            0xFFFFFFFF  // Infinite timeout
	#define FALSE               0
	#define TRUE                1
    #define __stdcall __attribute__((__stdcall__))

	typedef int                 BOOL;
	typedef void *HANDLE;
	typedef HANDLE HWND;
	typedef unsigned long       DWORD;
    typedef pthread_rwlock_t CRITICAL_SECTION;
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
	/*BOOL ResetEvent(HANDLE hEvent);
	BOOL SetEvent(HANDLE hEvent);

	DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds);
	DWORD WaitForMultipleObjects(DWORD nCount,HANDLE *lpHandles,BOOL bWaitAll,DWORD dwMilliseconds);
	HANDLE CreateEvent(int lpEventAttributes,BOOL bManualReset,BOOL bInitialState,void* lpName);
	BOOL TerminateThread(HANDLE hThread,DWORD dwExitCode);
	BOOL CloseHandle(HANDLE hObject);*/
	BOOL GetClientRect(HWND hWnd,RECT *lpRect);
	unsigned int _beginthreadex(void * _Security,unsigned _StackSize,unsigned (__stdcall * _StartAddress) (void *),void * _ArgList, unsigned _InitFlag,unsigned * _ThrdAddr);
#endif