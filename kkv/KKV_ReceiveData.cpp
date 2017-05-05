#include "KKV_ReceiveData.h"
#include "Qy_IPC_Context.h"
#include "Qy_Ipc_Manage.h"
#include <map>
#include <string>
#include <queue>
#include <process.h>
#include "json/json.h"
#include "Base64/Base64.h"
#include "md5/md5.h"
#include "../../KKPlayer/KKPlayerCore/KKPlugin.h"
#include "../../QyIPC/Qy_Ipc_Manage.h"
#include "KKVP.h"
#ifdef _DEBUG
    #pragma comment (lib,"QyIPC.lib")
    #pragma comment (lib,"jsoncpp.lib")
#else
    #pragma comment (lib,"QyIPC.lib")
    #pragma comment (lib,"jsoncpp.lib")
#endif
typedef unsigned char      uint8_t;
typedef long long          int64_t;

extern std::map<std::string,IPC_DATA_INFO>                        G_guidBufMap;
extern std::map<std::string,std::map<std::string,HANDLE>*>        G_URLRequestInfoMap;
extern std::map<std::string,std::string>                          G_SpeedInfoMap;
extern Qy_IPC::Qy_IPc_InterCriSec            G_KKMapLock;
extern std::map<std::string,unsigned int>    G_CacheTimeMap;
extern int G_IPC_Read_Write;

namespace Qy_IPC
{
	CKKV_DisConnect::CKKV_DisConnect()
	{
         
	}
	CKKV_DisConnect::~CKKV_DisConnect()
	{

	}
	void CKKV_DisConnect::DisConnct(HANDLE hPipeInst)
	{
            G_IPC_Read_Write=2;
	}

	CKKV_ReceiveData::CKKV_ReceiveData()
	{
		 m_hTheard=INVALID_HANDLE_VALUE;
		 m_Add=0;
	}
	CKKV_ReceiveData::~CKKV_ReceiveData()
	{
		if( m_hTheard==INVALID_HANDLE_VALUE)
		CloseHandle(m_hTheard);
	}

	void HandleIPCMsg(std::string guidstr,int msgId,unsigned char *dataBuf,long long dataLen,unsigned int CacheTime)
	{
		G_KKMapLock.Lock();
		//read消息
		if(msgId==IPCRead)
		{
			std::map<std::string,IPC_DATA_INFO>::iterator It= G_guidBufMap.find(guidstr);
			if(It!=G_guidBufMap.end())
			{
				if(dataLen>It->second.BufLen)
				{
				    int i=0;
					i++;
				}
				It->second.DataSize=dataLen;
				It->second.CacheTime=CacheTime;
				if(dataLen==-1)//暂时没有数据
				{

				}else if(dataLen==-2){ //数据已经读取完了

				}else if(dataLen==-1000){ //还未准备好

				}else if(dataLen==-1001){
				
				}else if(dataLen>0&&It->second.pBuf!=NULL){
					memcpy(It->second.pBuf, dataBuf, dataLen);
				}
				
			}

		}else if(msgId==IPCSeek){//seek 消息

			std::map<std::string,IPC_DATA_INFO>::iterator It= G_guidBufMap.find(guidstr);
			if(It!=G_guidBufMap.end())
			{
				It->second.DataSize=dataLen;
			}
		}else if(msgId==3){//down message

		}else if(msgId==4){//stop down

		}else if(msgId==IPCSpeed){//speed
			std::map<std::string,std::string>::iterator It= G_SpeedInfoMap.find(guidstr);
			if(It!=G_SpeedInfoMap.end())
			{
				It->second=(char*)dataBuf;
			}else{
				G_SpeedInfoMap.insert(std::pair<std::string,std::string>(guidstr,(char*)dataBuf));
			}
		}else if(msgId==6){

			
		}else if(msgId==IPCURLParser)
		{
		    std::map<std::string,IPC_DATA_INFO>::iterator It= G_guidBufMap.find(guidstr);
			if(It!=G_guidBufMap.end())
			{
			    char *jsonstr= (char*)dataBuf;
				int len=strlen(jsonstr);
				if(len>0)
				{
					len+=10;
					char *bb =(char*)::malloc(len);
					memset(bb,0,len);
					strcpy(bb,jsonstr);
					It->second.pBuf=bb;
				}
			}
		}

		G_KKMapLock.Unlock();
	}
	void CKKV_ReceiveData::HandelReceiveData(char *buf,int Len,void* strId)
	{

		Json::Reader JsReader;
		Json::Value JsValue;
		if(JsReader.parse(buf,JsValue)){
			int IPCMSG=0;
			long long DataLen=0;
			unsigned int CacheTime=0;
			if(JsValue["IPCMSG"].isInt())
				IPCMSG=JsValue["IPCMSG"].asInt();
			HANDLE HRW=(HANDLE)JsValue["HRW"].asInt();
			std::string guidstr=JsValue["Guid"].asString();

			if(IPCMSG==IPCRead){
				DataLen=JsValue["DataLen"].asInt64();
					std::string DataHexStr=JsValue["DataHex"].asString();
					int DataBufLen=DataLen*2;
					unsigned char* DataBuf=NULL;
					if(DataLen>0){
					    DataBuf=(unsigned char*)::malloc(DataBufLen);
						unsigned long uOutLen=DataBufLen;
					    bool DecOk=CBase64::Decode(DataHexStr,DataBuf,&uOutLen);
					}
					HandleIPCMsg(guidstr,IPCMSG,DataBuf,DataLen, CacheTime);
					
					if(DataBuf!=NULL)
						free(DataBuf);
					DataBuf=NULL;

			}else if(IPCMSG==IPCSeek){
				   DataLen=JsValue["Ret"].asInt64();
			       HandleIPCMsg(guidstr,IPCMSG,0,DataLen, CacheTime);
			}else if(IPCMSG==IPCURLParser){
			
			       std::string UrlJson=JsValue["UrlJson"].asString();
				   HandleIPCMsg(guidstr,IPCURLParser,(unsigned char*)UrlJson.c_str(),UrlJson.length(),0);
			}if(IPCMSG==IPCSpeed){	
				   guidstr=JsValue["Url"].asString();
			       std::string JsonSpeedInfo=JsValue["SpeedInfo"].asString();
				   HandleIPCMsg(guidstr,IPCSpeed,(unsigned char*)JsonSpeedInfo.c_str(),JsonSpeedInfo.length(),0);
			}
			if(HRW!=0)
			::SetEvent(HRW);
		}
	}
}