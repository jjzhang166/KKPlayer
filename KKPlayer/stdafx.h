// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#ifndef stdafx_kk_H_
#define stdafx_kk_H_
#include "targetver.h"


#ifndef LIBKKPLAYER
#include "UI.h"
#else
	#include <string>
	#include <vector>
	#include <list>
	#include <stdlib.h>
	#include <stdio.h>
	#include <tchar.h>
	#include <process.h>
	#include <Windows.h>
#endif
#include <assert.h>
typedef void *(*CreateRender)(HWND h,char *Oput,int cpu_flags);
typedef char (*DelRender)(void *p,char RenderType);
#endif
// TODO: 在此处引用程序需要的其他头文件
