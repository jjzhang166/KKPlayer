#include "KKCond_t.h"
void *KK_Malloc_(size_t size);
void  KK_Free_(void *ptr);
CKKCond_t::CKKCond_t(void)
{
#ifdef WIN32_KK
	 m_hWait=::CreateEvent(NULL,TRUE,FALSE,NULL);//手动复位，初始状态无效
#else
	//m_hWait=0;
	 pthread_cond_init(&m_hWait,NULL);
#endif 
	 m_nWaiting=m_nSignals=0;
}

CKKCond_t::~CKKCond_t(void)
{
#ifdef WIN32_KK
	::CloseHandle(m_hWait);
#else
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
	//pthread_cond_broadcast(&m_hWait);
	pthread_cond_signal(&m_hWait);
	return 0;
#endif
}

void CKKCond_t::CondSignal()
{
    m_Mutex.Lock();
#ifdef WIN32_KK
	if(m_nWaiting>m_nSignals)
	{
	    ++m_nSignals;
		SetCond();
	}
#else
    SetCond(); 
#endif
	m_Mutex.UnLock();
}
int CKKCond_t::WaitCond(int ms,CKKLock *pLock)
{
	#ifdef WIN32_KK
	        m_Mutex.Lock();
	        ++m_nWaiting;
		    m_Mutex.UnLock();
            ResetCond();

			pLock->Unlock();
            int ret=::WaitForSingleObject(m_hWait, INFINITE);
			pLock->Lock();

	        m_Mutex.Lock();
			if ( m_nSignals > 0 )
			{
			    --m_nSignals;
			}
	        --m_nWaiting;
		    m_Mutex.UnLock();
		return ret;
	#else

	m_Mutex.Lock();
	//++m_nWaiting;
	//m_Mutex.UnLock();

	pLock->Unlock();
	int ret=pthread_cond_wait(&m_hWait,&m_Mutex);
//	SetCond();
	pLock->Lock();

	//m_Mutex.Lock();
	//--m_nWaiting;
	m_Mutex.UnLock();
	return ret;
	#endif
}
void*  CKKCond_t::operator new(size_t size )
{
	return KK_Malloc_(size);
}
void   CKKCond_t::operator delete(void *ptr)
{
     KK_Free_(ptr);
}