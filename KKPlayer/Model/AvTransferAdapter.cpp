#include "AvTransferAdapter.h"
#include "../Control/kkmclv.h"
#include "../KKPlayerCore/KKPlayer.h"
#include "../MainPage/AVTransferDlg.h"
#include "json/json.h"
#include "../Tool/cchinesecode.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <math.h>
#include <iosfwd>
namespace SOUI
{
	char strabce[64]="";
	class DataItem
	{
	public:
		DataItem()
		{
		     speed=0;
	         progress=0;
             filesize=0;
	          Url=L"";
		}
		~DataItem(){}
	   int  speed;
	   long long  progress;
	   long long  filesize;
	   std::wstring  Url;
	};
	static std::vector<DataItem> DataItemVector;

	std::string format_thousands_separator(double val)
{
	std::stringstream stream;
	std::string strRet;
	stream.clear();
	stream.imbue(std::locale("chs"));
	stream << std::setprecision(3)<< val;//<<std::fixed << val;
	stream >> strRet;
	return strRet;//std::move(strRet);
}

	char * GetFormtDByMB(char *strabce,long long FileSize)
	{
		
		double dfileSize=0;
		if(FileSize>1024*1024*1024)
		{
			dfileSize=(double)FileSize/(1024*1024*1024);
			sprintf(strabce,"%sGb",format_thousands_separator(dfileSize).c_str());

		}else if(FileSize>1024*1024)
		{
			dfileSize=(double)FileSize/(1024*1024);
			sprintf(strabce,"%sMb",format_thousands_separator(dfileSize).c_str());
		}else{
			dfileSize=(double)FileSize/1024;
			dfileSize+=(FileSize%1024)/1024;
			sprintf(strabce,"%sKb",format_thousands_separator(dfileSize).c_str());
		}
		return strabce;
	}
	



	char * GetFormtDBySpeed(char *strabce,int FileSize)
	{

		double dfileSize=0;
		if(FileSize>1024*1024)
		{
			dfileSize=FileSize/(1024*1024);
			sprintf(strabce,"%s Mb/s",format_thousands_separator(dfileSize).c_str());
		}else if(FileSize>1024){
			dfileSize=(double)FileSize/1024;
			sprintf(strabce,"%s Kb/s",format_thousands_separator(dfileSize).c_str());
		}else{
			sprintf(strabce,"%d B/s",FileSize);
			
		}
		return strabce;
	}
	CDownAVListMcAdapterFix::CDownAVListMcAdapterFix(SMCListView *pSMCListView)
	{
	    m_pSMCListView=pSMCListView;
	}
	CDownAVListMcAdapterFix::~CDownAVListMcAdapterFix()
	{
	
	}
	void  CDownAVListMcAdapterFix::Refresh()
	{
	     std::list<KKPluginInfo>  &PlList=KKPlayer::GetKKPluginInfoList();
		 std::list<KKPluginInfo>::iterator It=PlList.begin();
		  wchar_t temp[2048]=L"";
		  DataItemVector.clear();
		 for(;It!=PlList.end();++It)
		 {
			 if(It->KKAllAVFilesSpeedInfo){
				 char *Out=0;
				 It->KKAllAVFilesSpeedInfo(&Out);
				
				 if(Out){

					 Json::Value jsonValue;
					 Json::Reader jsonReader;
					 if(jsonReader.parse(Out,jsonValue))
					 {
						for(int i=0;i<jsonValue.size();i++)
						{
						     Json::Value  speedinfo=jsonValue[i]["speedinfo"];
							 std::string url=jsonValue[i]["url"].asString();
                             memset(temp,0,2048);
							 CChineseCode::charTowchar(url.c_str(),temp, 2048) ; /**/

							 DataItem Item;
							 Item.Url=temp;
							 Item.filesize=speedinfo["filesize"].asInt64();
							 Item.speed=speedinfo["speed"].asInt();
							 Item.progress=speedinfo["progress"].asInt64();/* */
							 DataItemVector.push_back(Item);
						}
					 }
					 It->KKFree(Out);
				 }
			 }
		 }
	}
	int CDownAVListMcAdapterFix::getCount()
	{
		// Refresh();
	     return DataItemVector.size();
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
	      /*  int index=m_pSMCListView->GetSel();
			SItemPanel * pOldSItme=m_pSMCListView->GetItem(index);
			
			SItemPanel * pSItme=(SItemPanel *)pEvt->sender;*/
			return true;
	}
	bool CDownAVListMcAdapterFix::OnItemDelete(EventArgs *pEvt)
	{
	  return true;
	}
    bool  CDownAVListMcAdapterFix::OnItemPause(EventArgs *pEvt)
	{
	return true;
	}
	bool  CDownAVListMcAdapterFix::OnItemOpenLocal(EventArgs *pEvt)
    {
	return true;
	}
	void CDownAVListMcAdapterFix::getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
	{
		int len=2*1024*4;
		char *temp2=(char*)::malloc(4096);
		wchar_t  *temp = (wchar_t  *)::malloc(len);
	    if(pItem->GetChildrenCount()==0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
	   if(DataItemVector.size()>position)
	   {
			 DataItem &Item=DataItemVector.at(position);
			 SWindow  *pWin= pItem->FindChildByName(L"txt_name");
			 if(pWin)
			 {
				 int index= Item.Url.find_last_of (_T("\\"));
				if(index<0)
				{
					index=Item.Url.find_last_of(L"/");
				}
				index+=1;
				Item.Url=Item.Url.substr(index,Item.Url.length()-index);
				pWin->SetWindowText(Item.Url.c_str());
			 }
	         
			 
			 
			 pWin= pItem->FindChildByName(L"txt_size");
			 if(pWin){
				 memset(temp,0,len);
				 
				 memset(temp2,0,4096);
				 GetFormtDByMB(temp2,Item.filesize);
				 CChineseCode::charTowchar(temp2,temp,1024) ; /**/
				 SStringT Tip=temp;

				 GetFormtDByMB(temp2,Item.progress);
				 CChineseCode::charTowchar(temp2,temp,1024) ; /**/
                 Tip=_T("/")+Tip;
				 Tip=temp+Tip;
				 pWin->SetWindowText(Tip);
                 
			 }
			 pWin= pItem->FindChildByName(L"txt_speed");
			 if(pWin){
				 memset(temp,0,len);	 
				 memset(temp2,0,4096);
				 GetFormtDBySpeed(temp2,Item.speed);
				 CChineseCode::charTowchar(temp2,temp,1024) ; /**/
				 pWin->SetWindowText(temp);
			 } 
             pWin= pItem->FindChildByName(L"btn_pause");
			 if(pWin){
							
			 }
             pWin= pItem->FindChildByName(L"btn_delete");
			 if(pWin){
					 pWin->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CDownAVListMcAdapterFix::OnItemDelete, this));
			 }
	   }
	   ::free(temp2);
	   ::free(temp);
	   return ;
	}
    bool CDownAVListMcAdapterFix::OnButtonClick(EventArgs *pEvt)
	{
	   return 1;
	}
	SStringW CDownAVListMcAdapterFix::GetColumnName(int iCol) const
	{
	  return SStringW().Format(L"col%d",iCol+1);
	}
	bool CDownAVListMcAdapterFix::OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols)
	{
	   return true;
	}
}