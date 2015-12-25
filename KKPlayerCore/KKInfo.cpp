/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£º2015-6-25**********************************************/
#include "stdafx.h"
#include "KKInfo.h"
CKKMutex::CKKMutex(void)
{
	m_KKMutex=NULL;
	m_KKMutex=::CreateMutex(NULL,FALSE,NULL);
}

void CKKMutex::Lock()
{
	::WaitForSingleObject(m_KKMutex, INFINITE);
}
void CKKMutex::UnLock()
{
    ::ReleaseMutex(m_KKMutex);
}
CKKMutex::~CKKMutex(void)
{
	::CloseHandle(m_KKMutex);
	
}
