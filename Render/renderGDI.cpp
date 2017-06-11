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
,m_nTipTick(0)
{
	m_FpRenderImgCall=0;
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

	if(m_hView)
	{
		RECT rect;
		GetClientRect(hView, &rect);
		resize(rect.right, rect.bottom);
	}

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
void CRenderGDI::SetRenderImgCall(fpRenderImgCall fp,void* UserData)
{
m_FpRenderImgCall=fp;
m_UserData=UserData;
}
void CRenderGDI::render(kkAVPicInfo *Picinfo,bool wait)
{
	if (m_pixels == NULL||m_Picwidth!= Picinfo->width&&m_Picheight!=Picinfo->height)
		createBitmap(Picinfo->width,Picinfo->height);

	skiaSal(Picinfo);
 
	
	 if(m_FpRenderImgCall!=NULL){
		int len=m_width*m_height*4;
	     m_FpRenderImgCall(m_pixels,m_width,m_height,len,m_UserData);
	}else if(m_hView){
			HDC hDC = GetDC(m_hView);
			BitBlt(hDC, 0, 0, m_width, m_height, m_hDC, 0, 0, SRCCOPY);
			ReleaseDC(m_hView, hDC);
			::DeleteDC(hDC);
	}
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
void charTowchar(const char *chr, wchar_t *wchar, int size);
void  CRenderGDI::SetLeftPicStr(const char *str)
{
    wchar_t pwstr[1024]=L"";
    charTowchar(str,pwstr,1024);
    m_LeftStr=pwstr;
	m_nTipTick=::GetTickCount();
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


void  CRenderGDI::DrawSkVideo(SkCanvas& canvas,kkAVPicInfo *Picinfo)
{
	if(Picinfo!=NULL&&Picinfo->width>0&&Picinfo->height)
	{
		int offx=0;
		int offy=0;


        SkRect destRt;
		destRt.fLeft=0;
		destRt.fTop=0;
		destRt.fRight =m_width;
		destRt.fBottom=m_height;
        int  dh=m_height;
		int  dw=m_width;
		int  h=dh,w=dw;
		//if(dw>width)
		{
			dh=dw*Picinfo->height/Picinfo->width;
		}
		if(dh>h)
		{
			dh=h;
			dw=Picinfo->width*dh/Picinfo->height;
		}
        if(dw<w)
		{
			destRt.fLeft=(w-dw)/2;
			destRt.fRight=destRt.fLeft+dw;
		}
		if(dh<h)
		{
			destRt.fTop=(h-dh)/2;
			destRt.fBottom=destRt.fTop+dh;
		}

		SkBitmap AVSkbit;

		AVSkbit.setInfo(SkImageInfo::Make(Picinfo->width,Picinfo->height,SkColorType::kBGRA_8888_SkColorType,SkAlphaType::kPremul_SkAlphaType));
		AVSkbit.setPixels(Picinfo->data[0]);
		AVSkbit.notifyPixelsChanged();



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
void  CRenderGDI::SetErrPic(unsigned char* buf,int len)
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
void  CRenderGDI::ShowErrPic(bool show)
{

	
}

void  CRenderGDI::skiaSal(kkAVPicInfo *Picinfo)
{
	SkBitmap Skbit;
	Skbit.setInfo(SkImageInfo::Make(m_width,m_height,SkColorType::kBGRA_8888_SkColorType,SkAlphaType::kPremul_SkAlphaType));
	Skbit.setPixels(m_pixels);


	SkCanvas canvas(Skbit);


   
		SkRect destRt;
		destRt.fLeft=0;
		destRt.fTop=0;
		destRt.fRight=m_width;
		destRt.fBottom=m_height;

		m_Paint.setARGB(255, 0,0, 0);
		canvas.drawRect(destRt,m_Paint);


    if(m_bShowErrPic==false)
	    DrawSkVideo(canvas,Picinfo);

	if(Picinfo==NULL&&m_WaitBuffer!=NULL&&m_WaitLen>0&&m_bShowErrPic==false)
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

	if(m_LeftStr.length()>1)
	{
	    m_Paint.setTextEncoding(SkPaint::TextEncoding::kUTF16_TextEncoding);
		m_Paint.setTextAlign(SkPaint::Align::kLeft_Align);

		SkRect r; 
		m_Paint.setARGB(255, 255,255, 255); /**/
		r.set(25, 25, 200, 145); 
		// canvas.drawRect(r, paint); 


		SkRect dst2 = r;
		m_Paint.setTextSize(15);
		canvas.drawText(m_LeftStr.c_str(), m_LeftStr.length()*sizeof(WCHAR), dst2.fLeft, dst2.fTop , m_Paint);
	}
}

bool CRenderGDI::GetHardInfo(void** pd3d,void** pd3ddev,int *ver)
{
   return false;
}
void CRenderGDI::renderLock()
{

}
void CRenderGDI::renderUnLock()
{

}