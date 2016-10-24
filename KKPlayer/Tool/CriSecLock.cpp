#include "CriSecLock.h"
CriSecLock::CriSecLock(DWORD dwSpinCount)
{
		_ASSERTE(::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount));
}
CriSecLock::~CriSecLock()
{
		::DeleteCriticalSection(&m_crisec);
}
void CriSecLock::Lock()
{
	::EnterCriticalSection(&m_crisec);
}
void CriSecLock::Unlock()
{
	::LeaveCriticalSection(&m_crisec);
}
BOOL CriSecLock::TryLock()
{
	return ::TryEnterCriticalSection(&m_crisec);
}
DWORD CriSecLock::SetSpinCount(DWORD dwSpinCount) 
{
	return ::SetCriticalSectionSpinCount(&m_crisec, dwSpinCount);
}
CRITICAL_SECTION* CriSecLock::GetObject()
{
	return &m_crisec;
}