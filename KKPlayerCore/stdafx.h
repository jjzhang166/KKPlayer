//NoNeedWindows_H不需要windows头文件
#ifndef stdafx_H_
#define stdafx_H_
	#ifdef WIN32
			#define WIN32_LEAN_AND_MEAN
			#ifndef NoNeedWindows_H 
			#include <Windows.h>
			#include <WinUser.h>
			#endif
			#include <process.h>
			#include <assert.h>

            #define  LOGE(...)  ;
//(char _Labcd__[256]="%s";sprintf(_Labcd__,__VA_ARGS__);::OutputDebugStringA(_Labcd__))
	#else
			#include <pthread.h>
            #include <unistd.h>
            #include "platforms.h"
	#endif
#endif