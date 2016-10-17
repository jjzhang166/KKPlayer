/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£∫2015-6-25**********************************************/
#include "stdafx.h"
#include "RendLock.h"
/***¡„ΩÁ÷µ***/
 CRendLock::CRendLock()
 {
	 ::InitializeCriticalSectionAndSpinCount(&m_crisec,4096);
 }
CRendLock::CRendLock(DWORD dwSpinCount)
{
   ::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount);
}
CRendLock::~CRendLock()
{
   ::DeleteCriticalSection(&m_crisec); 
}
void CRendLock::Lock()
{
	::EnterCriticalSection(&m_crisec);
}
void CRendLock::Unlock()
{
   ::LeaveCriticalSection(&m_crisec);
}
BOOL CRendLock::TryLock()
{
	return ::TryEnterCriticalSection(&m_crisec);
}