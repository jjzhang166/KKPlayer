#include "KKCond_t.h"

CKKCond_t::CKKCond_t(void)
{
#ifdef WIN32
	m_hWait=::CreateEvent(NULL,TRUE,FALSE,NULL);//手动复位，初始状态无效
#else
	pthread_cond_init(&m_hWait,NULL);
#endif 
}

CKKCond_t::~CKKCond_t(void)
{

}

int CKKCond_t::ResetCond()
{
#ifdef WIN32
	return ::ResetEvent(m_hWait);
#else
	int ret = pthread_cond_init(&m_hWait,NULL);
	return ret;
#endif
}
int CKKCond_t::SetCond()
{
#ifdef WIN32
	return ::SetEvent(m_hWait);
#else
	m_Mutex.Lock();
	int ret=  pthread_cond_broadcast(&m_hWait);
	m_Mutex.UnLock();
#endif
}
int CKKCond_t::WaitCond(int ms)
{
	#ifdef WIN32
		return 	::WaitForSingleObject(m_hWait, INFINITE);
	#else
		m_Mutex.Lock();
		int ret =pthread_cond_wait(&m_hWait,&m_Mutex);
		m_Mutex.UnLock();
		return ret;
	#endif
}