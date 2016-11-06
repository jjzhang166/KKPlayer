#include "../stdafx.h"
#ifndef RENDER_H
#define RENDER_H
class CRender
{
public:
    enum RenderType
    {
        GDI_RENDER,
        D3D_RENDER,
    };
    virtual bool init(HWND hView) = 0;
    virtual void destroy() = 0;
    virtual void resize(unsigned int w, unsigned int h) = 0;
    virtual void WinSize(unsigned int w, unsigned int h) = 0;
	virtual void render(char* buf,int width,int height,int Imgwidth)=0;
	//≥ œ÷±≥æ∞Õº∆¨
	virtual void renderBk(unsigned char* buf,int len)=0;
	virtual void SetWaitPic(unsigned char* buf,int len)=0;

	virtual void LoadCenterLogo(unsigned char* buf,int len)=0;

	virtual void SetErrPic(unsigned char* buf,int len)=0;
	virtual void ShowErrPic(bool show)=0;
	//void SetLeftPicStr(wchar_t *str);
};

#endif