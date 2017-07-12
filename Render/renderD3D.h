#ifndef RENDER_D3D_H
#define RENDER_D3D_H

#include "../KKPlayerCore/render/render.h"
#include "RendLock.h"
#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include <core\SkCanvas.h>
#include <core\SkBitmap.h>
#include <core\SkTypeface.h>
#include <core\SkImageDecoder.h>
#include <core\SkStream.h>
class CRenderD3D : public IkkRender
{
public:
    CRenderD3D(int         cpu_flags);
    ~CRenderD3D();

    virtual bool init(HWND hView);
    virtual void destroy();
    virtual void resize(unsigned int w, unsigned int h);
    void  WinSize(unsigned int w, unsigned int h);
    bool LostDeviceRestore();
    void SurCopy(IDirect3DSurface9  *sur);
    void render(kkAVPicInfo *Picinfo,bool wait);
	void renderBk(unsigned char* buf,int len);
	void LoadCenterLogo(unsigned char* buf,int len);
	

	void SetWaitPic(unsigned char* buf,int len);
	
	void SetErrPic(unsigned char* buf,int len);
	void ShowErrPic(bool show);
	
	void DrawFontInfo();

	
	void SetLeftPicStr(const char *str);
	void FillRect(kkBitmap img,kkRect rt,unsigned int color);
	
	void SetRenderImgCall(fpRenderImgCall fp,void* UserData);
	bool GetHardInfo(void** pd3d,void** pd3ddev,int *ver);
	void SetResetHardInfoCall(fpResetDevCall call,void* UserData);
	void renderLock();
	void renderUnLock();
private:
	int          m_ncpu_flags;
	bool         m_bNeedReset;
	SkPaint      m_skPaint;
    std::wstring m_LeftStr;
	std::wstring m_LstLeftStr;
	CRendLock m_lock;
	bool UpdateTexture(kkAVPicInfo *Picinfo);
	void ResetTexture();
	bool UpdateLeftPicTexture();
	void AdJustErrPos(int picw,int pich);
	bool m_bShowErrPic;
    HWND m_hView;
	//提示字体

    IDirect3D9* m_pD3D;
    IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* Fontexture;

	unsigned char* m_ErrBufImg;
	int m_ErrBufImgLen;
	IDirect3DTexture9* m_ErrTexture;
	IDirect3DTexture9* m_CenterLogoTexture;
	IDirect3DTexture9* m_pWaitPicTexture;
	IDirect3DTexture9* m_pLeftPicTexture;
	//YUV
	IDirect3DSurface9  *m_pYUVAVTexture;
	IDirect3DSurface9  *m_pYUVAVTextureSysMem;
	IDirect3DSurface9  *m_pBackBuffer;
    int                m_lastpicw;
	int                m_lastpich;

    RECT m_rtViewport; 
	//渲染区域高度
    unsigned int m_w;
	unsigned int m_h;
	unsigned int m_lastw;
	unsigned int m_lasth;
    struct Vertex
    {
        float x, y, z, w;
        float u, v;

        enum 
        {
            FVF = D3DFVF_XYZRHW|D3DFVF_TEX1,
        };
    };

    Vertex m_VideoVertex[4];
	Vertex m_FontVertex[4];
	Vertex m_WaitVertex[4];
	Vertex m_CenterLogVertex[4];
	Vertex m_LeftPicVertex[4];
	Vertex m_ErrPicVertex[4];
    int    m_nPicformat;
	fpResetDevCall m_ResetCall;
	void*          m_ResetUserData;
};

#endif