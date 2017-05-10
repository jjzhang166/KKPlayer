#include "HistoryAdapter.h"
namespace SOUI
{
	CHistoryAdapterFix::CHistoryAdapterFix()
	{
	
	}
	CHistoryAdapterFix::~CHistoryAdapterFix()
	{
	
	}
	int          CHistoryAdapterFix::getCount()
	{
	     return 0;
	}
	bool         CHistoryAdapterFix::OnButtonClick(EventArgs *pEvt)
	{
	   return true;
	}
	void         CHistoryAdapterFix::getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
	{
	
	}
	bool         CHistoryAdapterFix::OnCbxSelChange(EventArgs *pEvt)
	{
	   return true;
	}
}