#include "AvRendererSetting.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"
#include <helper/SAdapterBase.h>
#include <control/SComboView.h>
#include "KKPlayer.h"
namespace SOUI
{

	class CAvRendererAdapter :public SAdapterBase
	{
		public:
			   CAvRendererAdapter (SListView  *m_Ctr);
			   ~CAvRendererAdapter ();
			    int getCount();   
                void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate);
				bool OnItemClick(EventArgs *pEvt);
                SStringT getItemDesc(int position);
	    private:
		       SListView*            m_Ctr;
			   std::vector<SStringT> m_AvRendererList;
			   int                   m_nVideoType;
	};

	CAvRendererAdapter::CAvRendererAdapter (SListView  *m_Ctr)
	{
			   m_AvRendererList.push_back(L"GDI渲染");
			   m_AvRendererList.push_back(L"d3d9兼容模式(DXVA2解码软渲染)");
			   m_AvRendererList.push_back(L"d3d9高速模式(DXVA2解码硬件渲染)");
	}
    CAvRendererAdapter::~CAvRendererAdapter ()
	{
	
	}
    int CAvRendererAdapter::getCount()
	{
		return m_AvRendererList.size();
	}
    void CAvRendererAdapter::getView(int pos, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
	    if(pItem->GetChildrenCount()==0){
			pItem->InitFromXml(xmlTemplate);
		}
		SStringT codecname=this->m_AvRendererList.at(pos);
		pItem->FindChildByName("StrRendererName")->SetWindowText(codecname);
	}
	bool CAvRendererAdapter::OnItemClick(EventArgs *pEvt)
	{
	    return true;
	}
    SStringT CAvRendererAdapter::getItemDesc(int pos)
	{
	     SStringT codecname=this->m_AvRendererList.at(pos);
		 return codecname;
	}





	CAvRendererSetting::CAvRendererSetting(): SDomodalwnd(_T("LAYOUT:XML_AvRendererSetting"))
	{
           
	}
	CAvRendererSetting::~CAvRendererSetting()
	{
        
	}
	void CAvRendererSetting::Init()
	{
       
		SComboView* cbx_AvRenderer=(SComboView* )this->FindChildByName(L"cbx_AvRenderer");
		SListView*   AvRendererView=cbx_AvRenderer->GetListView();
		CAvRendererAdapter* RendererAdapter = new CAvRendererAdapter( AvRendererView);
        AvRendererView->SetAdapter(RendererAdapter);
	}
	
	
	void CAvRendererSetting::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAvRendererSetting::OnMinimize()
	{
		
	}
}