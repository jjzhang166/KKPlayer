#include "HistoryAdapter.h"
#include "../Tool/cchinesecode.h"
extern SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
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
		return m_slQue.size();
	}
	bool         CHistoryAdapterFix::OnButtonClick(EventArgs *pEvt)
	{
	   return true;
	}
	void         CHistoryAdapterFix::getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
	{
	    AV_Hos_Info * pAVPic=m_slQue.at(position);
		if(pItem->GetChildrenCount()==0)
		{
			pItem->InitFromXml(xmlTemplate);
		}

		SItemPanel * pSItme=(SItemPanel *)pItem;
		pSItme->SetUserData((ULONG_PTR)pAVPic);
		pSItme->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_DBCLICK,Subscriber(&CHistoryAdapterFix::OnCbxSelChange,this));
		SImageWnd  *pAV_img= pItem->FindChildByName2<SImageWnd>(L"AV_img");
		std::map<int,IBitmap*>::iterator _It=m_BitMap.find(position);
		if(_It==m_BitMap.end())
		{
			BITMAPINFOHEADER header;
			header.biSize = sizeof(BITMAPINFOHEADER);
			int bpp=32;
			header.biWidth = pAVPic->width;
			header.biHeight = pAVPic->height*(-1);
			header.biBitCount = bpp;
			header.biCompression = 0;
			header.biSizeImage = 0;
			header.biClrImportant = 0;
			header.biClrUsed = 0;
			header.biXPelsPerMeter = 0;
			header.biYPelsPerMeter = 0;
			header.biPlanes = 1;


			//3 构造文件头
			BITMAPFILEHEADER bmpFileHeader;
			HANDLE hFile = NULL;
			DWORD dwTotalWriten = 0;
			DWORD dwWriten;

			bmpFileHeader.bfType = 0x4d42; //'BM';
			bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
			bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+ pAVPic->width*pAVPic->height*bpp/8;


			int Totl=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+pAVPic->bufLen;
			unsigned char *bufx=(unsigned char *)::malloc(Totl);
			unsigned char *bufx2=bufx;
			memcpy(bufx2,&bmpFileHeader, sizeof(BITMAPFILEHEADER));

			bufx2+=sizeof(BITMAPFILEHEADER);

			memcpy(bufx2,&header, sizeof(BITMAPINFOHEADER));
			bufx2+=sizeof(BITMAPINFOHEADER);
			memcpy(bufx2,pAVPic->pBuffer, pAVPic->bufLen);
			IBitmap *pImg=NULL;
			pRenderFactory->CreateBitmap(&pImg);
			HRESULT  ll=0;//pImg->LoadFromFile(L"D:/pic/0.bmp");

			ll=pImg->LoadFromMemory(bufx,Totl);
			/*IBitmap *pImg = new IBitmap();*/

			pAV_img->SetImage(pImg);

			m_BitMap.insert(std::pair<int,IBitmap*>(position,pImg));
			::free(bufx);
		}else
		{
			 pAV_img->SetImage(_It->second);
		}
		
		
        wchar_t strtmp[2048]=L"";
		std::wstring CurTimeStr;
		CChineseCode::charTowchar(pAVPic->url,strtmp,2048);
		CurTimeStr=strtmp;
		int index=CurTimeStr.find_last_of(L"\\");
		if(index<0)
			index=CurTimeStr.find_last_of(L"//");
		if(index>=0)
		{
			 index++;
             CurTimeStr=CurTimeStr.substr(index,CurTimeStr.length()-index);
			 index=CurTimeStr.find_last_of(L".");
             CurTimeStr=CurTimeStr.substr(0,index);
		}
		

		SStatic *pTxt = pItem->FindChildByName2<SStatic>(L"AV_name");
		pTxt->GetUserData();
		pTxt->SetWindowText(CurTimeStr.c_str());



		SStatic *pAV_CurTime= pItem->FindChildByName2<SStatic>(L"AV_CurTime");
		

		int h=(pAVPic->CurTime/(60*60));
		int m=(pAVPic->CurTime%(60*60))/(60);
		int s=((pAVPic->CurTime%(60*60))%(60));
		
		if(h<10){
			swprintf_s(strtmp,L"0%d:",h);
			CurTimeStr=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d:",h);
			CurTimeStr=strtmp;
		}
		if(m<10){
			swprintf_s(strtmp,L"0%d:",m);
			CurTimeStr+=strtmp;
		}
		else{				  
			swprintf_s(strtmp,L"%d:",m);
			CurTimeStr+=strtmp;
		}

		if(s<10){
			swprintf_s(strtmp,L"0%d",s);
			CurTimeStr+=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d",s);
			CurTimeStr+=strtmp;
		}
		pAV_CurTime->SetWindowText(CurTimeStr.c_str());


		h=pAVPic->TotalTime/(60*60);
		m=(pAVPic->TotalTime%(60*60))/60;
		s=((pAVPic->TotalTime%(60*60))%60);


		if(h<10)
		{
			swprintf_s(strtmp,L"0%d:",h);
			CurTimeStr=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d:",m);
			CurTimeStr=strtmp;
		}
		if(m<10){
			swprintf_s(strtmp,L"0%d:",m);
			CurTimeStr+=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d:",m);
			CurTimeStr+=strtmp;
		}
		if(s<10){
			swprintf_s(strtmp,L"0%d",s);
			CurTimeStr+=strtmp;
		}
		else{
			swprintf_s(strtmp,L"%d",s);
			CurTimeStr+=strtmp;
		}

		SStatic *pAV_Time= pItem->FindChildByName2<SStatic>(L"AV_Time");
		pAV_Time->SetWindowText(CurTimeStr.c_str());
	}
	bool CHistoryAdapterFix::OnCbxSelChange(EventArgs *pEvt)
	{
	     return true;
	}
}