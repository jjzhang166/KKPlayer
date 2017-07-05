#include "stdafx.h"
#include "renderRaw.h"

CRenderRaw::CRenderRaw(): m_Reanderfp(0),m_pRenderUserData(0)
{

}
CRenderRaw::~CRenderRaw()
{

}
bool CRenderRaw::init(HWND hView)
{
   return true;
}
void CRenderRaw::destroy()
{

}
void  CRenderRaw::resize(unsigned int w, unsigned int h)
{

}
void  CRenderRaw::LoadCenterLogo(unsigned char* buf,int len)
{

}
void  CRenderRaw::render(kkAVPicInfo *Picinfo,bool wait)
{
	
	if(Picinfo!=NULL)
	{m_lock.Lock();
		if(m_Reanderfp!=NULL)
        m_Reanderfp(Picinfo,m_pRenderUserData);
	m_lock.Unlock();
	}
}
void  CRenderRaw::renderBk(unsigned char* buf,int len)
{

}

void CRenderRaw::SetWaitPic(unsigned char* buf,int len)
{

}
void CRenderRaw::SetBkImagePic(unsigned char* buf,int len)
{

}
void CRenderRaw::WinSize(unsigned int w, unsigned int h)
{

}

void CRenderRaw::SetErrPic(unsigned char* buf,int len)
{

}
void CRenderRaw::ShowErrPic(bool show)
{

}
void CRenderRaw::FillRect(kkBitmap img,kkRect rt,unsigned int color)
{

}
void CRenderRaw::SetLeftPicStr(const char *str)
{

}
void CRenderRaw::SetRenderImgCall(fpRenderImgCall fp,void* UserData)
{
	m_lock.Lock();
   
    m_Reanderfp=fp;
	m_pRenderUserData=UserData;
	m_lock.Unlock();
}
bool CRenderRaw::GetHardInfo(void** pd3d,void** pd3ddev,int *ver)
{
   return false;
}
void CRenderRaw::SetResetHardInfoCall(fpResetDevCall call,void* UserData)
{

}
void CRenderRaw::renderLock()
{

}
void CRenderRaw::renderUnLock()
{

}