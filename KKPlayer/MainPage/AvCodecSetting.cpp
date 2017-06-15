#include "AvCodecSetting.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"
#include <helper/SAdapterBase.h>
#include <control/SComboView.h>
#include "KKPlayer.h"
namespace SOUI
{


	class CVideoCodecAdapter :public SAdapterBase
	{
		public:
			   CVideoCodecAdapter (SListView  *m_Ctr);
			   ~CVideoCodecAdapter ();
			    int getCount();   
                void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate);
				bool OnItemClick(EventArgs *pEvt);
                SStringT getItemDesc(int position);
	    private:
		       SListView* m_Ctr;
			   std::vector<SStringT> m_CodecList;
	};

	CVideoCodecAdapter::CVideoCodecAdapter (SListView  *m_Ctr)
	{
		   m_CodecList.push_back(L"Ä¬ÈÏ½âÂëÆ÷");
		   m_CodecList.push_back(L"DXVA2½âÂëÆ÷");
		   m_CodecList.push_back(L"InterQSV½âÂëÆ÷");
	}
    CVideoCodecAdapter::~CVideoCodecAdapter ()
	{
	
	}
    int CVideoCodecAdapter::getCount()
	{
		return m_CodecList.size();
	}
    void CVideoCodecAdapter::getView(int pos, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
	    if(pItem->GetChildrenCount()==0){
			pItem->InitFromXml(xmlTemplate);
		}
		SStringT codecname=this->m_CodecList.at(pos);
		pItem->FindChildByName("StrCodecName")->SetWindowText(codecname);
	}
	bool CVideoCodecAdapter::OnItemClick(EventArgs *pEvt)
	{
        //CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
	    return true;
	}
    SStringT CVideoCodecAdapter::getItemDesc(int pos)
	{
	     SStringT codecname=this->m_CodecList.at(pos);
		 return codecname;
	}


	CAvCodecSetting::CAvCodecSetting(): SDomodalwnd(_T("LAYOUT:XML_AvCodecSetting"))
	{
           
	}
	CAvCodecSetting::~CAvCodecSetting()
	{
        
	}
	void CAvCodecSetting::Init()
	{
	    SComboView* H264Box=(SComboView* )this->FindChildByName(L"cbx_H264Codec");
		SListView*  H264View=H264Box->GetListView();
		CVideoCodecAdapter* h264Adapter = new CVideoCodecAdapter(H264View);
        H264View->SetAdapter(h264Adapter);
		 

		H264Box->SetCurSel(atoi(selectIndex.c_str()));

		SComboView* H265Box=(SComboView* )this->FindChildByName(L"cbx_H265Codec");
		SListView*  H265View=H265Box->GetListView();

		CVideoCodecAdapter* h265Adapter = new CVideoCodecAdapter(H265View);
		H265View->SetAdapter(h265Adapter);

		InfoMgr->GetConfig("H265Codec",selectIndex);
		H265Box->SetCurSel(atoi(selectIndex.c_str()));
	}
	
	BOOL CAvCodecSetting::OnH264BoxChanged(EventArgs *pEvt)
	{

		EventCBSelChange *pEs=(EventCBSelChange *)pEvt;
		int pos=pEs->nCurSel;
        int med=0;
		if(pos==0)
		     med= SKK_VideoState::HARD_CODE_NONE;
		else if(pos==1)
		     med= SKK_VideoState::HARD_CODE_DXVA;
		else if(pos==2)
			 med= SKK_VideoState::HARD_CODE_QSV;

        char hardM[512]="";
		sprintf(hardM,"%d",med);

		CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
		InfoMgr->UpdataConfig("H264Codec",hardM);
	    return TRUE;
	}
    BOOL CAvCodecSetting::OnH265BoxChanged(EventArgs *pEvt)
	{
		EventCBSelChange *pEs=(EventCBSelChange *)pEvt;
		int pos=pEs->nCurSel;
        int med=0;
		if(pos==0)
		     med= SKK_VideoState::HARD_CODE_NONE;
		else if(pos==1)
		     med= SKK_VideoState::HARD_CODE_DXVA;
		else if(pos==2)
			 med= SKK_VideoState::HARD_CODE_QSV;

        char hardM[512]="";
		sprintf(hardM,"%d",med);

		CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
		InfoMgr->UpdataConfig("H265Codec",hardM);
	    return TRUE;
	}
	void CAvCodecSetting::OnClose()
	{
		this->PostMessage(WM_CLOSE,0,0);
	}
	void CAvCodecSetting::OnMinimize()
	{
		
	}
}