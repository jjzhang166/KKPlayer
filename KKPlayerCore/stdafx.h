//NoNeedWindows_H不需要windows头文件
//#define WIN32_KK //使用windows 线程
#include <stdlib.h>
#ifdef WIN32
     #define WIN32_KK  //否则使用pthred线程
#else
     #define Android_Plat
#endif
#ifndef stdafx_H_KK_
#define stdafx_H_KK_
	#ifdef WIN32_KK
			#define WIN32_LEAN_AND_MEAN
			#ifndef NoNeedWindows_H 
				#include <Windows.h>
				#include <WinUser.h>
			#endif
			#include <process.h>
			#include <assert.h>
            void WindowsLOGE(const char* format,...);
            typedef CRITICAL_SECTION KKCRITICAL_SECTION;
			//void __cdecl LOGE(const char* _Format,...);

            #define  LOGE WindowsLOGE
			//__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
	#else
            #include "platforms.h"
	#endif

	#ifdef WIN32
	       #define snprintf _snprintf
	#endif
    //统一分配内存
    void *KK_Malloc_(size_t size);
	void  KK_Free_(void *ptr);
#endif