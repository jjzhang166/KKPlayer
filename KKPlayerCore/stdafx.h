//NoNeedWindows_H不需要windows头文件
//#define WIN32_KK
#define Android_Plat
#ifndef stdafx_H_
#define stdafx_H_
	#ifdef WIN32_KK
			#define WIN32_LEAN_AND_MEAN
			#ifndef NoNeedWindows_H 
				#include <Windows.h>
				#include <WinUser.h>
			#endif
			#include <process.h>
			#include <assert.h>

            typedef CRITICAL_SECTION KKCRITICAL_SECTION;
            #define  LOGE(...)  ;
//(char _Labcd__[256]="%s";sprintf(_Labcd__,__VA_ARGS__);::OutputDebugStringA(_Labcd__))
	#else
            #include "platforms.h"
	#endif
#endif