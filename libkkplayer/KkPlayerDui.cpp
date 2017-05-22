#include "KkPlayerDui.h"
unsigned char* CKKPlayerDui::GetWaitImage(int &length,int curtime)
{
  return 0;
}
unsigned char* CKKPlayerDui::GetErrImage(int &length,int ErrType)
{
  return 0;
}
unsigned char* CKKPlayerDui::GetBkImage(int &length)
{
  return 0;
}
unsigned char* CKKPlayerDui::GetCenterLogoImage(int &length)
{
  return 0;
}
//打开失败
void CKKPlayerDui::OpenMediaStateNotify(char* strURL,EKKPlayerErr err)
{

}
 ///读取线程打开文件前的回调函数,用于分段，返回1，代表有分段，此时InOutUrl已经被修改。
 int CKKPlayerDui::PreOpenUrlCallForSeg(char *InOutUrl,int *AvIsSeg,int *Interrupt)
 {
    return 1;
 }
/*******视频流结束调用.用于加载下一个分片*******/
void CKKPlayerDui::GetNextAVSeg(void *playerIns,int Err,int quesize,KKPlayerNextAVInfo &NextInfo)
{
    return;
}

/***视频读取线程结束调用，只能使用其地址值****/
void  CKKPlayerDui::AVReadOverThNotify(void *playerIns)
{
    return;
}
/***视频读取线程结束调用***/
void  CKKPlayerDui::AVRender()
{
    return;
}