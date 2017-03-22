#include "FileRelation.h"
CFileRelation::CFileRelation()
{

}
CFileRelation::~CFileRelation()
{

}
bool CFileRelation::CheckFileRelation(const LPTSTR strExt, const LPTSTR strAppKey)
{
    int nRet=FALSE;
    HKEY hExtKey;
    TCHAR szPath[_MAX_PATH]; 
    DWORD dwSize=sizeof(szPath); 
    if(RegOpenKey(HKEY_CLASSES_ROOT,strExt,&hExtKey)==ERROR_SUCCESS)
    {
        RegQueryValueEx(hExtKey,NULL,NULL,NULL,(LPBYTE)szPath,&dwSize);
        //if(_stricmp(szPath,strAppKey)==0)
		if(wcsicmp(szPath,strAppKey)==0)
        {
            nRet=TRUE;
        }
        RegCloseKey(hExtKey);
        return nRet;
    }
    return nRet;
}

/*
* 注册文件关联
* strExe: 要检测的扩展名(例如: ".txt")
* strAppName: 要关联的应用程序名(例如: "C:/MyApp/MyApp.exe")
* strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")
* strDefaultIcon: 扩展名为strAppName的图标文件(例如: *"C:/MyApp/MyApp.exe,0")
* strDescribe: 文件类型描述
*/
void CFileRelation::RegisterFileRelation(LPTSTR strExt, LPTSTR strAppName,LPTSTR strAppKey, LPTSTR strDefaultIcon,LPTSTR strDescribe)
{
    TCHAR strTemp[_MAX_PATH];
    HKEY hKey;
    
    RegCreateKey(HKEY_CLASSES_ROOT,strExt,&hKey);
    RegSetValue(hKey,_T(""),REG_SZ,strAppKey,lstrlen(strAppKey)+1);
    RegCloseKey(hKey);
    
	HKEY RoothKey;
   int lxx= RegCreateKey(HKEY_CLASSES_ROOT,strAppKey,&hKey);
    RegSetValueW(hKey,_T(""),REG_SZ,strDescribe,lstrlen(strDescribe)+1);
    RegCloseKey(hKey);
    
    _stprintf(strTemp,_T("%s\\DefaultIcon"),strAppKey);
    RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);
    RegSetValue(hKey,_T(""),REG_SZ,strDefaultIcon,lstrlen(strDefaultIcon)+1);
    RegCloseKey(hKey);
    
    _stprintf(strTemp,_T("%s\\Shell"),strAppKey);
    RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);
	_stprintf(strTemp,_T("%s"),_T("Open"));
   lxx= RegSetValueW(hKey,_T(""),REG_SZ,strTemp,lstrlen(strTemp)+1);
    RegCloseKey(hKey);
    
   _stprintf(strTemp,_T("%s\\Shell\\Open\\Command"),strAppKey);
    RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);
    //sprintf(strTemp,_T("%s \"%%1\""),strAppName);
	_stprintf(strTemp,_T("\"%s\" \"%%1\""),strAppName);
    RegSetValue(hKey,_T(""),REG_SZ,strTemp, lstrlen(strTemp)+1);
    RegCloseKey(hKey);
}