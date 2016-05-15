#include "stdafx.h"
#include <string>
#include "CFileMgr.h"
#include <tchar.h>
CFileMgr::CFileMgr()
{

}


//判断文件是否存在
bool CFileMgr::IsExist(LPCTSTR Path)
{
	WIN32_FIND_DATA Info;

	::_tcscpy_s(Info.cFileName,  _countof(Info.cFileName), Path);
	HANDLE h=FindFirstFile(Path,&Info);
	if (h == INVALID_HANDLE_VALUE)
	{
		::FindClose(h);
		return false;
	}
	::FindClose(h);
	return true;
}

//如果目录不存在则会自动创建
bool CFileMgr::CreateDirectory(LPCTSTR Path)
{

	std::wstring path2;
	std::wstring TempStr(Path);
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

	;//TempStr.find_last_of(_T("\\"));
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
	std::wstring temp;
	std::wstring temp2;
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
			if(!CreateDirectory(temp2.c_str()))
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