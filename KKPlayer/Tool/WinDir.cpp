#include "WinDir.h"
#include <shlobj.h>
#include <map>
#include <tlhelp32.h>
#include <algorithm>
#include "cchinesecode.h"
CWinDir::CWinDir()
{

}
CWinDir::~CWinDir()
{

}
std::basic_string<TCHAR>CWinDir::GetUserAppDir()
{
	
	std::basic_string<TCHAR> FileName;
	   TCHAR MyDir[_MAX_PATH];  
	   //SHGetSpecialFolderPath(this->GetSafeHwnd(),MyDir,CSIDL_APPDATA,0);CSIDL_APPDATA
	   ::SHGetSpecialFolderPath(NULL,MyDir,CSIDL_LOCAL_APPDATA,0);
	   FileName=MyDir;
	   return FileName;
}		
//判断文件是否存在
bool CWinDir::IsExist(LPCTSTR Path)
{
	WIN32_FIND_DATA Info;
	::_tcscpy_s(Info.cFileName,  _countof(Info.cFileName), Path);
	//::_tcs
	HANDLE h=FindFirstFile(Path,&Info);
	if (h == INVALID_HANDLE_VALUE)
	{
		::FindClose(h);
		return false;
	}
	::FindClose(h);
	return true;
}
int CWinDir::IsDirectory(LPCTSTR Path)
{
    WIN32_FIND_DATA fd;
    BOOL ret = 0;
    HANDLE hFind = FindFirstFile(Path, &fd);
    if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {  //目录存在
            ret = 1;    
    }
   FindClose(hFind);
 return ret;
}
//如果目录不存在则会自动创建
bool CWinDir::CreateDir(LPCTSTR Path)
{

	kkString path2;
	kkString TempStr(Path);

	if(TempStr.length()<4)
	{
		return false;
	}
	int len=TempStr.length()-1;

	int x2x=TempStr.find_first_of(_T("//"));
	while(x2x>-1)
	{
		TempStr.replace(x2x,2,_T("\\"));
		x2x=TempStr.find_first_of(_T("//"));
	}

	
	if(x2x==len)
	{
		//  Path='\0';  
		TempStr=TempStr.substr(0,len);
		path2=TempStr;
	}else
	{
		path2=TempStr;
	}
	//	IsExist(L"D:\\MFC");
	//IsExist(L"D:\\MFC\\");
	if(IsExist(path2.c_str()))
		return true;
  
	kkString temp;
	kkString temp2;

	int Index=TempStr.find_first_of(_T("\\"));
	temp=TempStr.substr(0,Index);
	temp2+=temp;
	Index+=1;
	TempStr=TempStr.substr(Index,TempStr.length()-Index);
	Index=TempStr.find_first_of(_T("\\"));
	temp2+=_T("\\");
	while(Index>-1)
	{
		temp=TempStr.substr(0,Index);
		temp2+=temp;
		Index+=1;
		TempStr=TempStr.substr(Index,TempStr.length()-Index);
		bool OK= IsExist(temp2.c_str());
		if(!OK)
		{
			if(!CreateDir(temp2.c_str()))
			{
				// ASSERT(0);
				return false;
			}
		}
		temp2+=_T("\\");
		Index=TempStr.find_first_of(_T("\\"));
	}

	if(::CreateDirectory(path2.c_str(),NULL))
	{
		return true;
	}
	return false;
}


std::basic_string<TCHAR> g_strModuleFileName;
const std::basic_string<TCHAR>& XGetModuleFilename()
{
	if (g_strModuleFileName.empty()){
		if(g_strModuleFileName.empty()){
			TCHAR filename[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, filename, _countof(filename));
			g_strModuleFileName = filename;
		}
	}
	return g_strModuleFileName;
}
std::basic_string<TCHAR> CWinDir::GetModulePath()
{
	std::basic_string<TCHAR> strModuleFileName = XGetModuleFilename();
	unsigned int index = strModuleFileName.find_last_of(_T("\\"));
	if (index != std::string::npos)
	{
		return strModuleFileName.substr(0, index);
	}
	return _T("");
}
std::basic_string<char> g_strModuleFileNameA;
const std::basic_string<char>& XGetModuleFilenameA()
{

	if (g_strModuleFileNameA.empty())
	{
		if(g_strModuleFileNameA.empty())
		{
			char filename[MAX_PATH] = { 0 };
			::GetModuleFileNameA(NULL, filename, _countof(filename));
			g_strModuleFileNameA = filename;
		}
	}
	return g_strModuleFileNameA;
}

std::basic_string<char> CWinDir::GetModulePathA()
{
	std::basic_string<char> strModuleFileName = XGetModuleFilenameA();
	unsigned int index = strModuleFileName.find_last_of("\\");
	if (index != std::string::npos)
	{
		return strModuleFileName.substr(0, index);
	}
	return "";
}

bool  CWinDir::Copyfile(LPCTSTR pTo,LPCTSTR pFrom)
{
#ifndef UNICODE
	SHFILEOPSTRUCTW FileOp={0};
	FileOp.fFlags =        FOF_NOCONFIRMATION| //不出现确认对话框 
	                         FOF_NOCONFIRMMKDIR ; //需要时直接创建一个文件夹,不需用户确定    

	wchar_t dest1[1024]=L"";
	wchar_t dest2[1024]=L"";
	CChineseCode::charTowchar(pFrom,dest1,1024);
	CChineseCode::charTowchar(pTo,dest2,1024);
	FileOp.pFrom = dest1; 
	FileOp.pTo = dest2; 
	FileOp.wFunc = FO_COPY; 
    return SHFileOperationW(&FileOp)==0;
#else
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags =        FOF_NOCONFIRMATION| //不出现确认对话框 
	                         FOF_NOCONFIRMMKDIR ; //需要时直接创建一个文件夹,不需用户确定    
	
	FileOp.pFrom = pFrom; 
	FileOp.pTo = pTo; 
	FileOp.wFunc = FO_COPY; 
    return SHFileOperation(&FileOp)==0;
#endif
}
bool CWinDir::Deletefile(LPCTSTR pPath)
{
#ifndef UNICODE
	SHFILEOPSTRUCTW FileOp={0};
	FileOp.fFlags =    FOF_SILENT | FOF_NOCONFIRMATION;

	wchar_t dest1[1024]=L"";
	CChineseCode::charTowchar(pPath,dest1,1024);
	
	FileOp.pFrom = dest1; 
	FileOp.pTo = NULL; 
	FileOp.wFunc = FO_DELETE;
    return SHFileOperationW(&FileOp)==0;
#else
    SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags =    FOF_SILENT | FOF_NOCONFIRMATION; 
	//FileOp.fFlags =   FOF_NOCONFIRMATION; 
	FileOp.pFrom =pPath; 
	FileOp.pTo =NULL; 
	FileOp.wFunc = FO_DELETE; 
    return SHFileOperation(&FileOp)==0;
#endif
}
void CWinDir::GetDiskFreeInfo(LPCTSTR pPath,double &TotalSize/***G***/,double &FreeSize)
{
	
	DWORD dwSectorsPerCluster=0;//每簇中扇区数
	DWORD dwBytesPerSector=0;//每扇区中字节数
	DWORD dwFreeClusters=0;//剩余簇数
	DWORD dwTotalClusters=0;//总簇数
	if (GetDiskFreeSpace(pPath,&dwSectorsPerCluster,&dwBytesPerSector,
		&dwFreeClusters,&dwTotalClusters))
	{
	   
	   double dd=dwSectorsPerCluster*dwBytesPerSector/(1024.*1024.);//转换为 M
	   dd=dd/1024.;//转换为 G
	   TotalSize=dwTotalClusters*dd;//该磁盘总大小
	   FreeSize=dwFreeClusters*dd;
	}
}

void CWinDir::GetBestDrive(LPTSTR OutStr,int Len)
{
        TCHAR buf[1024]=_T(""); 
		int index=0;
		double DTotalSize=0;
		int dlen=GetLogicalDriveStrings(1024,buf);
		if(dlen>0)
		for (TCHAR * s=buf; *s; s+=_tcslen(s)+1) 
        { 
             LPCTSTR sDrivePath = s;     //单个盘符
			 double TotalSize=0;
			 double FreeSize=0;
			 GetDiskFreeInfo(sDrivePath,TotalSize,FreeSize);
			 if(TotalSize>DTotalSize)
			 {
			     _tcscpy(OutStr,s);
			 }
        } 

}