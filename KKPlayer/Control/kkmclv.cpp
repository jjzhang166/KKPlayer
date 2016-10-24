#include "stdafx.h"
#include "kkmclv.h"
namespace SOUI{
	
	CKKmclv::CKKmclv()
	{

	}
	CKKmclv::~CKKmclv()
	{

	}
	SItemPanel * CKKmclv::GetItem(int iItem)
	{
		return this->GetItemPanel(iItem);
	}
	void  CKKmclv::DataSetInvalidated()
	{
	      this->onDataSetInvalidated();
	}
}