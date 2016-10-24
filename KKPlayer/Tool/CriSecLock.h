#ifndef CriSecLock_H_
#define CriSecLock_H_
#include "../stdafx.h"
/***零界值***/
class CriSecLock
{
public:
	CriSecLock(DWORD dwSpinCount = 4096);
	~CriSecLock();
	void Lock();
	void Unlock();
	BOOL TryLock();
	DWORD SetSpinCount(DWORD dwSpinCount);

	CRITICAL_SECTION* GetObject();

private:
	CriSecLock(const CriSecLock& cs);
	CriSecLock operator = (const CriSecLock& cs);

private:
	//临界值
	CRITICAL_SECTION m_crisec;
};
#endif