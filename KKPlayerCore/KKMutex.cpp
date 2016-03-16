/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£º2015-6-25**********************************************/
#include "stdafx.h"
#include "KKMutex.h"
CKKMutex::CKKMutex(void)
{
#ifdef WIN32_KK
	m_KKMutex=::CreateMutex(NULL,FALSE,NULL);
#else
	pthread_mutex_init(&m_KKMutex, NULL);
#endif
}

void CKKMutex::Lock()
{
#ifdef WIN32_KK
	::WaitForSingleObject(m_KKMutex, INFINITE);
#else
	pthread_mutex_lock(&m_KKMutex);
#endif
}
void CKKMutex::UnLock()
{
#ifdef WIN32_KK
    ::ReleaseMutex(m_KKMutex);
#else
     pthread_mutex_unlock(&m_KKMutex);
#endif
}
#ifndef WIN32_KK
pthread_mutex_t* CKKMutex::operator&()
{
	return &m_KKMutex;
}
#endif
CKKMutex::~CKKMutex(void)
{
#ifdef WIN32_KK
	::CloseHandle(m_KKMutex);
#else
	pthread_mutex_destroy(&m_KKMutex);
#endif	
}
