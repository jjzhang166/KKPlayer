#ifndef RENDER_RAW_H
#define RENDER_RAW_H

#include "../KKPlayerCore/render/render.h"
#include "RendLock.h"
///原生呈现，不做任何处理
class CRenderRaw : public IkkRender
{
public:
    CRenderRaw();
    ~CRenderRaw();
    bool init(HWND hView);
    void destroy();
    void         resize(unsigned int w, unsigned int h);
	void LoadCenterLogo(unsigned char* buf,int len);
	void         render(kkAVPicInfo *Picinfo,bool wait);
	void          renderBk(unsigned char* buf,int len);

	void SetWaitPic(unsigned char* buf,int len);
	void SetBkImagePic(unsigned char* buf,int len);
    void WinSize(unsigned int w, unsigned int h);

	virtual void SetErrPic(unsigned char* buf,int len);
	virtual void ShowErrPic(bool show);
	virtual void FillRect(kkBitmap img,kkRect rt,unsigned int color);
	virtual void SetLeftPicStr(const char *str);
	void SetRenderImgCall(fpRenderImgCall fp,void* UserData);
	bool GetHardInfo(void** pd3d,void** pd3ddev,int *ver);
	void SetResetHardInfoCall(fpResetDevCall call,void* UserData);
	void renderLock();
	void renderUnLock();
private:
	CRendLock m_lock;
	fpRenderImgCall m_Reanderfp;
	void*           m_pRenderUserData;
};

#endif