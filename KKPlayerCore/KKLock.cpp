/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£∫2015-6-25**********************************************/
#include "KKLock.h"
/***¡„ΩÁ÷µ***/
CKKLock::CKKLock(DWORD dwSpinCount)
{
   ::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount);
}
CKKLock::~CKKLock()
{
   ::DeleteCriticalSection(&m_crisec);
}
void CKKLock::Lock()
{
	::EnterCriticalSection(&m_crisec);
}
void CKKLock::Unlock()
{
	::LeaveCriticalSection(&m_crisec);
}
BOOL CKKLock::TryLock()
{
	return ::TryEnterCriticalSection(&m_crisec);
}