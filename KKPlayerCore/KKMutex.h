/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£º2015-6-25**********************************************/
#include "stdafx.h"
#ifndef KK_INFO_H_
#define KK_INFO_H_

class CKKMutex
{
    public:
		    CKKMutex();
			~CKKMutex();
			void Lock();
			void UnLock();
    private:
#ifdef WIN32
	        HANDLE m_KKMutex;
#else
		    pthread_mutex_t m_KKMutex;
#endif
};
#endif