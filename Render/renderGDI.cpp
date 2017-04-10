#include "stdafx.h"
#include "renderGDI.h"
CRenderGDI::CRenderGDI():m_hView(NULL)
,m_hDC(NULL)
,m_hBitmap(NULL)
,m_pixels(NULL)
,m_width(0)
,m_height(0)
,m_BkBuffer(NULL)
,m_BkLen(NULL)
,m_WaitBuffer(NULL)
,m_WaitLen(NULL)
,m_CenterLogoBuf(NULL)
,m_CenterLogoBufLen(NULL)
,m_bShowErrPic(false)
,m_pErrbitmap(NULL)

{
}
CRenderGDI::~CRenderGDI()
{
	if (m_hDC)
		DeleteDC(m_hDC);

	if (m_hBitmap)
		DeleteObject(m_hBitmap);
}

bool CRenderGDI::init(HWND hView)
{
	m_hView = hView;
	m_hDC = CreateCompatibleDC(0);

	RECT rect;
	GetClientRect(hView, &rect);
	resize(rect.right, rect.bottom);
	return true;
}

void  CRenderGDI::destroy()
{
	delete this;
}
void CRenderGDI::resize(unsigned int w, unsigned int h)
{
	if (m_width == w && m_height == h)
		return;

	m_width = w;
	m_height = h;
	m_pixels = NULL;
}

void CRenderGDI::LoadCenterLogo(unsigned char* buf,int len)
{
	m_CenterLogoBuf=buf;
	m_CenterLogoBufLen=len;
}
void CRenderGDI::render(char* buf,int width,int height,int Imgwidth)
{
	if (m_pixels == NULL||m_Picwidth!= width&&m_Picheight!=height)
		createBitmap(width,height);

	skiaSal(buf,width,height);

	HDC hDC = GetDC(m_hView);
	BitBlt(hDC, 0, 0, m_width, m_height, m_hDC, 0, 0, SRCCOPY);
	ReleaseDC(m_hView, hDC);
	::DeleteDC(hDC);
}
void CRenderGDI::renderBk(unsigned char* buf,int len)
{

}
void CRenderGDI::SetWaitPic(unsigned char* buf,int len)
{
	m_WaitBuffer=(char*)buf;
	m_WaitLen=len;
}
void CRenderGDI::SetBkImagePic(unsigned char* buf,int len)
{
	m_BkBuffer=(char*)buf;
	m_BkLen=len;
}
void CRenderGDI::WinSize(unsigned int w, unsigned int h)
{

}
void CRenderGDI::createBitmap(unsigned int w, unsigned int h)
{
	if (m_hBitmap)
		DeleteObject(m_hBitmap);

	//图像区域宽度
	m_Picwidth=w;
	//图像区域高度
	m_Picheight=h;
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


	m_hBitmap = hbmp;

}


void  CRenderGDI::DrawSkVideo(SkCanvas& canvas,char *buf,int w,int h)
{
	if(buf!=NULL&&w>0&&h>0)
	{
		int offx=0;
		int offy=0;
		if(m_width>m_Picwidth)
		{
			offx=(m_width-m_Picwidth)/2;
		}
		if(m_height>m_Picheight)
		{
			offy=(m_height-m_Picheight)/2;
		}
		int totalLen=w*h*4;
		SkBitmap AVSkbit;

		AVSkbit.setInfo(SkImageInfo::Make(w,h,SkColorType::kBGRA_8888_SkColorType,SkAlphaType::kPremul_SkAlphaType));
		AVSkbit.setPixels(buf);
		AVSkbit.notifyPixelsChanged();


		SkRect destRt;
		destRt.fLeft=offx;
		destRt.fTop=offy;
		destRt.fRight=m_width;
		destRt.fBottom=m_height;

		if(m_width>m_Picwidth)
		{
			destRt.fRight=offx+w;

		}
		if(m_height>m_Picheight)
		{
			destRt.fBottom=offy+h;
		}

	
		canvas.drawBitmapRect(AVSkbit,destRt,&m_Paint);
	}
}
void  CRenderGDI::FillRect(kkBitmap img,kkRect rt,unsigned int colour)
{
	SkBitmap Skbit;
	Skbit.setInfo(SkImageInfo::Make( img.width,img.height,SkColorType::kBGRA_8888_SkColorType,SkAlphaType::kPremul_SkAlphaType));
	Skbit.setPixels(img.pixels);

	SkCanvas canvas(Skbit);

	m_Paint.setStyle(SkPaint::kFill_Style);
	SkRect srt={rt.left,rt.top,rt.right,rt.bottom};
	m_Paint.setARGB(255, GetRValue(colour), GetGValue(colour), GetBValue(colour));
	canvas.drawRect(srt,m_Paint);
}
void CRenderGDI::SetErrPic(unsigned char* buf,int len)
{

	if(buf!=NULL)
	{
		delete m_pErrbitmap;
	    m_pErrbitmap = new SkBitmap();
		SkMemoryStream stream(m_BkLen);
		stream.read(buf,len);
		//SkFILEStream ff("F:\\Pro\\ProcLectureRoom\\KKPlayer\\KKdebug\\Skin\\wait3.png");
		SkImageDecoder::DecodeStream(&stream,m_pErrbitmap);
	}

}
void CRenderGDI::ShowErrPic(bool show)
{

	
}

void  CRenderGDI::skiaSal(char *buf,int w,int h)
{
	SkBitmap Skbit;
	Skbit.setInfo(SkImageInfo::Make(m_width,m_height,SkColorType::kBGRA_8888_SkColorType,SkAlphaType::kPremul_SkAlphaType));
	Skbit.setPixels(m_pixels);

	SkCanvas canvas(Skbit);



	if(m_BkBuffer!=NULL)
	{
		SkBitmap Bkbitmap;
		SkMemoryStream stream(m_BkLen);
		stream.read(m_BkBuffer,m_BkLen);
		//SkFILEStream ff("F:\\Pro\\ProcLectureRoom\\KKPlayer\\KKdebug\\Skin\\wait3.png");
		SkImageDecoder::DecodeStream(&stream, &Bkbitmap);

		SkRect destRt;
		destRt.fLeft=0;
		destRt.fTop=0;
		destRt.fRight=m_width;
		destRt.fBottom=m_height;
		canvas.drawBitmapRect(Bkbitmap,destRt,&m_Paint);
	}


    if(m_bShowErrPic==false)
	    DrawSkVideo(canvas,buf,w,h);

	if(buf==NULL&&m_WaitBuffer!=NULL&&m_WaitLen>0&&m_bShowErrPic==false)
	{
		SkBitmap Bkbitmap;
		SkMemoryStream stream(m_WaitBuffer,m_WaitLen,true);


		//ff.read(m_BkBuffer,m_BkLen);
		SkImageDecoder::DecodeStream(&stream, &Bkbitmap);


		int poxx=m_width/2-22;
		int poxy=m_height/2-22;
		SkRect destRt;
		destRt.fLeft=poxx;
		destRt.fTop=poxy;
		destRt.fRight=poxx+44;
		destRt.fBottom=poxy+45;

		canvas.drawBitmapRect(Bkbitmap,destRt,&m_Paint);
	}else if(m_bShowErrPic&&m_pErrbitmap!=NULL)
	{
		

		int w=m_pErrbitmap->width();
		int h=m_pErrbitmap->height();
		int yx=m_width/2-w/2;
		int yy=m_height/2-h/2;

		SkRect destRt;
		destRt.fLeft=yx;
		destRt.fTop=yy;
		destRt.fRight=destRt.fLeft+w;
		destRt.fBottom=destRt.fTop+h;

		canvas.drawBitmapRect(*m_pErrbitmap,destRt,&m_Paint);
	}
}

