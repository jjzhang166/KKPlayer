#include "KKCond_t.h"

CKKCond_t::CKKCond_t(void)
{
#ifdef WIN32_KK
	 m_hWait=::CreateEvent(NULL,TRUE,FALSE,NULL);//手动复位，初始状态无效
#else
	//m_hWait=0;
	 pthread_cond_init(&m_hWait,NULL);
#endif 
}

CKKCond_t::~CKKCond_t(void)
{
	#ifndef WIN32_KK
     pthread_cond_destroy(&m_hWait);
    #endif
}

int CKKCond_t::ResetCond()
{
#ifdef WIN32_KK
	return ::ResetEvent(m_hWait);
#else
	/*m_Mutex.Lock();
	m_hWait=0;
	m_Mutex.UnLock();*/
	return 0;
#endif
}
int CKKCond_t::SetCond()
{
#ifdef WIN32_KK
	return ::SetEvent(m_hWait);
#else
	/*m_Mutex.Lock();
	m_hWait=1;
	m_Mutex.UnLock();*/
	pthread_cond_broadcast(&m_hWait);
	return 0;//m_hWait;
#endif
}
int CKKCond_t::WaitCond(int ms)
{
	#ifdef WIN32_KK
		return 	::WaitForSingleObject(m_hWait, INFINITE);
	#else

	m_Mutex.Lock();
	pthread_cond_wait(&m_hWait,&m_Mutex);
	m_Mutex.UnLock();
		/*m_Mutex.Lock();
		while(m_hWait==0)
		{
			m_Mutex.UnLock();
			Sleep(1);
			m_Mutex.Lock();
		}
		m_Mutex.UnLock();*/
		return 0;// m_hWait;
	#endif
}