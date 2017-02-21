#include "IQy_Ipc_Base.h"
#include "Qy_Ipc_Manage.h"
#ifndef KKV_ReceiveData_H_
#define KKV_ReceiveData_H_
typedef struct __IPC_DATA_INFO{
	void *pBuf;
	int BufLen;
	unsigned int DataSize;
	HANDLE hWait;
	unsigned int CacheTime;
}IPC_DATA_INFO;
namespace Qy_IPC
{
	class CKKV_DisConnect:public IQy_IPC_DisConnect
	{
	public:
		CKKV_DisConnect();
		~CKKV_DisConnect();
		void DisConnct(HANDLE hPipeInst);
	};
	class CKKV_ReceiveData:public IQy_HandelReceiveData
	{ 
		   public:
				  CKKV_ReceiveData();
				  ~CKKV_ReceiveData();
				   virtual void HandelReceiveData(char *buf,int Len,void* strId);
	       private:
			        UINT m_Add;
					HANDLE m_hTheard;
	};
}
#endif