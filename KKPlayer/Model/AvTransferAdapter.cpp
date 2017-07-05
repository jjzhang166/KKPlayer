#include "AvTransferAdapter.h"
namespace SOUI
{
	CDownAVListMcAdapterFix::CDownAVListMcAdapterFix(CKKmclv *pSMCListView)
	{
	
	}
	CDownAVListMcAdapterFix::~CDownAVListMcAdapterFix()
	{
	
	}
	int CDownAVListMcAdapterFix::getCount()
	{
	   return 0;
	}
    SStringT getSizeText(DWORD dwSize)
	{
	   return _T("");
	}
	bool OnItemClick(EventArgs *pEvt)
	{
	   return 0;
	}
	void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
	{
	   return ;
	}
    bool OnButtonClick(EventArgs *pEvt)
	{
	   return 1;
	}
			SStringW GetColumnName(int iCol) const;
			
			bool OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols);
}