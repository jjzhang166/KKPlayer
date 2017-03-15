#include "KKV_ReceiveData.h"
#include "Qy_IPC_Context.h"
#include "Qy_Ipc_Manage.h"
#include <map>
#include <string>
#include <queue>
#include <process.h>
#include "Json/json/json.h"
#include "Base64/Base64.h"
#include "md5/md5.h"
#include "../../KKPlayer/KKPlayerCore/KKPlugin.h"
#include "KKVP.h"
#ifdef _DEBUG
    #pragma comment (lib,"QyIPC.lib")
#else
    #pragma comment (lib,"QyIPC.lib")
#endif
typedef unsigned char      uint8_t;
typedef long long          int64_t;

extern std::map<std::string,IPC_DATA_INFO> G_guidBufMap;
extern std::map<std::string,unsigned int> G_CacheTimeMap;
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

	void HandleIPCMsg(std::string guidstr,int msgId,unsigned char *dataBuf,int dataLen,unsigned int CacheTime)
	{
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
				
				}else if(dataLen>0){
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

		}else if(msgId==5){//speed
			
			
		}else if(msgId==6){

			
		}
	}
	void CKKV_ReceiveData::HandelReceiveData(char *buf,int Len,void* strId)
	{

		Json::Reader JsReader;
		Json::Value JsValue;
		if(JsReader.parse(buf,JsValue)){
			int IPCMSG=0;
			int DataLen=0;
			unsigned int CacheTime=0;
			if(JsValue["IPCMSG"].isInt())
				IPCMSG=JsValue["IPCMSG"].asInt();
			HANDLE HRW=(HANDLE)JsValue["HRW"].asInt();
			std::string guidstr=JsValue["Guid"].asString();

			if(IPCMSG==IPCRead){
					DataLen=JsValue["DataLen"].asInt();
					std::string DataHexStr=JsValue["DataHex"].asString();
					int DataBufLen=DataLen*2;
					unsigned char* DataBuf=NULL;
					if(DataLen>0){
					    DataBuf=(unsigned char*)::malloc(DataBufLen);
						unsigned long uOutLen=DataBufLen;
					    bool DecOk=CBase64::Decode(DataHexStr,DataBuf,&uOutLen);

						 char md5buf[512]="";
					     MD5Data (DataBuf,uOutLen, md5buf);
						 std::string DataMd5=JsValue["datamd5"].asString();
						 if(DataMd5!=md5buf)
						 {
						    int ii=0;
							ii++;
						 }/**/
						 
					}
					HandleIPCMsg(guidstr,IPCMSG,DataBuf,DataLen, CacheTime);
					
					if(DataBuf!=NULL)
						free(DataBuf);
					DataBuf=NULL;

			}else if(IPCMSG==IPCSeek){
				   DataLen=JsValue["Ret"].asInt();
			       HandleIPCMsg(guidstr,IPCMSG,0,DataLen, CacheTime);
			}
			::SetEvent(HRW);
		}
	}
}