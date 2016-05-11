#ifndef RENDER_GDI_H
#define RENDER_GDI_H

#include "../../KKPlayerCore/render/render.h"

class CRenderGDI : public CRender
{
public:
    CRenderGDI()
        :m_hView(NULL)
        ,m_hDC(NULL)
        ,m_hBitmap(NULL)
        ,m_pixels(NULL)
        ,m_width(0)
        ,m_height(0)
    {}

    ~CRenderGDI()
    {
        if (m_hDC)
            DeleteDC(m_hDC);

        if (m_hBitmap)
            DeleteObject(m_hBitmap);
    }

    virtual bool init(HWND hView)
    {
        m_hView = hView;
        m_hDC = CreateCompatibleDC(0);

        RECT rect;
        GetClientRect(hView, &rect);
        resize(rect.right, rect.bottom);
        return true;
    }

    virtual void destroy()
    {
        delete this;
    }

    virtual void resize(unsigned int w, unsigned int h)
    {
        if (m_width == w && m_height == h)
            return;

        m_width = w;
        m_height = h;
        m_pixels = NULL;
    }

    virtual void render(char* buf,int width,int height)
    {
		if (m_pixels == NULL)
			createBitmap();

		if(width>200&&height>100)
		{
			char *bufx=(char*)m_pixels;
			int row=width*4;
			int row2=m_width*4;
			for(int i=0;i<height;i++)
			{
                memcpy(bufx,buf,row);
				buf+=row;
				bufx+=row2;
			}
			
			HDC hDC = GetDC(m_hView);
			BitBlt(hDC, 0, 0, m_width, m_height, m_hDC, 0, 0, SRCCOPY);
			ReleaseDC(m_hView, hDC);
			::DeleteDC(hDC);
		}
		
		//free(m_pixels);
    }

	virtual void LoadCenterLogo(unsigned char* buf,int len)
	{

	}
	virtual void renderBk(unsigned char* buf,int len)
	{

	}
	void SetWaitPic(unsigned char* buf,int len)
	{

	}
	void SetBkImagePic(unsigned char* buf,int len)
	{

	}
    void WinSize(unsigned int w, unsigned int h)
    {
		
	}
    void createBitmap()
    {
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth         = int(m_width);
        bi.bmiHeader.biHeight        = -int(m_height);
        bi.bmiHeader.biPlanes        = 1;
        bi.bmiHeader.biBitCount      = 32;
        bi.bmiHeader.biCompression   = BI_RGB;

        HBITMAP hbmp = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &m_pixels, NULL, 0);

        SelectObject(m_hDC, hbmp);

        if (m_hBitmap)
            DeleteObject(m_hBitmap);

        m_hBitmap = hbmp;
    }

private:
    HWND m_hView;
    HBITMAP m_hBitmap;
    HDC m_hDC;
    unsigned int m_width;
    unsigned int m_height;
    void* m_pixels;
};

#endif