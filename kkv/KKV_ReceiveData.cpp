#include "KKV_ReceiveData.h"
#include "Qy_IPC_Context.h"
#include "Qy_Ipc_Manage.h"
#include <map>
#include <string>
#include <queue>
#include <process.h>
#include "Json/json/json.h"
#include "../../KKPlayer/KKPlayerCore/KKPlugin.h"
#ifdef _DEBUG
    #pragma comment (lib,"QyIPC\\Debug\\QyIPC.lib")
#else
    #pragma comment (lib,"QyIPC\\Release\\QyIPC.lib")
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

	void HandleIPCMsg(char* guidstr,int msgId,char *TempBuf,int dataLen)
	{
		//read消息
		if(msgId==1)
		{
			//data:guidlen+guid+msgId+h+FileInfoSize+FileInfo+datasize+data; 
			int FileInfoLen=0;
			memcpy(&FileInfoLen,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;

			char FileInfo[256]="";
			memcpy( FileInfo,TempBuf,FileInfoLen);
			TempBuf+=FileInfoLen;
			dataLen-=FileInfoLen;

			unsigned int CacheeTime=0;
			memcpy(&CacheeTime,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;

			int DataSize=0;
			memcpy(&DataSize,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;

			std::string guis(guidstr);
			std::map<std::string,IPC_DATA_INFO>::iterator It= G_guidBufMap.find(guis);
			if(It!=G_guidBufMap.end())
			{
				It->second.DataSize=DataSize;
				It->second.CacheTime=CacheeTime;
				if(DataSize==-1)//暂时没有数据
				{

				}else if(DataSize==-2){ //数据已经读取完了

				}else if(DataSize==-1000){ //还未准备好

				}else if(DataSize>0){
					memcpy(It->second.pBuf, TempBuf,DataSize);
				}
				DataSize++;
			}

		}else if(msgId==2){//seek 消息

			int FileLen=0;
			memcpy(&FileLen,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;

			char FileMD[32]="";
			memcpy( FileMD,TempBuf,4);
			TempBuf+=FileLen;
			dataLen-=FileLen;

			int SizeDate=0;
			memcpy(&SizeDate,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;

			std::string guis(guidstr);
			std::map<std::string,IPC_DATA_INFO>::iterator It= G_guidBufMap.find(guis);
			if(It!=G_guidBufMap.end())
			{
				It->second.DataSize=SizeDate;
			}
		}else if(msgId==3){//down message

		}else if(msgId==4){//stop down

		}else if(msgId==5){//speed
			
			int FileInfoLen=0;
			memcpy(&FileInfoLen,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;
            if(FileInfoLen>256 ||FileInfoLen<=0)
				return;

			char FileInfo[256]="";
			memcpy( FileInfo,TempBuf,FileInfoLen);
			TempBuf+=FileInfoLen;
			dataLen-=FileInfoLen;

			
			int DataSize=0;
			memcpy(&DataSize,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;
			
			if(DataSize>256||DataSize<=0)
				return;
			char *strchr=(char*)::malloc(DataSize);
			memset(strchr,0,DataSize);
			memcpy(strchr,TempBuf,DataSize);
			
			std::string guis(guidstr);
			std::map<std::string,IPC_DATA_INFO>::iterator It= G_guidBufMap.find(guis);
			if(It!=G_guidBufMap.end()){
				It->second.DataSize=DataSize;
				It->second.pBuf=strchr;
			}
		}else if(msgId==6){

			int FileInfoLen=0;
			memcpy(&FileInfoLen,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;
			if(FileInfoLen>256 ||FileInfoLen<=0)
				return;

			char FileInfo[256]="";
			memcpy( FileInfo,TempBuf,FileInfoLen);
			TempBuf+=FileInfoLen;
			dataLen-=FileInfoLen;


			unsigned int DataSize=0;
			memcpy(&DataSize,TempBuf,4);
			TempBuf+=4;
			dataLen-=4;

			std::string xxcc(FileInfo);
			std::map<std::string,unsigned int>::iterator Itxx=G_CacheTimeMap.find(xxcc);
            if(Itxx!=G_CacheTimeMap.end())
			{
                 Itxx->second=DataSize;
			}else{
				G_CacheTimeMap.insert(std::pair<std::string,unsigned int>(xxcc,DataSize));
			}
		}
	}
	void CKKV_ReceiveData::HandelReceiveData(char *buf,int Len,std::string strId)
	{
		char *TempBuf=buf;
		if(Len>4){
				int dataLen=0;
				memcpy(&dataLen,TempBuf,4);
				TempBuf+=4;
				dataLen-=4;
				if(dataLen>36){
									  int guidlen=0;
									  memcpy(&guidlen,TempBuf,4);
									  TempBuf+=4;
									  dataLen-=4;

									  char guidstr[64]="";
									  memcpy(guidstr,TempBuf,guidlen);
									  TempBuf+=guidlen;
									  dataLen-=guidlen;

									  if(dataLen>4){
												   //获取消息id
												   int msgId=0;
												   memcpy(&msgId,TempBuf,4);
												   TempBuf+=4;
												   dataLen-=4;
												   if(dataLen>4){
													   //获取事件
													   int h=0;
													   memcpy(&h,TempBuf,4);
													   TempBuf+=4;
													   dataLen-=4;
													   if(dataLen>4){
														   HandleIPCMsg(guidstr,msgId,TempBuf,dataLen);
													   }
													   ::SetEvent((HANDLE)h);
												   }
										  
									  }
				}
			}
		
	}
}