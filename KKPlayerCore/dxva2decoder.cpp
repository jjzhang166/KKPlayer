// Dxva2Decoder.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Dxva2Def.h"
//#include "dxva2decoder.h"
#include "libavutil/buffer.h"
#include <strmif.h>
/* DLL */
HINSTANCE             G_hd3d9_dll=NULL;
HINSTANCE             G_hdxva2_dll=NULL;
IDirect3D9 *          G_pD3D9=NULL;
LPDIRECT3DDEVICE9     G_pD3Ddev=NULL;

static int D3dCreateDevice(kk_va_dxva2_t *);
static int D3dCreateDeviceManager(kk_va_dxva2_t *);
static int DxCreateVideoService(kk_va_dxva2_t *);
static int DxFindVideoServiceConversion(kk_va_dxva2_t *, GUID *input, D3DFORMAT *output);
static int DxCreateVideoDecoder(kk_va_dxva2_t *, int codec_id, const video_format_t *fmt);
static int DxResetVideoDecoder(kk_va_dxva2_t *);
static void DxCreateVideoConversion(kk_va_dxva2_t *);

void* gpu_memcpy(void* d, const void* s, size_t size);
static void (*CopyFrameNV12)(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch) = 0;


void* gpu_memcpy(void* d, const void* s, size_t size);
static void CopyFrameNV12_fallback(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	const size_t size = imageHeight * pitch;
	memcpy(pY, pSourceData, size);
	memcpy(pUV, pSourceData + (surfaceHeight * pitch), size >> 1);
}

static void CopyFrameNV12_fallback_MT(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	int i;
	const size_t halfSize = (imageHeight * pitch) >> 1;
	for(i=0;i<3;i++) {
		if (i < 2)
			memcpy(pY + (halfSize * i), pSourceData + (halfSize * i), halfSize);
		else
			memcpy(pUV, pSourceData + (surfaceHeight * pitch), halfSize);
	}
}

static void CopyFrameNV12_SSE4(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	const size_t size = imageHeight * pitch;
	gpu_memcpy(pY, pSourceData, size);
	gpu_memcpy(pUV, pSourceData + (surfaceHeight * pitch), size >> 1);
}

static void CopyFrameNV12_SSE4_MT(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	int i;
	const size_t halfSize = (imageHeight * pitch) >> 1;
	for(i=0;i<3;i++)  {
		if (i < 2){
			gpu_memcpy(pY + (halfSize * i), pSourceData + (halfSize * i), halfSize);
		}
		else{
			gpu_memcpy(pUV, pSourceData + (surfaceHeight * pitch), halfSize);
		}
	}
}

#define VEND_ID_ATI     0x1002
#define VEND_ID_NVIDIA  0x10DE
#define VEND_ID_INTEL   0x8086
void IniCopyFrameNV12(int m_dwVendorId)
{
	if (CopyFrameNV12 == 0) {
		int cpu_flags = av_get_cpu_flags();
		if (cpu_flags & AV_CPU_FLAG_SSE4) {
			if (m_dwVendorId == VEND_ID_INTEL)
				CopyFrameNV12 = CopyFrameNV12_SSE4_MT;
			else
				CopyFrameNV12 = CopyFrameNV12_SSE4;
		} else {
			if (m_dwVendorId == VEND_ID_INTEL)
				CopyFrameNV12 = CopyFrameNV12_fallback_MT;
			else
				CopyFrameNV12 = CopyFrameNV12_fallback;
		}
	}/**/
}
static const d3d_format_t *D3dFindFormat(D3DFORMAT format)
{
	for (unsigned i = 0; d3d_formats[i].name; i++) {
		if (d3d_formats[i].format == format)
			return &d3d_formats[i];
	}
	return NULL;
}
static const dxva2_mode_t *Dxva2FindMode(const GUID& guid)
{
	for (unsigned i = 0; dxva2_modes[i].name; i++) {
		if (IsEqualGUID(dxva2_modes[i].guid, guid))
			return &dxva2_modes[i];
	}
	return NULL;
}


/**
* It creates a Direct3D device usable for DXVA 2
*/
static int D3dCreateDevice(kk_va_dxva2_t *va)
{
	/* */
	typedef LPDIRECT3D9 (WINAPI *Create9func)(UINT SDKVersion);
	Create9func Create9 = (Create9func )GetProcAddress(G_hd3d9_dll, LPCSTR("Direct3DCreate9"));

	if (!Create9) {
		av_log(NULL, AV_LOG_ERROR, "Cannot locate reference to Direct3DCreate9 ABI in DLL");
		return -1;
	}

	/* */
	if(G_pD3D9==NULL){
		G_pD3D9= Create9(D3D_SDK_VERSION);
		if (!G_pD3D9) {
			av_log(NULL, AV_LOG_ERROR, "Direct3DCreate9 failed");
			return -1;
		}
	}
	
	

	/* */
	D3DADAPTER_IDENTIFIER9 d3dai ={};
	if (FAILED(IDirect3D9_GetAdapterIdentifier(G_pD3D9,
		D3DADAPTER_DEFAULT, 0, &d3dai))) {
			av_log(NULL, AV_LOG_WARNING, "IDirect3D9_GetAdapterIdentifier failed");
		
	}

	va->vendorId=d3dai.VendorId;

   
	IniCopyFrameNV12(va->vendorId);
	/* */
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Flags                  = D3DPRESENTFLAG_VIDEO;
	d3dpp.Windowed               = TRUE;
	d3dpp.hDeviceWindow          = NULL;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.BackBufferCount        = 0;                  /* FIXME what to put here */
	d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;    /* FIXME what to put here */
	d3dpp.BackBufferWidth        = 0;
	d3dpp.BackBufferHeight       = 0;
	d3dpp.EnableAutoDepthStencil = FALSE;

	/* Direct3D needs a HWND to create a device, even without using ::Present
	this HWND is used to alert Direct3D when there's a change of focus window.
	For now, use GetShellWindow, as it looks harmless */
	

	if(G_pD3Ddev==NULL)
	{
         G_pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetShellWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,&d3dpp, &G_pD3Ddev);
	}
	
	/*if (FAILED(IDirect3D9_CreateDevice(G_pD3D9, D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, GetShellWindow(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING |
		D3DCREATE_MULTITHREADED,
		&d3dpp, &d3ddev))) {
			av_log(NULL, AV_LOG_ERROR, "IDirect3D9_CreateDevice failed\n");
			return -1;
	}*/

	return 0;
}

static void DxDestroyVideoDecoder(kk_va_dxva2_t *va)
{
	if (va->decoder)
	{
		HRESULT  hr=	va->decoder->Release();
		va->decoder = NULL;
	}
	for (unsigned i = 0; i < va->surface_count; i++)
	{
            HRESULT  hr= va->surface[i].d3d->Release();
			if (FAILED(hr))
			{
				printf("Error!\n"); 
			}
            va->surface[i].d3d=NULL;
			va->hw_surface[i]=0;
	}
		
	

	va->surface_count = 0;
}

static int DxResetVideoDecoder(kk_va_dxva2_t *va)
{
	av_log(NULL, AV_LOG_ERROR, "DxResetVideoDecoder unimplemented\n");
	return -1;
}
void Close_Kk_Va_Dxva2(kk_va_dxva2_t *external,bool bFull)
{
	kk_va_dxva2_t *va = external;

	DxDestroyVideoDecoder(va);


	if(bFull)
	{

		
		if (va->device){
				HRESULT  hr=va->devmng->CloseDeviceHandle(va->device);
				if (FAILED(hr))
				{
					printf("Error!\n"); 
				}
				va->device=0;
				hr=0;
			}

		if (va->vs){
			HRESULT  hr=va->vs->Release();
			if (FAILED(hr))
			{
				printf("Error!\n"); 
			}
			va->vs=NULL;
		}



		
		if (va->devmng)
		{
			HRESULT  hr=va->devmng->Release();
			va->devmng=NULL;
		}

		

		
	   av_frame_unref(va->tmp_frame);
       av_frame_free(&va->tmp_frame);
	   free(va);
	}
}

void FroceClose_Kk_Va_Dxva2(void *kk_va)
{
     kk_va_dxva2_t *external=(kk_va_dxva2_t*)kk_va;
	 Close_Kk_Va_Dxva2(external,true);
}
/**
* Find the best suited decoder mode GUID and render format.
*/
static int DxFindVideoServiceConversion(kk_va_dxva2_t *va, GUID *input, D3DFORMAT *output)
{
	/* Retrieve supported modes from the decoder service */
	UINT input_count = 0;
	GUID *input_list = NULL;
	if (FAILED(va->vs->GetDecoderDeviceGuids(&input_count, &input_list))) {
		av_log(NULL, AV_LOG_ERROR, "IDirectXVideoDecoderService_GetDecoderDeviceGuids failed\n");
		return -1;
	}
	for (unsigned i = 0; i < input_count; i++) {
		const GUID &g = input_list[i];
		const dxva2_mode_t *mode = Dxva2FindMode(g);
		if (mode) {
			av_log(NULL, AV_LOG_INFO, "- '%s' is supported by hardware\n", mode->name);
		} else {
			av_log(NULL, AV_LOG_WARNING, "- Unknown GUID = %08X-%04x-%04x-XXXX\n",
				(unsigned)g.Data1, g.Data2, g.Data3);
		}
	}

	/* Try all supported mode by our priority */
	for (unsigned i = 0; dxva2_modes[i].name; i++) {
		const dxva2_mode_t *mode = &dxva2_modes[i];
		if (!mode->codec || mode->codec != va->codec_id)
			continue;

		/* */
		bool is_suported = false;
		for (unsigned count = 0; !is_suported && count < input_count; count++) {
			const GUID &g = input_list[count];
			is_suported = IsEqualGUID(mode->guid, g) == 0;
		}
		if (!is_suported)
			continue;

		/* */
		av_log(NULL, AV_LOG_DEBUG, "Trying to use '%s' as input\n", mode->name);
		UINT      output_count = 0;
		D3DFORMAT *output_list = NULL;
		if (FAILED(va->vs->GetDecoderRenderTargets( mode->guid,
			&output_count,
			&output_list))) 
		{
				av_log(NULL, AV_LOG_ERROR, "IDirectXVideoDecoderService_GetDecoderRenderTargets failed\n");
				continue;
		}
		for (unsigned j = 0; j < output_count; j++) 
		{
			const D3DFORMAT f = output_list[j];
			const d3d_format_t *format = D3dFindFormat(f);
			if (format) {
				av_log(NULL, AV_LOG_DEBUG, "%s is supported for output\n", format->name);
			} else {
				av_log(NULL, AV_LOG_DEBUG, "%d is supported for output (%4.4s)\n", f, (const char*)&f);
			}
		}

		/* */
		for (unsigned j = 0; d3d_formats[j].name; j++)
		{
			const d3d_format_t *format = &d3d_formats[j];

			/* */
			bool is_suported = false;
			for (unsigned k = 0; !is_suported && k < output_count; k++) {
				is_suported = format->format == output_list[k];
			}
			if (!is_suported)
				continue;

			/* We have our solution */
			av_log(NULL, AV_LOG_DEBUG, "Using '%s' to decode to '%s'\n", mode->name, format->name);
			*input  = mode->guid;
			*output = format->format;
			CoTaskMemFree(output_list);
			CoTaskMemFree(input_list);
			return 0;
		}
		CoTaskMemFree(output_list);
	}
	CoTaskMemFree(input_list);
	return -1;
}

/**
* It creates a DirectX video service
*/
static int DxCreateVideoService(kk_va_dxva2_t *va)
{
	typedef HRESULT (WINAPI *CreateVideoService_func)(IDirect3DDevice9 *,
		REFIID riid,
		void **ppService);
	CreateVideoService_func CreateVideoService =
		(CreateVideoService_func)GetProcAddress(G_hdxva2_dll,
		LPCSTR("DXVA2CreateVideoService"));

	if (!CreateVideoService)
	{
		return 4;
	}
	
	HRESULT hr;

	HANDLE device;
	hr = va->devmng->OpenDeviceHandle(&device);
	if (FAILED(hr)) {
		return -1;
	}
	va->device = device;


	IDirectXVideoDecoderService *vs;
	hr = va->devmng->GetVideoService(device,
		IID_IDirectXVideoDecoderService,
		(void **)&vs);
	if (FAILED(hr)) 
	{
		return -1;
	}
	va->vs = vs;

	return 0;
}

/**
* It creates a Direct3D device manager
*/
static int D3dCreateDeviceManager(kk_va_dxva2_t *va)
{
	typedef HRESULT (WINAPI *CreateDeviceManager9_func)(UINT *pResetToken, IDirect3DDeviceManager9 **);
	CreateDeviceManager9_func CreateDeviceManager9 =
		(CreateDeviceManager9_func)GetProcAddress(G_hdxva2_dll, LPCSTR("DXVA2CreateDirect3DDeviceManager9"));

	if (!CreateDeviceManager9)
	{
		return -1;
	}

	UINT token;
	IDirect3DDeviceManager9 *devmng;
	if (FAILED(CreateDeviceManager9(&token, &devmng)))
	{
		return -1;
	}

	HRESULT hr = devmng->ResetDevice(G_pD3Ddev, token);
	if (FAILED(hr))
	{
		return -1;
	}

	devmng->AddRef();
	va->token  = token;
	va->devmng = devmng;
	return 0;
}
kk_va_dxva2_t *vlc_va_NewDxva2(int codec_id,kk_va_dxva2_t *va)
{

	va->codec_id = codec_id;
	/* Load dll*/
	if(G_hd3d9_dll==NULL)
	G_hd3d9_dll = LoadLibrary(TEXT("D3D9.DLL"));
	if (!G_hd3d9_dll)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot load d3d9.dll\n");
		goto error;
	}
	if(G_hdxva2_dll==NULL)
	G_hdxva2_dll = LoadLibrary(TEXT("DXVA2.DLL"));

	if (!G_hdxva2_dll) 
	{
		av_log(NULL, AV_LOG_ERROR, "cannot load dxva2.dll\n");
		goto error;
	}
	av_log(NULL, AV_LOG_INFO, "DLLs loaded\n");

	/* */
	if (!G_pD3Ddev&&D3dCreateDevice(va))
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to create Direct3D device\n");
		goto error;
	}
	av_log(NULL, AV_LOG_INFO, "D3dCreateDevice succeed\n");

	if (!va->devmng &&D3dCreateDeviceManager(va)) {
		av_log(NULL, AV_LOG_ERROR, "D3dCreateDeviceManager failed\n");
		goto error;
	}

	if (!va->vs&&DxCreateVideoService(va))
	{
		av_log(NULL, AV_LOG_ERROR, "DxCreateVideoService failed\n");
		goto error;
	}

	/* */
	if (DxFindVideoServiceConversion(va, &va->input, &va->render))
	{
		av_log(NULL, AV_LOG_ERROR, "DxFindVideoServiceConversion failed\n");
		goto error;
	}

	/* TODO print the hardware name/vendor for debugging purposes */
	return va;

error:
	Close_Kk_Va_Dxva2(va,true);
	return NULL;
}

/**
* It creates a DXVA2 decoder using the given video format
*/
static int DxCreateVideoDecoder(kk_va_dxva2_t *va,
	int codec_id, const video_format_t *fmt)
{
	/* */
	av_log(NULL, AV_LOG_DEBUG, "DxCreateVideoDecoder id %d %dx%d\n",
		codec_id, fmt->i_width, fmt->i_height);

	if(va->vs==NULL)
		return -1;

	va->width  = fmt->i_width;
	va->height = fmt->i_height;

	/* Allocates all surfaces needed for the decoder */
	va->surface_width  = (fmt->i_width  + 15) & ~15;
	va->surface_height = (fmt->i_height + 15) & ~15;
	switch (codec_id) {
	case AV_CODEC_ID_H264:
		va->surface_count = 16 + 1;
		break;
	default:
		va->surface_count = 2 + 1;
		break;
	}
	LPDIRECT3DSURFACE9 surface_list[VA_DXVA2_MAX_SURFACE_COUNT];
	if (FAILED(va->vs->CreateSurface(va->surface_width,
		va->surface_height,
		va->surface_count - 1,
		va->render,
		D3DPOOL_DEFAULT,
		0,
		DXVA2_VideoDecoderRenderTarget,
		surface_list,
		NULL))) {
			av_log(NULL, AV_LOG_ERROR, "IDirectXVideoAccelerationService_CreateSurface failed\n");
			va->surface_count = 0;
			return -1;
	}
	for (unsigned i = 0; i < va->surface_count; i++) {
		vlc_va_surface_t *surface = &va->surface[i];
		surface->d3d = surface_list[i];
		surface->age = UINT64_MAX;
		surface->index = i;
		surface->used=0;
		surface->usedCount=0;

		// fill the surface in black, to avoid the "green screen" in case the first frame fails to decode.
		G_pD3Ddev->ColorFill(surface->d3d, NULL, D3DCOLOR_XYUV(0, 128, 128));
		
	}
	av_log(NULL, AV_LOG_DEBUG, "IDirectXVideoAccelerationService_CreateSurface succeed with %d surfaces (%dx%d)\n",
		va->surface_count, fmt->i_width, fmt->i_height);

	/* */
	DXVA2_VideoDesc dsc;
	ZeroMemory(&dsc, sizeof(dsc));
	dsc.SampleWidth     = fmt->i_width;
	dsc.SampleHeight    = fmt->i_height;
	dsc.Format          = va->render;
	if (fmt->i_frame_rate > 0 && fmt->i_frame_rate_base > 0) {
		dsc.InputSampleFreq.Numerator   = fmt->i_frame_rate;
		dsc.InputSampleFreq.Denominator = fmt->i_frame_rate_base;
	} else {
		dsc.InputSampleFreq.Numerator   = 0;
		dsc.InputSampleFreq.Denominator = 0;
	}
	dsc.OutputFrameFreq = dsc.InputSampleFreq;
	dsc.UABProtectionLevel = FALSE;
	dsc.Reserved = 0;

	/* FIXME I am unsure we can let unknown everywhere */
	DXVA2_ExtendedFormat *ext = &dsc.SampleFormat;
	ext->SampleFormat = 0;//DXVA2_SampleUnknown;
	ext->VideoChromaSubsampling = 0;//DXVA2_VideoChromaSubsampling_Unknown;
	ext->NominalRange = 0;//DXVA2_NominalRange_Unknown;
	ext->VideoTransferMatrix = 0;//DXVA2_VideoTransferMatrix_Unknown;
	ext->VideoLighting = 0;//DXVA2_VideoLighting_Unknown;
	ext->VideoPrimaries = 0;//DXVA2_VideoPrimaries_Unknown;
	ext->VideoTransferFunction = 0;//DXVA2_VideoTransFunc_Unknown;

	/* List all configurations available for the decoder */
	UINT                      cfg_count = 0;
	DXVA2_ConfigPictureDecode *cfg_list = NULL;
	if (FAILED(va->vs->GetDecoderConfigurations(va->input,
		&dsc,	
		NULL,
		&cfg_count,
		&cfg_list))) {
			av_log(NULL, AV_LOG_ERROR, "IDirectXVideoDecoderService_GetDecoderConfigurations failed\n");
			return -1;
	}
	av_log(NULL, AV_LOG_DEBUG, "we got %d decoder configurations\n", cfg_count);

	/* Select the best decoder configuration */
	int cfg_score = 0;
	for (unsigned i = 0; i < cfg_count; i++)
	{
		const DXVA2_ConfigPictureDecode *cfg = &cfg_list[i];
		/* */
		av_log(NULL, AV_LOG_DEBUG, "configuration[%d] ConfigBitstreamRaw %d\n",
			i, cfg->ConfigBitstreamRaw);

		/* */
		int score;
		if (cfg->ConfigBitstreamRaw == 1)
			score = 1;
		else if (codec_id == AV_CODEC_ID_H264 && cfg->ConfigBitstreamRaw == 2)
			score = 2;
		else
			continue;
		if (IsEqualGUID(cfg->guidConfigBitstreamEncryption, DXVA_NoEncrypt))
			score += 16;

		if (cfg_score < score)
		{
			va->cfg = *cfg;
			cfg_score = score;
		}
	}
	CoTaskMemFree(cfg_list);
	cfg_list = NULL;

	if (cfg_score <= 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to find a supported decoder configuration\n");
		return -1;
	}

	/* Create the decoder */
	IDirectXVideoDecoder *decoder;
	if (FAILED(va->vs->CreateVideoDecoder(va->input,
		&dsc,
		&va->cfg,
		surface_list,
		va->surface_count,
		&decoder))) 
	{
			av_log(NULL, AV_LOG_ERROR, "IDirectXVideoDecoderService_CreateVideoDecoder failed\n");
			return -1;
	}
	va->decoder = decoder;
	av_log(NULL, AV_LOG_DEBUG, "IDirectXVideoDecoderService_CreateVideoDecoder succeed\n");
	return 0;
}


static void DxCreateVideoConversion(kk_va_dxva2_t *va)
{
	switch (va->render) {
	case MAKEFOURCC('N','V','1','2'):
		va->output = (D3DFORMAT)MAKEFOURCC('Y','V','1','2');
		break;
	default:
		va->output = va->render;
		break;
	}
	//    CopyInitCache(&va->surface_cache, va->surface_width);
}


static int Setup(kk_va_dxva2_t *external, void **hw, AVPixelFormat *chroma,
	int width, int height)
{
	kk_va_dxva2_t *va = external;

	if (va->width == width && va->height == height && va->decoder)
		goto ok;

	
	//DxDestroyVideoDecoder(va);

	*chroma = AV_PIX_FMT_NONE;
	if (width <= 0 || height <= 0)
		return -1;

	/* FIXME transmit a video_format_t by VaSetup directly */
	video_format_t fmt;
	memset(&fmt, 0, sizeof(fmt));
	fmt.i_width = width;
	fmt.i_height = height;

	if (DxCreateVideoDecoder(va, va->codec_id, &fmt))
		return -1;
	/* */
	va->hw.decoder = va->decoder;
	va->hw.cfg = &va->cfg;
	va->hw.surface_count = va->surface_count;
	va->hw.surface = va->hw_surface;
	for (unsigned i = 0; i < va->surface_count; i++)
		va->hw.surface[i] = va->surface[i].d3d;

	/* */
	DxCreateVideoConversion(va);

	/* */
ok:
	*hw = &va->hw;
	const d3d_format_t *output = D3dFindFormat(va->output);
	*chroma = output->codec;
	return 0;
}

typedef struct SurfaceWrapper {
	kk_va_dxva2_t  *pDec;
	AVFrame *pPic;
	LPDIRECT3DSURFACE9 surface;
	IDirectXVideoDecoder         *pDXDecoder;
} SurfaceWrapper;

static int Get(kk_va_dxva2_t *external, AVFrame *ff)
{
	kk_va_dxva2_t *va = external;
	
	/* Check the device */
	HRESULT hr = va->devmng->TestDevice(va->device);
	if (hr == DXVA2_E_NEW_VIDEO_DEVICE) {
		if (DxResetVideoDecoder(va))
			return -1;
	} else if (FAILED(hr)) {
		av_log(NULL, AV_LOG_ERROR, "IDirect3DDeviceManager9_TestDevice %u", (unsigned)hr);
		return -1;
	}

	
	
	unsigned i=0;
	int old, old_unused;
	for (i = 0, old = 0, old_unused = -1; i < va->surface_count; i++) {
		vlc_va_surface_t  *surface = &va->surface[i];
		if (!surface->used && (old_unused == -1 || surface->age <va->surface[old_unused].age))
			old_unused = i;
		if (surface->age < va->surface[i].age)
			old = i;
	}
	if (old_unused == -1) {
		//DbgLog((LOG_TRACE, 10, L"No free surface, using oldest"));
		i = old;
	} else {
		i = old_unused;
	}

	vlc_va_surface_t *surface = &va->surface[i];

	surface->used = true;
	surface->age = va->surface_order++;
    surface->usedCount++;

	
	memset(ff->data, 0, sizeof(ff->data));
	memset(ff->linesize, 0, sizeof(ff->linesize));
	memset(ff->buf, 0, sizeof(ff->buf));

	ff->data[0]=ff->data[3] = (uint8_t *)surface->d3d;
	return i;
}



template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
static void ffmpeg_ReleaseFrameBuf( void *p1,
	uint8_t *p2 )
{
	
	 SurfaceWrapper *sw=(SurfaceWrapper *)p1;

	AVFrame *p_ff_pic=(AVFrame *)sw->pPic;
	kk_va_dxva2_t *va = (kk_va_dxva2_t *)sw->pDec;

	LPDIRECT3DSURFACE9 pSurface = sw->surface;
	for (int i = 0; i < va->surface_count; i++) {
		if (va->surface[i].d3d == pSurface) {
			va->surface[i].used = false;
			va->surface[i].usedCount--;
			break;
		}
	}
	
	SafeRelease(&pSurface);
	SafeRelease(&sw->pDXDecoder);
	delete sw;

	if(p2!=NULL)
	{
		int xx=0;
		xx++;
	}
}

/*****************************************************************************
* ffmpeg_GetFrameBuf: callback used by ffmpeg to get a frame buffer.
*****************************************************************************
* It is used for direct rendering as well as to get the right PTS for each
* decoded picture (even in indirect rendering mode).
*****************************************************************************/
static int ffmpeg_GetFrameBuf( struct AVCodecContext *p_context,
	AVFrame *p_ff_pic,int flags)
{
	kk_va_dxva2_t *va = (kk_va_dxva2_t *)p_context->opaque;
	AVPixelFormat chroma;
	if( va )
	{
		/* hwaccel_context is not present in old ffmpeg version */
		if( Setup( va,&p_context->hwaccel_context, &chroma,p_context->width, p_context->height ) )
		{
			av_log(NULL, AV_LOG_ERROR, "vlc_va_Setup failed" );
			return -1;
		}
		int ix=Get( va, p_ff_pic );
		if( ix<0 )
		{
			av_log(NULL, AV_LOG_ERROR, "VaGrabSurface failed" );
			return -1;
		}

		SurfaceWrapper *surfaceWrapper = new SurfaceWrapper();
		surfaceWrapper->pDec =va ;
		surfaceWrapper->pPic=p_ff_pic;
		surfaceWrapper->surface=va->surface[ix].d3d;
		surfaceWrapper->surface->AddRef();
		surfaceWrapper->pDXDecoder=va->decoder;
		surfaceWrapper->pDXDecoder->AddRef();
		
		p_ff_pic->buf[0] = av_buffer_create(NULL, 0, ffmpeg_ReleaseFrameBuf, surfaceWrapper, 0);
		return 0;
	}
	return -1;
}



static enum AVPixelFormat ffmpeg_GetFormat( AVCodecContext *p_context,
	const enum AVPixelFormat *pi_fmt )
{
	kk_va_dxva2_t *va = (kk_va_dxva2_t *)p_context->opaque;

	if( va )
	{
		Close_Kk_Va_Dxva2( va,false );
	}

	/* Try too look for a supported hw acceleration */
	for( int i = 0; pi_fmt[i] != AV_PIX_FMT_NONE; i++ )
	{
		static const char *ppsz_name[AV_PIX_FMT_NB] = {NULL};
		ppsz_name[AV_PIX_FMT_VDPAU_H264] = "PIX_FMT_VDPAU_H264";
		ppsz_name[AV_PIX_FMT_VAAPI_IDCT] = "PIX_FMT_VAAPI_IDCT";
		ppsz_name[AV_PIX_FMT_VAAPI_VLD] = "PIX_FMT_VAAPI_VLD";
		ppsz_name[AV_PIX_FMT_VAAPI_MOCO] = "PIX_FMT_VAAPI_MOCO";
		ppsz_name[AV_PIX_FMT_DXVA2_VLD] = "PIX_FMT_DXVA2_VLD";
		ppsz_name[AV_PIX_FMT_YUYV422] = "PIX_FMT_YUYV422";
		ppsz_name[AV_PIX_FMT_YUV420P] = "PIX_FMT_YUV420P";

		/* Only VLD supported */
		if( pi_fmt[i] == AV_PIX_FMT_DXVA2_VLD )
		{
			av_log(p_context, AV_LOG_DEBUG, "Trying DXVA2\n" );
			vlc_va_NewDxva2( p_context->codec_id,va);
			if( !va )
				av_log(NULL, AV_LOG_ERROR, "Failed to open DXVA2\n" );
		}

		if( va &&
			p_context->width > 0 && p_context->height > 0 )
		{
			
			if( Setup( va,
				&p_context->hwaccel_context,
				&p_context->pix_fmt,
				p_context->width, p_context->height ) )
			{
				Close_Kk_Va_Dxva2( va,false);
				va = NULL;
			}
		}

		if( va )
		{
			p_context->opaque = va;
			return pi_fmt[i];
		}
	}

	return avcodec_default_get_format( p_context, pi_fmt );
}


static int dxva_Extract(kk_va_dxva2_t *va, AVFrame *src,AVFrame **Out)
{
    LPDIRECT3DSURFACE9 d3d = (LPDIRECT3DSURFACE9)(uintptr_t)src->data[3];
	if(va->output != MAKEFOURCC('Y','V','1','2'))
		return -1;

	D3DLOCKED_RECT lock;
	if (FAILED(IDirect3DSurface9_LockRect(d3d, &lock, NULL, D3DLOCK_READONLY))) {
		return -1;
	}

	if (va->render == MAKEFOURCC('Y','V','1','2') ||va->render == MAKEFOURCC('I','M','C','3'))
	{
		if(!va->Okxx)
		{
			va->tmp_frame->width  =lock.Pitch;//FFALIGN(va->width, 64);
			va->tmp_frame->height =va->height;//FFALIGN(va->height, 2);
			va->tmp_frame->format = AV_PIX_FMT_NV12;

			va->Okxx=true;
			int ret = av_frame_get_buffer(va->tmp_frame, 32);
			if (ret < 0)
				return ret;
		}

		CopyFrameNV12((BYTE *)lock.pBits,va->tmp_frame->data[0],va->tmp_frame->data[1], va->surface_height, va->height,lock.Pitch);
		if(Out!=NULL)
			*Out=va->tmp_frame;
	}
	else
	{
		if(!va->Okxx)
		{
			va->tmp_frame->width  =lock.Pitch;//FFALIGN(va->width, 64);
			va->tmp_frame->height =va->height;//FFALIGN(va->height, 2);
			va->tmp_frame->format = AV_PIX_FMT_NV12;

			va->Okxx=true;
			int ret = av_frame_get_buffer(va->tmp_frame, 32);
			if (ret < 0)
				return ret;
		}
		
		CopyFrameNV12((BYTE *)lock.pBits,va->tmp_frame->data[0],va->tmp_frame->data[1], va->surface_height, va->height,lock.Pitch);		
		if(Out!=NULL)
		   *Out=va->tmp_frame;
	}
	IDirect3DSurface9_UnlockRect(d3d);
	return 0;
}
int DxPictureCopy(struct AVCodecContext *avctx,AVFrame *src,AVFrame **Out)
{
	kk_va_dxva2_t *p_va = (kk_va_dxva2_t *)avctx->opaque;

	return dxva_Extract(p_va,src,Out);
}

int BindDxva2Module(	AVCodecContext  *pCodecCtx)
{

	kk_va_dxva2_t *dxva =(kk_va_dxva2_t *) malloc(sizeof(kk_va_dxva2_t));
	memset(dxva,0,sizeof(kk_va_dxva2_t));
	if(NULL == dxva)
		return -1;
	pCodecCtx->opaque = dxva;
	kk_va_dxva2_t * dxs=vlc_va_NewDxva2(pCodecCtx->codec_id,dxva);
	if(dxs==NULL)
	{
        dxva =NULL;
		return -1;
	}

	dxva->tmp_frame= av_frame_alloc();
	pCodecCtx->get_format = ffmpeg_GetFormat;
	pCodecCtx->get_buffer2 = ffmpeg_GetFrameBuf;
	pCodecCtx->thread_count = 1;
	pCodecCtx->slice_flags    |= SLICE_FLAG_ALLOW_FIELD;
	
	int res = Setup(dxva, &pCodecCtx->hwaccel_context, &pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	if (res < 0)
	{
		free(dxva);
		dxva=NULL;
		return res;
	}

	
	
	
	return 0;
}