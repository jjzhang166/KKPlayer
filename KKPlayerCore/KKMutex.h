/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£º2015-6-25**********************************************/
#include "stdafx.h"
#ifndef KKMutex_H_
#define KKMutex_H_

class CKKMutex
{
    public:
		    CKKMutex();
			~CKKMutex();
			void Lock();
			void UnLock();
#ifndef WIN32_KK
			pthread_mutex_t* operator&();
#endif
    private:
#ifdef WIN32_KK
	        HANDLE m_KKMutex;
#else
	
		    pthread_mutex_t m_KKMutex;
#endif
};
#endif