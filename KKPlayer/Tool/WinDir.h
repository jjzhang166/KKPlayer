#ifndef WinDir_H_
#define WinDir_H_
#include "..\stdafx.h"
#include <string>
#ifdef  UNICODE  
     typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >  kkString;
#else
     typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> >           kkString;
#endif
class  CWinDir{
 public:
	    CWinDir();
		~CWinDir();
		//获取用户App目录
		std::basic_string<TCHAR> GetUserAppDir();
		//判断文件是否存在
		bool IsExist(LPCTSTR Path);
		//如果目录不存在则会自动创建
		bool CreateDir(LPCTSTR Path);
		//获取当前模块目录
		std::basic_string<TCHAR> GetModulePath();
		/**
		*复制文件,使用UIcode才复制中文成功
		*/
		bool Copyfile(LPCTSTR pTo,LPCTSTR pFrom);
		bool Deletefile(LPCTSTR pPath);
		///判断是否是目录
		int  IsDirectory(LPCTSTR Path);

		void GetDiskFreeInfo(LPCTSTR pPath,double &TotalSize/***G***/,double &FreeSize);
		void GetBestDrive(LPTSTR OutStr,int Len);
};
#endif