#ifndef KK_mclv_H_
#define KK_mclv_H_
#include "../stdafx.h"
#include <helper/SAdapterBase.h>
#include "control/SMCListView.h"
namespace SOUI{
	class CKKmclv:public SMCListView
	{
         SOUI_CLASS_NAME(  CKKmclv, L"kkmclv")
	public:
		CKKmclv();
		~CKKmclv();
		 SItemPanel * GetItem(int iItem);
         void DataSetInvalidated();
	};
}
#endif