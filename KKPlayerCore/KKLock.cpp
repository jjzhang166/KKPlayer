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
#ifdef WIN32_KK
	 ::InitializeCriticalSectionAndSpinCount(&m_crisec,4096);
#else
	
	 int ret=pthread_rwlock_init(&m_crisec,NULL);
     if(ret)
	 {
		 //Err
	 }
#endif
 }
CKKLock::CKKLock(DWORD dwSpinCount)
{
#ifdef WIN32_KK
   ::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount);
#endif
}
CKKLock::~CKKLock()
{
   #ifdef WIN32_KK
		::DeleteCriticalSection(&m_crisec);
   #else
	     pthread_rwlock_destroy(&m_crisec);
   #endif
}
void CKKLock::Lock()
{
	#ifdef WIN32_KK
		::EnterCriticalSection(&m_crisec);
	#else
	     pthread_rwlock_wrlock(&m_crisec);
	#endif
}
void CKKLock::Unlock()
{
	#ifdef WIN32_KK
		::LeaveCriticalSection(&m_crisec);
    #else
		pthread_rwlock_unlock(&m_crisec);
	#endif
}
BOOL CKKLock::TryLock()
{
	#ifdef WIN32_KK
	      return ::TryEnterCriticalSection(&m_crisec);
	#else
		if(pthread_rwlock_trywrlock(&m_crisec))
		{
			return TRUE;
		}
		return FALSE;
	#endif
}