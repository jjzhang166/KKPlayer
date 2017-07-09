#include "AvTransferAdapter.h"
#include "../Control/kkmclv.h"
#include "../KKPlayerCore/KKPlayer.h"
#include "json/json.h"
#include "../Tool/cchinesecode.h"
#include <vector>
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
	   int  progress;
	   int  filesize;
	   SStringT  Url;
	};
	static std::vector<DataItem> DataItemVector;
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
						     /*Json::Value  speedinfo=jsonValue[i]["speedinfo"];
							 std::string url=jsonValue[i]["url"].asString();
                             memset(temp,0,2048);
							 CChineseCode::charTowchar(url.c_str(),temp, 2048) ;*/

							 DataItem Item;
							 Item.Url=temp;
							/*  Item.filesize=speedinfo["filesize"].asInt64();
							 Item.speed=speedinfo["speed"].asInt();
							 Item.speed=speedinfo["progress"].asInt64();*/
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
	void CDownAVListMcAdapterFix::getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
	{
	 
	   if(pItem->GetChildrenCount()==0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
	  if(DataItemVector.size()>position)
  DataItem &Item=DataItemVector.at(position);
	SWindow  *pName= pItem->FindChildByName(L"file_name");
	  if(pName)
	   pName->SetWindowText(L"xx");
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