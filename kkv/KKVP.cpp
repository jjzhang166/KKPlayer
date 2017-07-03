#include "KKVP.h"
//errno.h
#include "../../QyIPC/Qy_Ipc_Manage.h"
#include "KKV_ReceiveData.h"
#include <map>
#include <string> 
#include <algorithm>
#include "./../KKPlayer/KKPlayerCore/KKError.h"
#include "../../KKPlayer/KKPlayerCore/KKPlugin.h"
#include "json/json.h"

#include <process.h>
#include <tlhelp32.h>






Qy_IPC::Qy_Ipc_Manage *G_pInstance=NULL;
typedef unsigned char      uint8_t;
typedef long long          int64_t;

//所有的Ipc操作的结果.
std::map<std::string,IPC_DATA_INFO>                 G_guidBufMap;

std::map<std::string,unsigned int>                  G_CacheTimeMap;
std::map<std::string,HANDLE>                        G_guidHMap;

std::map<std::string,std::string>                    G_URLInfoMap;
std::map<std::string,std::string>                    G_SpeedInfoMap;
Qy_IPC::CKKV_ReceiveData                            *G_pKKV_Rec=NULL;
Qy_IPC::CKKV_DisConnect                             *G_pKKV_Dis=NULL;
Qy_IPC::Qy_IPc_InterCriSec                           G_KKMapLock;
HANDLE                                               G_IPC_PreHandel=0;
HANDLE                                               G_KKResHandle  = 0;//::OpenEvent(EVENT_ALL_ACCESS,  TRUE,  L"Global\\xxxxx_name");
//IPC 状态函数
int G_IPC_Read_Write=-1;
int OpenIPc();

bool GetSpeedInfo(const char *strurl,char *jsonBuf,int len)
{
	bool aaa=false;
	G_KKMapLock.Lock();
	std::map<std::string,std::string>::iterator It=G_SpeedInfoMap.find(strurl);
	if(It!=G_SpeedInfoMap.end()){
		if(len>It->second.length())
		{
		    strcpy(jsonBuf,It->second.c_str());
			aaa=true;
		}
	}
	G_KKMapLock.Unlock();
    return aaa;
}
static std::basic_string<TCHAR> g_strModuleFileName;
const  std::basic_string<TCHAR>& XGetModuleFilename()
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


void AddIPCGuid(std::string& strGuid,IPC_DATA_INFO &xxda)
{
	G_KKMapLock.Lock();
	G_guidBufMap.insert(std::pair<std::string,IPC_DATA_INFO>(strGuid,xxda));
	G_KKMapLock.Unlock();
}


void IPCFreeData()
{
	
	G_KKMapLock.Lock();
	std::map<std::string,IPC_DATA_INFO>::iterator Itx=G_guidBufMap.begin();

	for(;Itx!=G_guidBufMap.end();++Itx)
	{
		SetEvent(Itx->second.hWait);
	}
	G_guidBufMap.clear();
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

bool KillProcessFromName(std::wstring strProcessName)  
{  
	transform(strProcessName.begin(), strProcessName.end(), strProcessName.begin(),  toupper);   
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)  
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  

	//PROCESSENTRY32进程快照的结构体  
	PROCESSENTRY32 pe;  

	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作  
	pe.dwSize = sizeof(PROCESSENTRY32);  


	//下面的IF效果同:  
	//if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄  
	if(!Process32First(hSnapShot,&pe))  
	{  
		return false;  
	}  

	bool ok=false;
	//如果句柄有效  则一直获取下一个句柄循环下去  
	while (Process32Next(hSnapShot,&pe))  
	{  

		//pe.szExeFile获取当前进程的可执行文件名称  
		std::wstring scTmp = pe.szExeFile;


		//将可执行文件名称所有英文字母修改为小写  
		transform(scTmp.begin(), scTmp.end(), scTmp.begin(),  toupper);   

		//比较当前进程的可执行文件名称和传递进来的文件名称是否相同  
		//相同的话Compare返回0  
		if(scTmp==strProcessName)  
		{  

			//从快照进程中获取该进程的PID(即任务管理器中的PID)  
			DWORD dwProcessID = pe.th32ProcessID;  
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);  
			::TerminateProcess(hProcess,0);  
			CloseHandle(hProcess);  
			ok= true;  
		}  

	}  
	return ok;  
}  

//FILE *TestFb=NULL;
int OpenIPc()
{
	int recount=0;
ReOpen:
	if(G_pInstance==NULL)
		return 0;

	//KillProcessFromName(L"kkres.exe");
	if(G_IPC_PreHandel==0)
		G_IPC_PreHandel=::CreateEvent(NULL,TRUE,FALSE,NULL);
	bool Ok=G_pInstance->OpenServerPipe("\\\\.\\Pipe\\KKPlayer_Res_70ic");
	if(!Ok){
		if(recount<20){
			recount++;
#ifndef _DEBUG
				{
					std::wstring kkres=GetModulePath();
					kkres+=L"//kkres//kkRes.exe";

					PROCESS_INFORMATION pi;
					STARTUPINFO si;

					memset(&pi,0,sizeof(pi));
					memset(&si,0,sizeof(si));
					KillProcessFromName(L"kkres.exe");
					BOOL ret = CreateProcess(kkres.c_str(),NULL, NULL, NULL, FALSE, 0, NULL, NULL,&si, &pi);
				    Sleep(5000);
				}
#endif
			goto ReOpen;
		}
		G_IPC_Read_Write=0;
		return 0;
	}else{
		G_IPC_Read_Write=1;
	}
	G_pInstance->Start();

	
	//::MessageBox(0,L"xxx2",L"",0);
	while(1)
	{
	    DWORD Ret=	::WaitForSingleObject(G_IPC_PreHandel,20);
		if(Ret==WAIT_TIMEOUT)
		{
		
		}else if(Ret==0)
		{
			//::MessageBox(0,L"xxx",L"",0);
			::ResetEvent(G_IPC_PreHandel);
		    break;
		}
	}
	//::MessageBox(0,L"xxx2",L"",0);
	return 1;
}

//消息通讯格式规定。
//buflen+data;
int   InitIPC(){
	

	if(G_pKKV_Rec==NULL)
	{
         G_pKKV_Rec= new Qy_IPC::CKKV_ReceiveData();
	}
	if(G_pKKV_Dis==NULL)
	{
		G_pKKV_Dis= new Qy_IPC::CKKV_DisConnect();
	}
	if(G_pInstance!=NULL)
	{
		delete G_pInstance;
		G_pInstance=NULL;
	}
	
	G_pInstance =new Qy_IPC::Qy_Ipc_Manage();
	G_pInstance->Init(G_pKKV_Rec,Qy_IPC::QyIpcClient,G_pKKV_Dis);
	int lx=OpenIPc();
	
	if(lx==0){
		//::MessageBox(0,L"",L"",0);
		delete G_pInstance;
		G_pInstance=NULL;
	}
	
	return lx;
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

int   KKVWritePipe(unsigned char *pBuf,int Len,HANDLE hPipeInst)
{
	int lx=0;
	G_KKMapLock.Lock();
	if(G_pInstance==NULL||G_IPC_Read_Write==2)
	{
		lx=InitIPC();
		if(lx==1)
		{
			G_pInstance->WritePipe(pBuf,Len,hPipeInst);
		}
	}else{
		if(G_IPC_Read_Write==1)
		{
            G_pInstance->WritePipe(pBuf,Len,hPipeInst);
			lx=1;
		
		}
	}
	G_KKMapLock.Unlock();
	return lx;
}

//typedef bool (*fKKDownAVFileSpeedInfo)(const char *strurl,char *jsonBuf,int len);
static std::string GetUrlRemoveKkv(std::string Url)
{
	int index=Url.find("kkv:");
	if(index>-1)
	{
		index+=4;
		Url=Url.substr(index,Url.length()-index);
	}
	return Url;
}
extern "C"{

char __declspec(dllexport)KKCloseAVFile(char *strUrl);
char __declspec(dllexport) GetPtlHeader(char *buf,char len)
{
	
	memset(buf,0,len);
	memcpy(buf,"kkv",4);
	return 0;
}

void __declspec(dllexport) KKFree(void* p)
{
   if(p!=NULL)
	   free(p);
}




int  __declspec(dllexport) Kkv_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
	
LOOP1:
	KKPlugin* KKP=(KKPlugin*)opaque;
	KKP->SetNeedWait(KKP->PlayerOpaque,true);

	std::string strGuid;
	CreatStrGuid(strGuid);

    memset(buf,0,buf_size);
	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda;
	xxda.pBuf=buf;
	xxda.BufLen=buf_size;
	xxda.DataSize=-1000;
	xxda.hWait=hRead;
	xxda.CacheTime=0;
	
	AddIPCGuid(strGuid,xxda);

	Json::Value jsonValue;
	jsonValue["IPCMSG"]=IPCRead;
	jsonValue["Guid"]=strGuid;
	jsonValue["Url"]=KKP->URL;
    jsonValue["HRW"]=(int)hRead;
	jsonValue["FirstRead"]=KKP->FirstRead;
	KKP->FirstRead=0;
	if(buf_size>32768)
		 jsonValue["BufLen"]=32768;
	else
    jsonValue["BufLen"]=buf_size;


	strGuid=jsonValue.toStyledString();
	int buflen=strGuid.length()+1024;
	unsigned char *IPCbuf=(unsigned char*)::malloc(buflen);
	memset(IPCbuf,0,buflen);
	memcpy(IPCbuf,strGuid.c_str(),strGuid.length());

	int rext=KKVWritePipe(IPCbuf,buflen,0);
	while(1&&G_IPC_Read_Write==1&&rext==1)
	{
		DWORD ret=::WaitForSingleObject( hRead,50);
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
	
	if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1||rext==0)
	{

		if(rext==0)
			return  KK_AVERROR(ERROR_BROKEN_PIPE);

		return KK_AVERROR(errno);
	}
	
	IPC_DATA_INFO OutInfo;
	bool RetOk=false;
	strGuid=jsonValue["Guid"].asString();
	GetIPCOpRet(strGuid,RetOk,OutInfo);
	if(RetOk)
	{
		int ret=OutInfo.DataSize;
		if(ret<0){
		    return KK_AVERROR_EOF;
		}
			
		if(KKP->CalPlayerDelay!=NULL)
		{
			KKP->CalPlayerDelay(KKP->PlayerOpaque,OutInfo.CacheTime,2);
		}
		return ret;
	}
	
	return KK_AVERROR_EOF;
}

//buflen+data;
//data:guidlen +guid+ msgId+h+FileURLSize+FileURL+offset+whence
//Reply:buflen+data;
//data:guidlen+guid+msgId+h+FileInfoSize+FileInfo+Pos; 
int64_t  __declspec(dllexport) Kkv_seek(void *opaque, int64_t offset, int whence)//guidlen+guid+msgId+h+FileURL+FileURL+Buf_Size
{
LOOP1:
	KKPlugin* KKP=(KKPlugin*)opaque;
	

	std::string strGuid;
	CreatStrGuid(strGuid);


	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda;
	xxda.pBuf=0;
	xxda.BufLen=0;
	xxda.DataSize=-1000;
	xxda.hWait=hRead;
	xxda.CacheTime=0;
	AddIPCGuid(strGuid,xxda);

	Json::Value jsonValue;
	jsonValue["IPCMSG"]=IPCSeek;
	jsonValue["Guid"]=strGuid;
	jsonValue["Url"]=KKP->URL;
    jsonValue["HRW"]=(int)hRead;
    jsonValue["offset"]=offset;
    jsonValue["whence"]=whence;

	strGuid=jsonValue.toStyledString();
	int buflen=strGuid.length()+1024;
	unsigned char *IPCbuf=(unsigned char*)::malloc(buflen);
	memset(IPCbuf,0,buflen);
	memcpy(IPCbuf,strGuid.c_str(),strGuid.length());

    int rext=KKVWritePipe(IPCbuf,buflen,0);
	while(1&&G_IPC_Read_Write==1&&rext==1)
	{
		DWORD ret=::WaitForSingleObject( hRead,50);
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
	
	if(KKP->opaque==NULL&&KKP->kkirq!=NULL&&KKP->kkirq(KKP->PlayerOpaque)==1||rext==0)
	{

		if(rext==0)
			return  KK_AVERROR(109);
		//KKCloseAVFile(KKP->URL);
		return KK_AVERROR(errno);
	}

	IPC_DATA_INFO OutInfo;
	bool RetOk=false;
	strGuid=jsonValue["Guid"].asString();
	GetIPCOpRet(strGuid,RetOk,OutInfo);
	if(RetOk)
	{
		long long ret=OutInfo.DataSize;
		return ret;
	}
	return KK_AVERROR(errno);
}



unsigned int  Kkv_GetCacheTime(void *opaque)//guidlen+guid+msgId+h+FileURL+FileURL+Buf_Size
{

	return 0;
}
//创建一个插件实例
KKPlugin __declspec(dllexport) *CreateKKPlugin()
{
	KKPlugin* p= (KKPlugin*)::malloc(sizeof(KKPlugin));
	p->GetCacheTime=Kkv_GetCacheTime;
	p->kkread=Kkv_read_packet;
	p->kkseek=Kkv_seek;
	p->opaque=NULL;
	
	return p;
}

//删除一个插件实例
void __declspec(dllexport) DeleteKKPlugin(KKPlugin* p)
{
    IPCFreeData();
	::free(p);
}

//下载文件件
char __declspec(dllexport)KKDownAVFile(char *strUrl)
{
	  
	  return 0;
}

//暂停下载文件
void __declspec(dllexport)KKPauseDownAVFile(char *strUrl,bool Pause)
{
	if(G_IPC_Read_Write!=1)
		return ;
	Json::Value jsonValue;
	if(Pause)
	    jsonValue["IPCMSG"]=IPCDownPause;
	else
		jsonValue["IPCMSG"]=IPCDownResume;
	jsonValue["Guid"]="";
	jsonValue["Url"]=strUrl;
    jsonValue["HRW"]=0;
	jsonValue["FirstRead"]=0;
	std::string strGuid=jsonValue.toStyledString();
    int buflen=strGuid.length()+1024;
	
	unsigned char *IPCbuf=(unsigned char*)::malloc(buflen);
	memset(IPCbuf,0,buflen);
	
	
	memcpy(IPCbuf,strGuid.c_str(),strGuid.length());
	KKVWritePipe(IPCbuf,buflen,0);
	::free(IPCbuf);

	return;
}


///获取下载速度信息
bool __declspec(dllexport) KKDownAVFileSpeedInfo(const char *strurl,char *jsonBuf,int len)
{
	if(G_IPC_Read_Write!=1)
		return false;
	std::string url=GetUrlRemoveKkv(strurl);
	Json::Value jsonValue;
	jsonValue["IPCMSG"]=IPCSpeed;
	jsonValue["Guid"]="";
	jsonValue["Url"]=url;
    jsonValue["HRW"]=0;
	jsonValue["FirstRead"]=0;

	std::string strGuid=jsonValue.toStyledString();
    int buflen=strGuid.length()+1024;
	
	unsigned char *IPCbuf=(unsigned char*)::malloc(buflen);
	memset(IPCbuf,0,buflen);
	
	
	memcpy(IPCbuf,strGuid.c_str(),strGuid.length());
	KKVWritePipe(IPCbuf,buflen,0);
	::free(IPCbuf);
	
	return GetSpeedInfo(url.c_str(),jsonBuf,len);
}



char * c_left(char *dst,char *src, int n)
{
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if(n>len) n = len;
	/*p += (len-n);*/   /*从右边第n个字符开始*/
	while(n--) *(q++) = *(p++);
	*(q++)='\0'; /*有必要吗？很有必要*/
	return dst;
}
char* GetIPCUrlParserRet(const char*Url,int *abort_request);
char __declspec(dllexport) *KKUrlParser(const char *strurl,int *abort_request)
{
	char *ret=NULL;
	char* pos=(char*)strstr(strurl,":") ;
	if(pos!=NULL)
	{
		///int buflen=strGuid.length()+1024;
		int Len=strlen(strurl)+1024;
		char *ProName=(char*)::malloc(Len);
        memset(ProName,0,Len);
		int lll=pos-strurl;
		c_left(ProName,(char *)strurl,lll);
		if(strcmp(ProName,"kkv")==0){
		      strcpy(ProName,pos+1);
              ret=GetIPCUrlParserRet(ProName,abort_request);
		}
		::free(ProName);
	}
	return ret; 
}
};
///得到URL分析的结果
char* GetIPCUrlParserRet(const char*Url,int *abort_request)
{

    std::string strGuid;
	CreatStrGuid(strGuid);

 
	HANDLE hRead=CreateEvent(NULL, TRUE, FALSE, NULL);
	IPC_DATA_INFO xxda;
	xxda.pBuf=0;
	xxda.BufLen=0;
	xxda.DataSize=0;
	xxda.hWait=hRead;
	xxda.CacheTime=0;
	
	AddIPCGuid(strGuid,xxda);

	Json::Value jsonValue;
	jsonValue["IPCMSG"]=IPCURLParser;
	jsonValue["Guid"]=strGuid;
	jsonValue["Url"]=Url;
    jsonValue["HRW"]=(int)hRead;
	jsonValue["FirstRead"]=0;
	

	strGuid=jsonValue.toStyledString();
	int buflen=strGuid.length()+1024;
	unsigned char *IPCbuf=(unsigned char*)::malloc(buflen);
	memset(IPCbuf,0,buflen);
	memcpy(IPCbuf,strGuid.c_str(),strGuid.length());

	int rext=KKVWritePipe(IPCbuf,buflen,0);
	while(rext==1&&*abort_request==0)
	{
		DWORD ret=::WaitForSingleObject( hRead,50);
        if(ret==WAIT_OBJECT_0)
		{
			break;
		}
	}
	
	
	::free(IPCbuf);
	::CloseHandle(hRead);
	
	
	IPC_DATA_INFO OutInfo;
	bool RetOk=false;
	strGuid=jsonValue["Guid"].asString();
	GetIPCOpRet(strGuid,RetOk,OutInfo);

	if(RetOk){
		return (char*)OutInfo.pBuf;
	}
	return NULL;
}