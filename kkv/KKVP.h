#ifndef KKVP_H_
#define KKVP_H_
enum IPCMSG{
	IPCUnknown=0,
	IPCRead=1,
	IPCSeek=2,
	IPCDown=3,
	IPCClose=4,
	IPCSpeed=5,
	IPCCacheTime=6,
	IPCURLParser=9,
	IPCDownPause=10,
	IPCDownResume=11,
	IPCALLSpeed=15,
};
#endif