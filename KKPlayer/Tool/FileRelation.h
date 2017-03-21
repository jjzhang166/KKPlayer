#ifndef FileRelation_H_
#define FileRelation_H_
#include "../stdafx.h"
class CFileRelation
{
public:
       CFileRelation();
	   ~CFileRelation();
public:
	    bool CheckFileRelation(const LPTSTR strExt, const LPTSTR strAppKey);
		void RegisterFileRelation(LPTSTR strExt, LPTSTR strAppName,LPTSTR strAppKey, LPTSTR strDefaultIcon,LPTSTR strDescribe);
};
#endif