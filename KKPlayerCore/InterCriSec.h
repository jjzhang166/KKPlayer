/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "stdafx.h"
#ifndef InterCriSec_H
#define InterCriSec_H
/***零界值***/
class InterCriSec
{
public:
    InterCriSec(DWORD dwSpinCount = 4096)
	{
		::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount);
	}
    ~InterCriSec()
	{
		::DeleteCriticalSection(&m_crisec);
	}
 
    void Lock()                            {::EnterCriticalSection(&m_crisec);}
    void Unlock()                          {::LeaveCriticalSection(&m_crisec);}
    BOOL TryLock()                         {return ::TryEnterCriticalSection(&m_crisec);}
    DWORD SetSpinCount(DWORD dwSpinCount) {return ::SetCriticalSectionSpinCount(&m_crisec, dwSpinCount);}
 
    CRITICAL_SECTION* GetObject()          {return &m_crisec;}
 
private:
		InterCriSec(const InterCriSec& cs);
		InterCriSec operator = (const InterCriSec& cs);
 
private:
        //临界值
        CRITICAL_SECTION m_crisec;
};
#endif