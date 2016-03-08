//NoNeedWindows_H不需要windows头文件
#ifndef stdafx_H_
#define stdafx_H_
	#ifndef WIN32
		#include "platforms.h"
	#else
		#define WIN32_LEAN_AND_MEAN
		#ifndef NoNeedWindows_H 
			#include <Windows.h>
			#include <WinUser.h>
		#endif
		#include <process.h>
		#include <assert.h>
	#endif
#endif