#include "AvCodecSetting.h"
#include "MainDlg.h"
#include "../Tool/cchinesecode.h"
#include "../SqlOp/HistoryInfoMgr.h"
#include <helper/SAdapterBase.h>
#include <control/SComboView.h>
#include "KKPlayer.h"
namespace SOUI
{

	/// 1 h264   2 h265

	class CVideoCodecAdapter :public SAdapterBase
	{
		public:
			   CVideoCodecAdapter (SListView  *m_Ctr,int VideoType);
			   ~CVideoCodecAdapter ();
			    int getCount();   
                void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate);
				bool OnItemClick(EventArgs *pEvt);
                SStringT getItemDesc(int position);
	    private:
		       SListView*            m_Ctr;
			   std::vector<SStringT> m_CodecList;
			   int                   m_nVideoType;
	};

	CVideoCodecAdapter::CVideoCodecAdapter (SListView  *m_Ctr,int VideoType):m_nVideoType(VideoType)
	{
		  if(m_nVideoType==1){
			   m_CodecList.push_back(L"Ä¬ÈÏ½âÂëÆ÷");
			   m_CodecList.push_back(L"DXVA2½âÂëÆ÷");
			   m_CodecList.push_back(L"InterQSV½âÂëÆ÷");
		  }else if(m_nVideoType==2){
		       m_CodecList.push_back(L"Ä¬ÈÏ½âÂëÆ÷");
			   m_CodecList.push_back(L"InterQSV½âÂëÆ÷");
		  }
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
        
		CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
	    SComboView* H264Box=(SComboView* )this->FindChildByName(L"cbx_H264Codec");
		SListView*  H264View=H264Box->GetListView();
		CVideoCodecAdapter* h264Adapter = new CVideoCodecAdapter(H264View,1);
        H264View->SetAdapter(h264Adapter);
		 

		H264Box->SetCurSel(InfoMgr->GetH264Codec());

		SComboView* H265Box=(SComboView* )this->FindChildByName(L"cbx_H265Codec");
		SListView*  H265View=H265Box->GetListView();

		CVideoCodecAdapter* h265Adapter = new CVideoCodecAdapter(H265View,2);
		H265View->SetAdapter(h265Adapter);

		
		H265Box->SetCurSel(InfoMgr->GetH265Codec());
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

		CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
		InfoMgr->UpdataH264Codec(med);
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
		     med= SKK_VideoState::HARD_CODE_QSV;
		
		CHistoryInfoMgr *InfoMgr=CHistoryInfoMgr::GetInance();
		InfoMgr->UpdataH265Codec(med);
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