#include "../../QyIPC/Qy_Ipc_Manage.h"
#include "KKV_ReceiveData.h"
#include <map>
#include <string>
#include "../../KKPlayer/KKPlayerCore/KKPlugin.h"
#include "Json/json/json.h"
Qy_IPC::Qy_Ipc_Manage *G_pInstance=NULL;
typedef unsigned char      uint8_t;
typedef long long          int64_t;
//    687281b0bee195d415caaf20bd50a8de
//kkv:614ccd77440dd9c92603667e5cc14bf1|mp4.mp4
//���е�Ipc�����Ľ��
std::map<std::string,IPC_DATA_INFO> G_guidBufMap;

std::map<std::string,HANDLE> G_guidHMap;
std::map<std::string,std::map<std::string,HANDLE>*> G_URLRequestInfoMap;

Qy_IPC::CKKV_ReceiveData *G_pKKV_Rec=NULL;

Qy_IPC::Qy_IPc_InterCriSec G_KKMapLock;
//IPC ״̬����
int G_IPC_Read_Write=-1;
int OpenIPc();


std::basic_string<TCHAR> g_strModuleFileName;
const std::basic_string<TCHAR>& XGetModuleFilename()
{

	if (g_strModuleFileName.empty())
	{
		if(g_strModuleFileName.empty())
		{
			TCHAR filename[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, filename, _countof(filename));
			g_strModuleFileName = filename;
		}
	}
	return g_strModuleFileName;
}
std::basic_string<TCHAR> GetModulePath()
{
	std::basic_string<TCHAR> strModuleFileName = XGetModuleFilename();
	unsigned int index = strModuleFileName.find_last_of(L"\\");
	if (index != std::string::npos)
	{
		return strModuleFileName.substr(0, index);
	}
	return L"";
}

DWORD WINAPI UpdateWkeWebViewTh(LPVOID lpThreadParameter)
{
	while(true){
		Sleep(60000);
		if(G_IPC_Read_Write==0)
		{
              OpenIPc();
		}else if(G_IPC_Read_Write==2){
			 Sleep(10000);
			 G_pInstance->Stop();
             G_pInstance->Start();
		}
	}
	return 0;
}


void AddIPCGuid(std::string& strGuid,IPC_DATA_INFO &xxda)
{
	G_KKMapLock.Lock();
	G_guidBufMap.insert(std::pair<std::string,IPC_DATA_INFO>(strGuid,xxda));
	G_KKMapLock.Unlock();
}

void GetIPCOpRet(std::string& strGuid,bool& Ok,IPC_DATA_INFO &OutInfo)
{
	memset(&OutInfo,0,sizeof(OutInfo));
	G_KKMapLock.Lock();
	std::map<std::string,IPC_DATA_INFO>::iterator Itx=G_guidBufMap.find(strGuid);
	if(Itx!=G_guidBufMap.end())
	{
		Ok=true;
		OutInfo=Itx->second;
		G_guidBufMap.erase(Itx);
	}
	G_KKMapLock.Unlock();
}
int OpenIPc()
{
	if(G_pInstance==NULL)
		return 0;
	bool Ok=G_pInstance->OpenServerPipe("\\\\.\\Pipe\\KKPlayer_Res_70ic");
	if(!Ok){
		G_IPC_Read_Write=0;
		return 0;
	}else{
		G_IPC_Read_Write=1;
	}
	G_pInstance->Start();
	return 1;
}
//��ϢͨѶ��ʽ�涨��
//buflen+data;
int InitIPC(){
	G_pKKV_Rec= new Qy_IPC::CKKV_ReceiveData();
	G_pInstance = Qy_IPC::Qy_Ipc_Manage::GetInstance();
	G_pInstance->Init(G_pKKV_Rec,Qy_IPC::EQyIpcType::Client,0);
	OpenIPc();
	return 0;
}

void  CreatStrGuid(std::string &strGuid)
{
	GUID stGuid;
	::CoCreateGuid(&stGuid);
	char abcd[256]="";
	sprintf(abcd,"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", stGuid.Data1,stGuid.Data2,stGuid.Data3,
		stGuid.Data4[0],stGuid.Data4[1],stGuid.Data4[2],stGuid.Data4[3],
		stGuid.Data4[4],stGuid.Data4[5],stGuid.Data4[6],stGuid.Data4[7]);
	strGuid=abcd;
}


extern "C"{

char __declspec(dllexport)KKCloseAVFile(char *strUrl);
char __declspec(dllexport) GetPtlHeader(char *buf,char len)
{
	
	memset(buf,0,len);
	memcpy(buf,"kkv",4);
	return 0;
}

#define KKMKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define KKPERRTAG(a, b, c, d) (-(int)KKMKTAG(a, b, c, d))
#define AVERROR_EOF                KKPERRTAG( 'E','O','F',' ') ///< End of file
#define AVERROR(e) (-(e))

enum IPCMSG{
    IPCUnknown=0,
	IPCRead=1,
    IPCSeek=2,
	IPCDown=3,
	IPCClose=4,
	IPCSpeed=5,
};

void __declspec(dllexport) KKFree(void* p)
{
   if(p!=NULL)
	   free(p);
}

KK_DOWN_SPEED_INFO * ParseSpeedInfo(char *jsonstr)
{
	int lex=sizeof(KK_DOWN_SPEED_INFO);
	Json::Reader JsonReader;
	Json::Value jsonvalue;
	KK_DOWN_SPEED_INFO *PreInfo=NULL;
	if(jsonstr!=NULL&&JsonReader.parse(jsonstr,jsonvalue))
	{
		if(jsonvalue.size()>0){
			for(int i=0;i <jsonvalue.size();i++)
			{
				Json::Value Item=jsonvalue[i];
				KK_DOWN_SPEED_INFO *info=(KK_DOWN_SPEED_INFO*)::malloc(lex);
				info->Speed=Item["Speed"].asInt();
			    info->FileSize=Item["FileSize"].asInt();
				info->AcSize=Item["AcTotalSize"].asInt();
				info->Tick=Item["Tick"].asInt();
				info->Next=NULL;
				if(PreInfo==NULL)
				{
					PreInfo=info;
				}else{
					PreInfo->Next=info;
					PreInfo=info;
				}

			}
		}

	}
	return PreInfo;
}

//read 1; seek 2; down 3,close 4,SpeedInfo 5
//Send:buflen+data;
//data:guidlen+guid+msgId+h+FileInfoSize+FileInfo+bufsize;


//Reply:buflen+data;
//data:guidlen+guid+msgId+h+FileURL+FileURL+Buf_Size
int  __declspec(dllexport) Kkv_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
	if(G_pInstance==NULL)
	{
		InitIPC();
	}
LOOP1:
	KKPlugin* KKP=(KKPlugin*)opaque;
	char *IPCbuf=(char*)::malloc(1024);
	char *tempBuf=IPCbuf;
	tempBuf+=4;
	int len=4;

	std::string strGuid;
	CreatStrGuid(strGuid);


	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda={buf,0,hRead};
	

	AddIPCGuid(strGuid,xxda);

	int guilen=strGuid.length();
	memcpy(tempBuf,&guilen,4);
	tempBuf+=4;
	len+=4;                                                        //guilen 4
	
	memcpy(tempBuf,strGuid.c_str(),strGuid.length());
	tempBuf+=guilen;
	len+=guilen;                                                   //guid    32

	int msgId=1;//read
	memcpy(tempBuf,&msgId,4);          
	tempBuf+=4;
	len+=4;                                                        //msgId   4                             

	
	memcpy(tempBuf,&hRead,4);                                      //h       4
	tempBuf+=4;
	len+=4;

	//��״̬
	int FirstRead=KKP->FirstRead;
	KKP->FirstRead=0;
	memcpy(tempBuf,&FirstRead,4);
	tempBuf+=4;
	len+=4;

	int FileURlSize=strlen(KKP->URL);                              //FileURlSize 4
	memcpy(tempBuf,&FileURlSize,4);
	tempBuf+=4;
	len+=4;

	memcpy(tempBuf,KKP->URL,FileURlSize);                         //FileURl
	tempBuf+=FileURlSize;
	len+=FileURlSize;

	memcpy(tempBuf,&buf_size,4);                                  //buf_size    4
	tempBuf+=4;
	len+=4;
	//
	memcpy(IPCbuf,&len,4);
	while(G_IPC_Read_Write!=1)
	{
		if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1)
		{
			break;
		}
		Sleep(10);
	}
	G_pInstance->WritePipe(IPCbuf,len,0);
	while(1)
	{
		DWORD ret=::WaitForSingleObject( hRead,20);
        if(ret==WAIT_OBJECT_0)
		{
			break;
		}
		if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1)
		{
			break;
		}
	}
	
	
	::free(IPCbuf);
	::CloseHandle(hRead);
	
	if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1)
	{
		//KKCloseAVFile(KKP->URL);
		return AVERROR(errno);
	}
	
	IPC_DATA_INFO OutInfo;
	bool RetOk=false;
	GetIPCOpRet(strGuid,RetOk,OutInfo);
	if(RetOk)
	{
		int ret=OutInfo.DataSize;
		if(ret==-1000)
		{
			
			goto LOOP1;
		}
		return ret;
	}
	

	return AVERROR_EOF;
}

//buflen+data;
//data:guidlen +guid+ msgId+h+FileURLSize+FileURL+offset+whence
//Reply:buflen+data;
//data:guidlen+guid+msgId+h+FileInfoSize+FileInfo+Pos; 
int64_t  __declspec(dllexport) Kkv_seek(void *opaque, int64_t offset, int whence)//guidlen+guid+msgId+h+FileURL+FileURL+Buf_Size
{
LOOP1:
	KKPlugin* KKP=(KKPlugin*)opaque;

	char *IPCbuf=(char*)::malloc(1024);
	char *tempBuf=IPCbuf;
	tempBuf+=4;
	int len=0;
	len+=4;

	std::string strGuid;
	CreatStrGuid(strGuid);

	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda={0,0,hRead};
	
	AddIPCGuid(strGuid,xxda);

	int guilen=strGuid.length();
	memcpy(tempBuf,&guilen,4);
	tempBuf+=4;
	len+=4;

	memcpy(tempBuf,strGuid.c_str(),strGuid.length());
	tempBuf+=guilen;
	len+=guilen;

	int msgId=2;//seek
	memcpy(tempBuf,&msgId,4);
	tempBuf+=4;
	len+=4;

	memcpy(tempBuf,&hRead,4);
	tempBuf+=4;
	len+=4;

	int FileInfoSize=strlen(KKP->URL);
	memcpy(tempBuf,&FileInfoSize,4);
	tempBuf+=4;
	len+=4;

	memcpy(tempBuf,KKP->URL,FileInfoSize);
	tempBuf+=FileInfoSize;
	len+=FileInfoSize;

	memcpy(tempBuf,&offset,8);
	tempBuf+=8;
	len+=8;

	memcpy(tempBuf,&whence,4);
	tempBuf+=4;
	len+=4;

	//
	memcpy(IPCbuf,&len,4);

	while(G_IPC_Read_Write!=1)
	{
		if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1)
		{
			break;
		}
		Sleep(10);
	}
	G_pInstance->WritePipe(IPCbuf,len,0);

	while(1)
	{
		DWORD ret=::WaitForSingleObject( hRead,500);
		if(ret==WAIT_OBJECT_0)
		{
			break;
		}
		if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1)
		{
			break;
		}
	}


	//::WaitForSingleObject( hRead,INFINITE);
	::free(IPCbuf);
	::CloseHandle(hRead);
	
	if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1)
	{

		//KKCloseAVFile(KKP->URL);
		return AVERROR(errno);
	}

	IPC_DATA_INFO OutInfo;
	bool RetOk=false;
	GetIPCOpRet(strGuid,RetOk,OutInfo);
	if(RetOk)
	{
		int ret=OutInfo.DataSize;
		if(ret==-1000)
		{
			goto LOOP1;
		}
		return ret;
	}

	G_KKMapLock.Unlock();

	return AVERROR(errno);
}


//����һ�����ʵ��
KKPlugin __declspec(dllexport) *CreateKKPlugin()
{
	KKPlugin* p= (KKPlugin*)::malloc(sizeof(KKPlugin));
	p->kkread=Kkv_read_packet;
	p->kkseek=Kkv_seek;
	p->opaque=NULL;
	return p;
}
char __declspec(dllexport)KKStopDownAVFile(char *strUrl);
//ɾ��һ�����ʵ��
void __declspec(dllexport) DeleteKKPlugin(KKPlugin* p)
{
	KKStopDownAVFile(p->URL);
	::free(p);
}

//�����ļ���
char __declspec(dllexport)KKDownAVFile(char *strUrl)
{
	  if(G_pInstance==NULL||strUrl==NULL)
	  {
		  InitIPC();
	  }

	  char *IPCbuf=(char*)::malloc(1024);
	  char *tempBuf=IPCbuf;
	  tempBuf+=4;
	  int len=4;

	  std::string strGuid;
	  CreatStrGuid(strGuid);

	  HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	  IPC_DATA_INFO xxda={NULL,0,hRead};

	  int guilen=strGuid.length();
	  memcpy(tempBuf,&guilen,4);
	  tempBuf+=4;
	  len+=4;                                                        //guilen 4

	  memcpy(tempBuf,strGuid.c_str(),strGuid.length());
	  tempBuf+=guilen;
	  len+=guilen;                                                   //guid    32

	  int msgId=3;
	  memcpy(tempBuf,&msgId,4);          
	  tempBuf+=4;
	  len+=4;                                                        //msgId   4                             


	  memcpy(tempBuf,&hRead,4);                                      //h       4
	  tempBuf+=4;
	  len+=4;

	  int FileURlSize=strlen(strUrl);                              //FileURlSize 4
	  memcpy(tempBuf,&FileURlSize,4);
	  tempBuf+=4;
	  len+=4;

	  memcpy(tempBuf,strUrl,FileURlSize);                         //FileURl
	  tempBuf+=FileURlSize;
	  len+=FileURlSize;

	  
	  //
	  memcpy(IPCbuf,&len,4);
	  G_pInstance->WritePipe(IPCbuf,len,0);
	  
	  ::free(IPCbuf);
	  ::CloseHandle(hRead);
	  return 0;
  }
//ֹͣ�����ļ�
char __declspec(dllexport)KKStopDownAVFile(char *strUrl)
{
	if(G_pInstance==NULL||strUrl==NULL)
	{
		return 0;
	}

	char *IPCbuf=(char*)::malloc(1024);
	char *tempBuf=IPCbuf;
	tempBuf+=4;
	int len=4;

	std::string strGuid;
	CreatStrGuid(strGuid);

	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda={NULL,0,hRead};

	int guilen=strGuid.length();
	memcpy(tempBuf,&guilen,4);
	tempBuf+=4;
	len+=4;                                                        //guilen 4

	memcpy(tempBuf,strGuid.c_str(),strGuid.length());
	tempBuf+=guilen;
	len+=guilen;                                                   //guid    32

	int msgId=5;
	memcpy(tempBuf,&msgId,4);          
	tempBuf+=4;
	len+=4;                                                        //msgId   4                             


	memcpy(tempBuf,&hRead,4);                                      //h       4
	tempBuf+=4;
	len+=4;

	int FileURlSize=strlen(strUrl);                              //FileURlSize 4
	memcpy(tempBuf,&FileURlSize,4);
	tempBuf+=4;
	len+=4;

	memcpy(tempBuf,strUrl,FileURlSize);                         //FileURl
	tempBuf+=FileURlSize;
	len+=FileURlSize;


	//
	memcpy(IPCbuf,&len,4);
	G_pInstance->WritePipe(IPCbuf,len,0);

	::free(IPCbuf);
	::CloseHandle(hRead);
	return 0;
}

KK_DOWN_SPEED_INFO __declspec(dllexport) *KKDownAVFileSpeedInfo(char *strurl,int *TotalSpeed)
{
	if(G_pInstance==NULL||strurl==NULL)
	{
		return NULL;
	}

	KK_DOWN_SPEED_INFO * pSpeedInfo=NULL;
	char *IPCbuf=(char*)::malloc(1024);
	char *tempBuf=IPCbuf;
	tempBuf+=4;
	int len=4;

	std::string strGuid;
	CreatStrGuid(strGuid);

	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda={NULL,0,hRead};
	AddIPCGuid(strGuid,xxda);

	int guilen=strGuid.length();
	memcpy(tempBuf,&guilen,4);
	tempBuf+=4;
	len+=4;                                                        //guilen 4

	memcpy(tempBuf,strGuid.c_str(),strGuid.length());
	tempBuf+=guilen;
	len+=guilen;                                                   //guid    32

	int msgId=IPCMSG::IPCSpeed;
	memcpy(tempBuf,&msgId,4);          
	tempBuf+=4;
	len+=4;                                                        //msgId   4                             


	memcpy(tempBuf,&hRead,4);                                      //h       4
	tempBuf+=4;
	len+=4;

	int FileURlSize=strlen(strurl);                              //FileURlSize 4
	memcpy(tempBuf,&FileURlSize,4);
	tempBuf+=4;
	len+=4;

	memcpy(tempBuf,strurl,FileURlSize);                         //FileURl
	tempBuf+=FileURlSize;
	len+=FileURlSize;


	//
	memcpy(IPCbuf,&len,4);
	G_pInstance->WritePipe(IPCbuf,len,0);

	while(1&&G_IPC_Read_Write==1)
	{
		DWORD ret=::WaitForSingleObject( hRead,20);
		if(ret==WAIT_OBJECT_0){
			break;
		}
	}

	IPC_DATA_INFO OutInfo;
	bool RetOk=false;
	GetIPCOpRet(strGuid,RetOk,OutInfo);

	if(RetOk)
	{
		pSpeedInfo=ParseSpeedInfo((char*)OutInfo.pBuf);
	}

	::free(IPCbuf);
	::CloseHandle(hRead);
	return pSpeedInfo; 
}
};