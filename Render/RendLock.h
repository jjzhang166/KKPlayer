/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "stdafx.h"
#ifndef RendLock_H
#define RendLock_H
/***零界值***/
class CRendLock
{
public:
	CRendLock();
    CRendLock(DWORD dwSpinCount);
    ~CRendLock();
    void Lock();
    void Unlock();
    BOOL TryLock();
private:
		CRendLock(const CRendLock& cs);
		CRendLock operator = (const CRendLock& cs);
 
private:
        //临界值
        CRITICAL_SECTION m_crisec;
};
#endif