#include "KKCond_t.h"

CKKCond_t::CKKCond_t(void)
{
#ifdef WIN32
	m_hWait=::CreateEvent(NULL,TRUE,FALSE,NULL);//手动复位，初始状态无效
#else
	m_hWait=0;
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
	m_hWait=0;
	return m_hWait;
#endif
}
int CKKCond_t::SetCond()
{
#ifdef WIN32
	return ::SetEvent(m_hWait);
#else
	m_Mutex.Lock();
	m_hWait=1;
	m_Mutex.UnLock();
	return m_hWait;
#endif
}
int CKKCond_t::WaitCond(int ms)
{
	#ifdef WIN32
		return 	::WaitForSingleObject(m_hWait, INFINITE);
	#else
		m_Mutex.Lock();
		while(m_hWait==0)
		{
			m_Mutex.UnLock();
			Sleep(1);
			m_Mutex.Lock();
		}
		m_Mutex.UnLock();
		return m_hWait;
	#endif
}