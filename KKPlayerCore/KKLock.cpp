/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£∫2015-6-25**********************************************/
#include "KKLock.h"
/***¡„ΩÁ÷µ***/
 CKKLock::CKKLock()
 {
#ifdef WIN32
	 ::InitializeCriticalSectionAndSpinCount(&m_crisec,4096);
#endif
 }
CKKLock::CKKLock(DWORD dwSpinCount)
{
#ifdef WIN32
   ::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount);
#endif
}
CKKLock::~CKKLock()
{
	#ifdef WIN32
   ::DeleteCriticalSection(&m_crisec);
   #endif
}
void CKKLock::Lock()
{
	#ifdef WIN32
	::EnterCriticalSection(&m_crisec);
	#endif
}
void CKKLock::Unlock()
{
	#ifdef WIN32
	::LeaveCriticalSection(&m_crisec);
	#endif
}
BOOL CKKLock::TryLock()
{
	#ifdef WIN32
	return ::TryEnterCriticalSection(&m_crisec);
	#else
		return TRUE;
	#endif
}