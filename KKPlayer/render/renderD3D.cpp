#include "stdafx.h"
#include "renderD3D.h"
#include <d3dx9tex.h>
#include <wchar.h>
#include <stdio.h>
#include <assert.h>
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif
#include <string>
std::basic_string<TCHAR> GetModulePath();
//#define VFYUY420P
typedef IDirect3D9* (WINAPI* LPDIRECT3DCREATE9)( UINT );
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);  

LPFN_ISWOW64PROCESS fnIsWow64Process;  
BOOL IsWow64()  
{  
	BOOL bIsWow64 = FALSE;  

	//IsWow64Process is not available on all supported versions of Windows.  
	//Use GetModuleHandle to get a handle to the DLL that contains the function  
	//and GetProcAddress to get a pointer to the function if available.  

	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(  
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");  

	if(NULL != fnIsWow64Process)  
	{  
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))  
		{  
			//handle error  
		}  
	}  
	return bIsWow64;  
}  
CRenderD3D::CRenderD3D()
    :m_hView(NULL)
    ,m_pD3D(NULL)
    ,m_pDevice(NULL)
	,m_pDxTexture(NULL)
	,m_pDirect3DSurfaceRender(NULL)
	,Fontexture(NULL)
	,m_PBkTexture(NULL)
	,m_pWaitPicTexture(NULL)
	,m_CenterLogoTexture(NULL)
{
}

CRenderD3D::~CRenderD3D()
{
	
	SAFE_RELEASE(m_pWaitPicTexture);
	SAFE_RELEASE(m_pDirect3DSurfaceRender); 
    SAFE_RELEASE(m_CenterLogoTexture);
	SAFE_RELEASE(m_pDxTexture);
	SAFE_RELEASE(m_PBkTexture);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pD3D);
}

D3DPRESENT_PARAMETERS GetPresentParams(HWND hView)
{
    D3DPRESENT_PARAMETERS PresentParams;
    ZeroMemory(&PresentParams, sizeof(PresentParams));

    PresentParams.BackBufferWidth = 0;
    PresentParams.BackBufferHeight = 0;
    PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
    PresentParams.BackBufferCount = 1;
    PresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    PresentParams.MultiSampleQuality = 0;
    PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    PresentParams.hDeviceWindow = hView;
    PresentParams.Windowed = TRUE;
    PresentParams.EnableAutoDepthStencil = TRUE;
    PresentParams.AutoDepthStencilFormat = D3DFMT_D24X8;
    PresentParams.Flags = D3DPRESENTFLAG_VIDEO;
    PresentParams.FullScreen_RefreshRateInHz = 0;
    PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    return PresentParams;
}


bool CRenderD3D::init(HWND hView)
{
	m_hView = hView;

	HMODULE hModD3D9 = LoadLibraryA("d3d9.dll");
	if(!hModD3D9)
	{
		std::wstring path=GetModulePath();
		if(IsWow64())
			path+=L"\\win7\\d3d9.dll";
		else
		{
			path+=L"\\xp\\d3d9.dll";
		}
		hModD3D9 = LoadLibrary(path.c_str());
	}
	if (hModD3D9)
	{

		LPDIRECT3DCREATE9 pfnDirect3DCreate9 = (LPDIRECT3DCREATE9)GetProcAddress(hModD3D9, "Direct3DCreate9");
		if(pfnDirect3DCreate9==NULL)
		{
			::MessageBox(hView,L"未能加载Direct3DCreate9函数",L"提示",MB_ICONHAND);
		}
		m_pD3D=pfnDirect3DCreate9(D3D_SDK_VERSION);//Direct3DCreate9(D3D_SDK_VERSION);//

		DWORD BehaviorFlags =D3DCREATE_FPU_PRESERVE | D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_NOWINDOWCHANGES;
		D3DPRESENT_PARAMETERS PresentParams = GetPresentParams(hView);

		int ii=m_pD3D->GetAdapterCount();
		//GetParent()
		HRESULT hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetParent(hView), BehaviorFlags, &PresentParams, &m_pDevice);
		if (FAILED(hr) && hr != D3DERR_DEVICELOST)
		{
			BehaviorFlags = D3DCREATE_FPU_PRESERVE | D3DCREATE_PUREDEVICE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hView, BehaviorFlags, &PresentParams, &m_pDevice);
			if (FAILED(hr) && hr != D3DERR_DEVICELOST)
			{
				assert(0);
			}
			::MessageBox(hView,L"d3d创建错误",L"提示",MB_ICONHAND);
			return false;
		}

		resize(PresentParams.BackBufferWidth, PresentParams.BackBufferHeight);
		return true;
	}

	::MessageBox(hView,L"未能加载d3d9.dll，请安装d3d9",L"提示",MB_ICONHAND);
	return false;
}

void CRenderD3D::destroy()
{
    delete this;
}

void CRenderD3D::resize(unsigned int w, unsigned int h)
{
	m_w=w;
	m_h=h;
   {
		m_Vertex[0].x = -0.5f;
		m_Vertex[0].y = -0.5f;
		m_Vertex[0].z = 0.f;
		m_Vertex[0].w = 1.f;
		m_Vertex[0].u = 0.f;
		m_Vertex[0].v = 0.f;

		m_Vertex[1].x = w - 0.5f;
		m_Vertex[1].y = -0.5f;
		m_Vertex[1].z = 0.f;
		m_Vertex[1].w = 1.f;
		m_Vertex[1].u = 1.f;
		m_Vertex[1].v = 0.f;

		m_Vertex[2].x = -0.5f;
		m_Vertex[2].y = h - 0.5f;
		m_Vertex[2].z = 0.f;
		m_Vertex[2].w = 1.f;
		m_Vertex[2].u = 0.f;
		m_Vertex[2].v = 1.f;

		m_Vertex[3].x = w - 0.5f;
		m_Vertex[3].y = h - 0.5f;
		m_Vertex[3].z = 0.f;
		m_Vertex[3].w = 1.f;
		m_Vertex[3].u = 1.f;
		m_Vertex[3].v = 1.f;
    }
    {
		w=200;
		h=100;
		int yx=m_w/2-w/2;
		int yy=m_h/2-h/2;
		m_FontVertex[0].x =yx -0.5f;  //A     A(leftTop)->B(rightTop)->C(leftBu)->D->(rightBu)
		m_FontVertex[0].y =yy -0.5f;
		m_FontVertex[0].z = 0.0f;
		m_FontVertex[0].w = 1.f;
		m_FontVertex[0].u = 0.f;
		m_FontVertex[0].v = 0.f;

		m_FontVertex[1].x = yx+w - 0.5f;  //B
		m_FontVertex[1].y = yy-0.5f;
		m_FontVertex[1].z = 0.0f;
		m_FontVertex[1].w = 1.f;
		m_FontVertex[1].u = 1.f;
		m_FontVertex[1].v = 0.f;

		m_FontVertex[2].x =yx -0.5f;   //C
		m_FontVertex[2].y =yy+h - 0.5f;
		m_FontVertex[2].z = 0.0f;
		m_FontVertex[2].w = 1.f;
		m_FontVertex[2].u = 0.f;
		m_FontVertex[2].v = 1.f;

		m_FontVertex[3].x =yx+ w - 0.5f; //D
		m_FontVertex[3].y =yy+ h - 0.5f;
		m_FontVertex[3].z = 0.f;
		m_FontVertex[3].w = 1.f;
		m_FontVertex[3].u = 1.f;
		m_FontVertex[3].v = 1.f;
	}
	{
		w=44;
		h=46;
		int yx=m_w/2-w/2;
		int yy=m_h/2-h/2;
		 m_WaitVertex[0].x =yx -0.5f;  //A     A(leftTop)->B(rightTop)->C(leftBu)->D->(rightBu)
		 m_WaitVertex[0].y =yy -0.5f;
		 m_WaitVertex[0].z = 0.0f;
		 m_WaitVertex[0].w = 1.f;
		 m_WaitVertex[0].u = 0.f;
		 m_WaitVertex[0].v = 0.f;

		 m_WaitVertex[1].x = yx+w - 0.5f;  //B
		 m_WaitVertex[1].y = yy-0.5f;
		 m_WaitVertex[1].z = 0.0f;
		 m_WaitVertex[1].w = 1.f;
		 m_WaitVertex[1].u = 1.f;
		 m_WaitVertex[1].v = 0.f;

		 m_WaitVertex[2].x =yx -0.5f;   //C
		 m_WaitVertex[2].y =yy+h - 0.5f;
		 m_WaitVertex[2].z = 0.0f;
		 m_WaitVertex[2].w = 1.f;
		 m_WaitVertex[2].u = 0.f;
		 m_WaitVertex[2].v = 1.f;

		 m_WaitVertex[3].x =yx+ w - 0.5f; //D
		 m_WaitVertex[3].y =yy+ h - 0.5f;
		 m_WaitVertex[3].z = 0.f;
		 m_WaitVertex[3].w = 1.f;
		 m_WaitVertex[3].u = 1.f;
		 m_WaitVertex[3].v = 1.f;
	}
	{
		
		w=97;
		h=72;
		int yx=m_w/2-w/2;
		int yy=m_h/2-h/2;
		m_CenterLogVertex[0].x =yx -0.5f;  //A     A(leftTop)->B(rightTop)->C(leftBu)->D->(rightBu)
		m_CenterLogVertex[0].y =yy -0.5f;
		m_CenterLogVertex[0].z = 0.0f;
		m_CenterLogVertex[0].w = 1.f;
		m_CenterLogVertex[0].u = 0.f;
		m_CenterLogVertex[0].v = 0.f;

		m_CenterLogVertex[1].x = yx+w - 0.5f;  //B
		m_CenterLogVertex[1].y = yy-0.5f;
		m_CenterLogVertex[1].z = 0.0f;
		m_CenterLogVertex[1].w = 1.f;
		m_CenterLogVertex[1].u = 1.f;
		m_CenterLogVertex[1].v = 0.f;

		m_CenterLogVertex[2].x =yx -0.5f;   //C
		m_CenterLogVertex[2].y =yy+h - 0.5f;
		m_CenterLogVertex[2].z = 0.0f;
		m_CenterLogVertex[2].w = 1.f;
		m_CenterLogVertex[2].u = 0.f;
		m_CenterLogVertex[2].v = 1.f;

		m_CenterLogVertex[3].x =yx+ w - 0.5f; //D
		m_CenterLogVertex[3].y =yy+ h - 0.5f;
		m_CenterLogVertex[3].z = 0.f;
		m_CenterLogVertex[3].w = 1.f;
		m_CenterLogVertex[3].u = 1.f;
		m_CenterLogVertex[3].v = 1.f;
	}
	WinSize(w,h);
}

void  CRenderD3D::WinSize(unsigned int w, unsigned int h)
{
	D3DPRESENT_PARAMETERS PresentParams = GetPresentParams(m_hView);
	m_pDevice->Reset(&PresentParams);

	SAFE_RELEASE(m_pDxTexture);
  //  SAFE_RELEASE(m_pDirect3DSurfaceRender);
}
void CRenderD3D::SetWaitPic(unsigned char* buf,int len)
{
	// 将刚才构建好的bmp数据，转成IDirect3DTexture9*  的纹理  
	SAFE_RELEASE(m_pWaitPicTexture);
	if ( FAILED( D3DXCreateTextureFromFileInMemory( this->m_pDevice,buf, len, &m_pWaitPicTexture)))
	{
		//assert(0);
		return;// S_FALSE;
	}
}
void CRenderD3D::render(char *pBuf,int width,int height)
{
	#ifndef VFYUY420P
    if (!LostDeviceRestore())
        return;
    #endif

	#ifdef VFYUY420P
	    m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 ); 

		if(SUCCEEDED(m_pDevice->BeginScene()))
		{
			IDirect3DSurface9 * pBackBuffer = NULL;  
			RECT m_rtViewport; 
            GetClientRect(m_hView,&m_rtViewport);  
			m_pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);  
			m_pDevice->StretchRect(m_pDirect3DSurfaceRender,NULL,pBackBuffer,&m_rtViewport,D3DTEXF_LINEAR);  
			m_pDevice->EndScene();  
			m_pDevice->Present( NULL, NULL, NULL, NULL );  
		}
		
    #else
	    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 64), 1.0f, 0);
		if( SUCCEEDED(m_pDevice->BeginScene()) )
		{
			if(pBuf!=NULL)
			{
                UpdateTexture(pBuf,width,height);	
				m_pDevice->SetTexture(0, m_pDxTexture);
				m_pDevice->SetFVF(Vertex::FVF);
				m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_Vertex, sizeof(Vertex));
			}
			if(pBuf==NULL)
			{
				m_pDevice->SetTexture(0, m_PBkTexture);
				m_pDevice->SetFVF(Vertex::FVF);
				m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_Vertex, sizeof(Vertex));

				m_pDevice->SetTexture(0,  m_pWaitPicTexture);
				m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,  m_WaitVertex, sizeof(Vertex));/**/
			}
			
			
			m_pDevice->EndScene();
			m_pDevice->Present(NULL, NULL, NULL, NULL);
		}
    #endif
   
	
}

bool CRenderD3D::LostDeviceRestore()
{
    HRESULT hr = m_pDevice->TestCooperativeLevel();
    if (hr == D3DERR_DEVICELOST)
    {
        SAFE_RELEASE(m_pDxTexture);
        return false;
    }

    if (hr == D3DERR_DEVICENOTRESET)
    {
        SAFE_RELEASE(m_pDxTexture);
        D3DPRESENT_PARAMETERS PresentParams = GetPresentParams(m_hView);
        hr = m_pDevice->Reset(&PresentParams);
        if (FAILED(hr))
            return false;
    }

    return true;
}
void CRenderD3D::CreateFonet()
{
	HDC hDc = ::CreateCompatibleDC( NULL);                    // 通过当前桌面创建设备内容HDC
	SetTextColor( hDc, RGB( 255,255,255));                          // 设置背景颜色和文字的颜色
//	SetBkColor( hDc, 0xff000000);
	
	SetBkMode( hDc,TRANSPARENT);
	SetMapMode( hDc, MM_TEXT);

	DWORD* pBitmapBits;                                                        //  创建一个位图
	BITMAPINFO bitmapInfo;
	ZeroMemory( &bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	bitmapInfo.bmiHeader.biSize  = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = 200;
	bitmapInfo.bmiHeader.biHeight = 100;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biBitCount = 32;

	HBITMAP hBitmap = CreateDIBSection( hDc, &bitmapInfo,  DIB_RGB_COLORS, (VOID**)&pBitmapBits, NULL, 0);

	{
		int dwSize=200*100*4;
		char *lpBuffer =(char *) pBitmapBits;
		memset(lpBuffer,0,dwSize);/**/
	}
	
	SelectObject( hDc, hBitmap);                            // 将位图与HDC关联，这样文字时间上就保存在hBitmap里面了



	LOGFONT lf;
	ZeroMemory( &lf, sizeof(LOGFONT));
	lf.lfHeight = 72;
	lf.lfWidth = 0;

	HFONT   hFont   =   CreateFontIndirect(&lf);              // 创建大小为72的字体

	SelectObject(hDc, hFont);
	TextOutA( hDc,0,0,"在这里写字", strlen("在这里写字"));

	BITMAP bmp;
	GetObject( hBitmap, sizeof(BITMAP), &bmp);
	BYTE* buffer;   
	int  blength;                                  //　整个hBitMap文件字节长度
	blength = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp.bmWidthBytes * bmp.bmHeight;
	buffer = new BYTE[blength];

	BITMAPINFOHEADER bi;
	bi.biSize                    =  sizeof(BITMAPINFOHEADER);  
	bi.biWidth                   =  bmp.bmWidth;  
	bi.biHeight                  =  bmp.bmHeight;  
	bi.biPlanes                  =  1;  
	bi.biBitCount                =  bmp.bmBitsPixel;  
	bi.biCompression             =  BI_RGB;  
	bi.biSizeImage               =  0;  
	bi.biXPelsPerMeter           =  0;  
	bi.biYPelsPerMeter           =  0;  
	bi.biClrImportant            =  0;  
	bi.biClrUsed                 =  0;

	BITMAPFILEHEADER bmfHdr;
	bmfHdr.bfType  =  0x4D42;  //  "BM"    
	bmfHdr.bfSize  =  blength;    
	bmfHdr.bfReserved1  =  0;    
	bmfHdr.bfReserved2  =  0;    
	bmfHdr.bfOffBits  =  (DWORD)sizeof(BITMAPFILEHEADER)  +  (DWORD)sizeof(BITMAPINFOHEADER);

	int dwSize= blength - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
    unsigned char *lpBuffer =(unsigned char *) bmp.bmBits;
	for( int i = 0 ; i < dwSize ; i+=4 )
	{
		if(lpBuffer[i]==255)
		{		
			lpBuffer[i+3]=255;
		}else
		{
			lpBuffer[i+3]=0;
		}
	}/**/

	memcpy( buffer, &bmfHdr, sizeof(BITMAPFILEHEADER));
	memcpy( &buffer[sizeof(BITMAPFILEHEADER)], &bi, sizeof(BITMAPINFOHEADER));
	memcpy( &buffer[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)],
		bmp.bmBits, blength - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER));

	// 将刚才构建好的bmp数据，转成IDirect3DTexture9*  的纹理  
	if ( FAILED( D3DXCreateTextureFromFileInMemory( this->m_pDevice, buffer, blength, &Fontexture)))
	{
		return;// S_FALSE;
	}



	delete[] buffer;
	DeleteObject(hFont);
	DeleteObject( hBitmap);
	DeleteDC( hDc);
}
int GetBmpSize(int w,int h);
bool CRenderD3D::UpdateTexture(char *pBuf,int w,int h)
{
#ifdef VFYUY420P	
   if (m_pDirect3DSurfaceRender == NULL)
    {
        RECT rect2;
        GetClientRect(m_hView, &rect2);
		UINT hei=rect2.bottom - rect2.top;
		UINT Wei=rect2.right - rect2.left;
		if(hei>0&&Wei>0)
		{
			resize( Wei,hei);

			HRESULT  hr = m_pDevice->CreateOffscreenPlainSurface(Wei,
				hei,
				(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),  
				D3DPOOL_DEFAULT,  
				&m_pDirect3DSurfaceRender,  
				NULL);  

			if (FAILED(hr))
				return false;
		}else
			return false;
		
    }
#else
   {
	   if (m_pDxTexture == NULL)
	   {
		   RECT rect2;
		   GetClientRect(m_hView, &rect2);
		   UINT hei=rect2.bottom - rect2.top;
		   UINT Wei=rect2.right - rect2.left;
		   resize( Wei,hei);

		   HRESULT  hr = m_pDevice->CreateTexture(
			   w,
			   h,
			   1,
			   D3DUSAGE_DYNAMIC,
			   D3DFMT_X8R8G8B8,
			   D3DPOOL_DEFAULT,
			   &m_pDxTexture,
			   NULL);/**/
		   if (FAILED(hr))
			   return false;
		   CreateFonet();
		  
	   }
	}
#endif
   
      D3DLOCKED_RECT rect;//1009*488
      
	  {
		 // w=852 h=480
		#ifdef VFYUY420P
		  {
			  if(m_w>w&&m_h>h)
			  {
				  m_pDirect3DSurfaceRender->LockRect(&rect,NULL,D3DLOCK_DONOTWAIT);  
				  byte *pSrc = (byte *)pBuf;  
				  byte * pDest = (BYTE *)rect.pBits;  
				  int stride = rect.Pitch;  
				  unsigned long i = 0;  
				  int pixel_h=h;
				  int pixel_w=w;
				  //Copy Data (YUV420P)  
				  for(i = 0;i < pixel_h;i ++)
				  {  
					  memcpy(pDest + i * stride,pSrc + i * pixel_w, pixel_w);  
				  }  
				  for(i = 0;i < pixel_h/2;i ++)
				  {  
					  memcpy(pDest + stride * pixel_h + i * stride / 2,pSrc + pixel_w * pixel_h + pixel_w * pixel_h / 4 + i * pixel_w / 2, pixel_w / 2);  
				  }  
				  for(i = 0;i < pixel_h/2;i ++)
				  {  
					  memcpy(pDest + stride * pixel_h + stride * pixel_h / 4 + i * stride / 2,pSrc + pixel_w * pixel_h + i * pixel_w / 2, pixel_w / 2);  
				  } 
				  m_pDirect3DSurfaceRender->UnlockRect();
			  }
			
		  } 
#else
			  m_pDxTexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
			 
			  unsigned char* dst = (unsigned char*)rect.pBits; 
			  unsigned char* src = (unsigned char*)pBuf; 
			  memset(dst,255,rect.Pitch*h);
			  if(pBuf!=NULL)
			  {
				  int row=w*4;
			      for(int i = 0; i < h; ++i) 
				  {
				    memcpy(dst, src,row);
				    src += row;
				    dst += rect.Pitch;
				   }
			  }
			m_pDxTexture->UnlockRect(0);		  
#endif
	  }
      
//D3DXCreateTextureFromFile
	
    return true;
}




void CRenderD3D::renderBk(unsigned char* buf,int len)
{
	if(m_PBkTexture==NULL)
	{
		// 将刚才构建好的bmp数据，转成IDirect3DTexture9*  的纹理  
		if ( FAILED( D3DXCreateTextureFromFileInMemory( this->m_pDevice,buf, len, &m_PBkTexture)))
		{
			//assert(0);
			return;// S_FALSE;
		}
	}

	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 64), 1.0f, 0);
	if( SUCCEEDED(m_pDevice->BeginScene()) )
	{
		m_pDevice->SetTexture(0, m_PBkTexture);
		m_pDevice->SetFVF(Vertex::FVF);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_Vertex, sizeof(Vertex));


		 
		if(m_CenterLogoTexture!=NULL)
		{
				 m_pDevice->SetTexture(0,  m_CenterLogoTexture);
				 m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				 m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				 m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				 m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				 m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,  m_CenterLogVertex, sizeof(Vertex));/**/
		}

		m_pDevice->EndScene();
		m_pDevice->Present(NULL, NULL, NULL, NULL);
	}
}
void  CRenderD3D::LoadCenterLogo(unsigned char* buf,int len)
{
	if(m_CenterLogoTexture==NULL)
	{
		// 将刚才构建好的bmp数据，转成IDirect3DTexture9*  的纹理  
		if ( FAILED( D3DXCreateTextureFromFileInMemory( this->m_pDevice,buf, len, & m_CenterLogoTexture)))
		{
			//assert(0);
			return;// S_FALSE;
		}
	}
}
void CRenderD3D::SetBkImagePic(unsigned char* buf,int len)
{
	if(m_PBkTexture==NULL)
	{
		// 将刚才构建好的bmp数据，转成IDirect3DTexture9*  的纹理  
		if ( FAILED( D3DXCreateTextureFromFileInMemory( this->m_pDevice,buf, len, &m_PBkTexture)))
		{
			int i=0;
			i++;
			i=0;
			//assert(0);
			return;// S_FALSE;
		}
	}
}