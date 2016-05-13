#ifndef RENDER_D3D_H
#define RENDER_D3D_H

#include "../../KKPlayerCore/render/render.h"
#include <d3d9.h>
#include <d3dx9.h>
class CRenderD3D : public CRender
{
public:
    CRenderD3D();
    ~CRenderD3D();

    virtual bool init(HWND hView);
    virtual void destroy();
    virtual void resize(unsigned int w, unsigned int h);
    void  WinSize(unsigned int w, unsigned int h);
    bool LostDeviceRestore();
    bool UpdateTexture(char *pBuf,int w,int h);
    void render(char *pBuf,int width,int height);
	void renderBk(unsigned char* buf,int len);
	void LoadCenterLogo(unsigned char* buf,int len);
	void SetWaitPic(unsigned char* buf,int len);
	void SetBkImagePic(unsigned char* buf,int len);
	void DrawFontInfo();
private:

	bool UpdateLeftPicTexture();
	//void  source_hdc = GetDC(hwnd);
	void CreateFonet();
    HWND m_hView;
	//Ã· æ◊÷ÃÂ

    IDirect3D9* m_pD3D;
    IDirect3DDevice9* m_pDevice;
    IDirect3DTexture9* m_pDxTexture;
	IDirect3DTexture9* Fontexture;
	IDirect3DTexture9* m_PBkTexture;
	IDirect3DTexture9* m_CenterLogoTexture;
	IDirect3DTexture9* m_pWaitPicTexture;
	IDirect3DTexture9* m_pLeftPicTexture;
	//YUV
	IDirect3DSurface9 *m_pDirect3DSurfaceRender;

//	m_CenterLogoTexture
    unsigned int m_w;
	unsigned int m_h;
    struct Vertex
    {
        float x, y, z, w;
        float u, v;

        enum 
        {
            FVF = D3DFVF_XYZRHW|D3DFVF_TEX1,
        };
    };

    Vertex m_Vertex[4];
	Vertex m_FontVertex[4];
	Vertex m_WaitVertex[4];
	Vertex m_CenterLogVertex[4];

	Vertex m_LeftPicVertex[4];

};

#endif