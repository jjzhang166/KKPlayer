#include "AvTransferAdapter.h"
namespace SOUI
{
	char strabce[64]="";
	char * GetFormtDByMB(char *strabce,int FileSize)
	{
		
		double dfileSize=0;
		if(FileSize>1024*1024*1024)
		{
			dfileSize=(double)FileSize/(1024*1024*1024);
			sprintf(strabce,"%.3fGB",dfileSize);

		}else if(FileSize>1024*1024)
		{
			dfileSize=(double)FileSize/(1024*1024);
			sprintf(strabce,"%.3fMB",dfileSize);
		}else{
			dfileSize=(double)FileSize/1024;
			dfileSize+=(FileSize%1024)/1024;
			sprintf(strabce,"%.2fKB",dfileSize);
		}
		return strabce;
	}
	char * GetFormtDBySpeed(char *strabce,int FileSize)
	{

		double dfileSize=0;
		if(FileSize>1024*1024)
		{
			dfileSize=(double)FileSize/(1024*1024);
			sprintf(strabce,"%.2f MB/s",dfileSize);
		}else if(FileSize>1024){
			
			sprintf(strabce,"%d KB/s",FileSize/1024);
		}else{
			dfileSize=(double)FileSize;
            sprintf(strabce,"%d B/s",FileSize);
			
		}
		return strabce;
	}
	CDownAVListMcAdapterFix::CDownAVListMcAdapterFix(CKKmclv *pSMCListView)
	{
	    m_pSMCListView=pSMCListView;
	}
	CDownAVListMcAdapterFix::~CDownAVListMcAdapterFix()
	{
	
	}
	int CDownAVListMcAdapterFix::getCount()
	{
	   return 0;
	}
    SStringT CDownAVListMcAdapterFix::getSizeText(DWORD dwSize)
	{
	        int num1=dwSize/(1<<20);
			dwSize -= num1 *(1<<20);
			int num2 = dwSize*100/(1<<20);
			return SStringT().Format(_T("%d.%02dM"),num1,num2);
	}
	bool CDownAVListMcAdapterFix::OnItemClick(EventArgs *pEvt)
	{
	        int index=m_pSMCListView->GetSel();
			SItemPanel * pOldSItme=m_pSMCListView->GetItem(index);
			
			SItemPanel * pSItme=(SItemPanel *)pEvt->sender;
			return true;
	}
	void CDownAVListMcAdapterFix::getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
	{
	   return ;
	}
    bool CDownAVListMcAdapterFix::OnButtonClick(EventArgs *pEvt)
	{
	   return 1;
	}
	SStringW CDownAVListMcAdapterFix::GetColumnName(int iCol) const
	{
	   return _T("");
	}
	bool CDownAVListMcAdapterFix::OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols)
	{
	   return true;
	}
}