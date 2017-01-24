#include "AVDownManageDlg.h"
#include "MainDlg.h"
#include <helper/SAdapterBase.h>
#include "control/SMCListView.h"
#include "../Tool/cchinesecode.h"
#include "../control/kkmclv.h"
#include "../../KKPlayerCore/SqlOp/AVInfomanage.h"
extern SOUI::CMainDlg *m_pDlgMain;
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
			sprintf(strabce,"%.3fKB",dfileSize);
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
	//http://www.cnblogs.com/setoutsoft/p/4863555.html
	class CDownAVListMcAdapterFix : public SMcAdapterBase
	{
	private:
		CKKmclv *m_pSMCListView;
	public:
		CDownAVListMcAdapterFix(CKKmclv *pSMCListView)
		{
                 m_pSMCListView=pSMCListView;
		}

		virtual int getCount()
		{
			CAVDownManage* px= CAVDownManage::GetInstance();
			int counx=px->Count();
			return counx;
		}   

		SStringT getSizeText(DWORD dwSize)
		{
			int num1=dwSize/(1<<20);
			dwSize -= num1 *(1<<20);
			int num2 = dwSize*100/(1<<20);
			return SStringT().Format(_T("%d.%02dM"),num1,num2);
		}

		bool OnItemClick(EventArgs *pEvt)
		{
			int index=m_pSMCListView->GetSel();
			SItemPanel * pOldSItme=m_pSMCListView->GetItem(index);
			
			SItemPanel * pSItme=(SItemPanel *)pEvt->sender;
			return true;
		}
		virtual void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
		{
			if(pItem->GetChildrenCount()==0)
			{
				pItem->InitFromXml(xmlTemplate);
			}
			
			CAVDownManage* px= CAVDownManage::GetInstance();

			AV_DOWN_SPEED_INFO DownInfo;
			memset(&DownInfo,0,sizeof(AV_DOWN_SPEED_INFO));
			px->GetFileDownInfo2(position,DownInfo);
			SStringW xx=pItem->GetClassName();
			
			wchar_t abce[512]=L"";
			CChineseCode::charTowchar(DownInfo.url, abce,512);
			pItem->FindChildByName(L"file_name")->SetWindowText(abce);

			char strabce[64]="";
		
			GetFormtDByMB(strabce,DownInfo.FileSize);
			CChineseCode::charTowchar(strabce, abce,512);
			pItem->FindChildByName(L"file_Size")->SetWindowText(abce);



			SProgress* AVDownSlider=(SProgress*)pItem->FindChildByName(L"file_progress");
            AVDownSlider->SetRange(0,DownInfo.FileSize> 0?DownInfo.FileSize:1000);
			AVDownSlider->SetValue(DownInfo.AcSize);

			int Tick=::GetTickCount();
			if(Tick>DownInfo.Tick&&Tick<DownInfo.Tick+1000)
			{
				GetFormtDBySpeed(strabce,DownInfo.Speed);
				CChineseCode::charTowchar(strabce, abce,512);
				pItem->FindChildByName(L"file_Speed")->SetWindowText(abce);
			}else{
                pItem->FindChildByName(L"file_Speed")->SetWindowText(L"0KB/S");
			}
			
			SItemPanel *SItem=(SItemPanel *)pItem;
			SItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK,Subscriber(&CDownAVListMcAdapterFix::OnItemClick,this));
		}

		bool OnButtonClick(EventArgs *pEvt)
		{
			
		  int ixx=0;
		  ixx++;
		  ixx=ixx+1;
			return true;
		}

		SStringW GetColumnName(int iCol) const{
			return SStringW().Format(L"col%d",iCol+1);
		}

		struct SORTCTX
		{
			int iCol;
			SHDSORTFLAG stFlag;
		};

		bool OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols)
		{
			if(iCol==5) //最后一列“操作”不支持排序
				return false;

			return true;
		}

		static int __cdecl SortCmp(void *context,const void * p1,const void * p2)
		{
			
			return 0;
		}
	};
	CAVDownManageDlg::CAVDownManageDlg(): SHostWnd(_T("LAYOUT:XML_AVDownManage"))
	{
	
	}
	CAVDownManageDlg::~CAVDownManageDlg()
	{

	}
	
	BOOL CAVDownManageDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
	{
		//mclistview name="mclv_AVDownList
		CKKmclv* pAV=(CKKmclv* )FindChildByName(L"mclv_AVDownList");
	    IMcAdapter *pAdapter = new CDownAVListMcAdapterFix(pAV);
	    pAV->SetAdapter(pAdapter);
	    pAdapter->Release();/**/

		
		m_nTM_TICKER=123;
		BOOL xx=this->SetTimer(m_nTM_TICKER,500);
		return TRUE;
	}
	void CAVDownManageDlg::OnTimer(UINT_PTR nIDEvent)
	{
		if(nIDEvent==m_nTM_TICKER){
			CKKmclv* pAV=(CKKmclv* )FindChildByName(L"mclv_AVDownList");
			pAV->DataSetInvalidated();
			
		}
	}
	void CAVDownManageDlg::OnLButtonDown(UINT nFlags, CPoint point)
	{

		this->OnMouseEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));

		m_pDlgMain->ShowMiniUI(false);
	}


	
	void CAVDownManageDlg::OnClose()
	{
		ShowWindow(SW_HIDE);
	}
	void CAVDownManageDlg::OnMinimize()
	{

	}
}