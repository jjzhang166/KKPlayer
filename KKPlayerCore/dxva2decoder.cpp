// Dxva2Decoder.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Dxva2Def.h"
#include "dxva2decoder.h"
#include "libavutil/buffer.h"
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
static int D3dCreateDevice(vlc_va_dxva2_t *va)
{
	/* */
	typedef LPDIRECT3D9 (WINAPI *Create9func)(UINT SDKVersion);
	Create9func Create9 = (Create9func )GetProcAddress(va->hd3d9_dll, LPCSTR("Direct3DCreate9"));

	if (!Create9) {
		av_log(NULL, AV_LOG_ERROR, "Cannot locate reference to Direct3DCreate9 ABI in DLL");
		return -1;
	}

	/* */
	LPDIRECT3D9 d3dobj;
	d3dobj = Create9(D3D_SDK_VERSION);
	if (!d3dobj) {
		av_log(NULL, AV_LOG_ERROR, "Direct3DCreate9 failed");
		return -1;
	}
	va->d3dobj = d3dobj;

	/* */
	D3DADAPTER_IDENTIFIER9 *d3dai = &va->d3dai;
	if (FAILED(IDirect3D9_GetAdapterIdentifier(va->d3dobj,
		D3DADAPTER_DEFAULT, 0, d3dai))) {
			av_log(NULL, AV_LOG_WARNING, "IDirect3D9_GetAdapterIdentifier failed");
			ZeroMemory(d3dai, sizeof(*d3dai));
	}

	va->vendorId=d3dai->VendorId;
    va->tmp_frame= av_frame_alloc();
	IniCopyFrameNV12(va->vendorId);
	/* */
	D3DPRESENT_PARAMETERS *d3dpp = &va->d3dpp;
	ZeroMemory(d3dpp, sizeof(*d3dpp));
	d3dpp->Flags                  = D3DPRESENTFLAG_VIDEO;
	d3dpp->Windowed               = TRUE;
	d3dpp->hDeviceWindow          = NULL;
	d3dpp->SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp->MultiSampleType        = D3DMULTISAMPLE_NONE;
	d3dpp->PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp->BackBufferCount        = 0;                  /* FIXME what to put here */
	d3dpp->BackBufferFormat       = D3DFMT_X8R8G8B8;    /* FIXME what to put here */
	d3dpp->BackBufferWidth        = 0;
	d3dpp->BackBufferHeight       = 0;
	d3dpp->EnableAutoDepthStencil = FALSE;

	/* Direct3D needs a HWND to create a device, even without using ::Present
	this HWND is used to alert Direct3D when there's a change of focus window.
	For now, use GetShellWindow, as it looks harmless */
	LPDIRECT3DDEVICE9 d3ddev;
	if (FAILED(IDirect3D9_CreateDevice(d3dobj, D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, GetShellWindow(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING |
		D3DCREATE_MULTITHREADED,
		d3dpp, &d3ddev))) {
			av_log(NULL, AV_LOG_ERROR, "IDirect3D9_CreateDevice failed\n");
			return -1;
	}
	va->d3ddev = d3ddev;

	return 0;
}
/**
* It releases a Direct3D device and its resources.
*/
static void D3dDestroyDevice(vlc_va_dxva2_t *va)
{
	if (va->d3ddev)
		IDirect3DDevice9_Release(va->d3ddev);
	if (va->d3dobj)
		IDirect3D9_Release(va->d3dobj);
}
/**
* It destroys a Direct3D device manager
*/
static void D3dDestroyDeviceManager(vlc_va_dxva2_t *va)
{
	if (va->devmng)
		va->devmng->Release();
}
static void DxDestroyVideoService(vlc_va_dxva2_t *va)
{
	if (va->device)
		va->devmng->CloseDeviceHandle(va->device);
	if (va->vs)
		va->vs->Release();
}
static void DxDestroyVideoDecoder(vlc_va_dxva2_t *va)
{
	if (va->decoder)
		va->decoder->Release();
	va->decoder = NULL;

	for (unsigned i = 0; i < va->surface_count; i++)
		va->surface[i].d3d->Release();
	va->surface_count = 0;
}
static void DxDestroyVideoConversion(vlc_va_dxva2_t *va)
{
	//    CopyCleanCache(&va->surface_cache);
}
static int DxResetVideoDecoder(vlc_va_dxva2_t *va)
{
	av_log(NULL, AV_LOG_ERROR, "DxResetVideoDecoder unimplemented\n");
	return -1;
}
static void Close(vlc_va_dxva2_t *external)
{
	vlc_va_dxva2_t *va = external;

	DxDestroyVideoConversion(va);
	DxDestroyVideoDecoder(va);
	DxDestroyVideoService(va);
	D3dDestroyDeviceManager(va);
	D3dDestroyDevice(va);

	if (va->hdxva2_dll)
		FreeLibrary(va->hdxva2_dll);
	if (va->hd3d9_dll)
		FreeLibrary(va->hd3d9_dll);
	free(va);
}
/**
* Find the best suited decoder mode GUID and render format.
*/
static int DxFindVideoServiceConversion(vlc_va_dxva2_t *va, GUID *input, D3DFORMAT *output)
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
static int DxCreateVideoService(vlc_va_dxva2_t *va)
{
	typedef HRESULT (WINAPI *CreateVideoService_func)(IDirect3DDevice9 *,
		REFIID riid,
		void **ppService);
	CreateVideoService_func CreateVideoService =
		(CreateVideoService_func)GetProcAddress(va->hdxva2_dll,
		LPCSTR("DXVA2CreateVideoService"));

	if (!CreateVideoService)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot load function\n");
		return 4;
	}
	av_log(NULL, AV_LOG_INFO, "DXVA2CreateVideoService Success!\n");

	HRESULT hr;

	HANDLE device;
	hr = va->devmng->OpenDeviceHandle(&device);
	if (FAILED(hr)) {
		av_log(NULL, AV_LOG_ERROR, "OpenDeviceHandle failed\n");
		return -1;
	}
	va->device = device;

	IDirectXVideoDecoderService *vs;
	hr = va->devmng->GetVideoService(device,
		IID_IDirectXVideoDecoderService,
		(void **)&vs);
	if (FAILED(hr)) 
	{
		av_log(NULL, AV_LOG_ERROR, "GetVideoService failed\n");
		return -1;
	}
	va->vs = vs;

	return 0;
}

/**
* It creates a Direct3D device manager
*/
static int D3dCreateDeviceManager(vlc_va_dxva2_t *va)
{
	typedef HRESULT (WINAPI *CreateDeviceManager9_func)(UINT *pResetToken, IDirect3DDeviceManager9 **);
	CreateDeviceManager9_func CreateDeviceManager9 =
		(CreateDeviceManager9_func)GetProcAddress(va->hdxva2_dll, LPCSTR("DXVA2CreateDirect3DDeviceManager9"));

	if (!CreateDeviceManager9)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot load function\n");
		return -1;
	}
	av_log(NULL, AV_LOG_INFO, "OurDirect3DCreateDeviceManager9 Success!\n");

	UINT token;
	IDirect3DDeviceManager9 *devmng;
	if (FAILED(CreateDeviceManager9(&token, &devmng)))
	{
		av_log(NULL, AV_LOG_ERROR, " OurDirect3DCreateDeviceManager9 failed\n");
		return -1;
	}

	HRESULT hr = devmng->ResetDevice(va->d3ddev, token);
	if (FAILED(hr))
	{
		av_log(NULL, AV_LOG_ERROR, "IDirect3DDeviceManager9_ResetDevice failed: %08x", (unsigned)hr);
		return -1;
	}

	devmng->AddRef();
	va->token  = token;
	va->devmng = devmng;
	av_log(NULL, AV_LOG_INFO, "obtained IDirect3DDeviceManager9\n");

	return 0;
}
vlc_va_dxva2_t *vlc_va_NewDxva2(int codec_id)
{
	vlc_va_dxva2_t *va = (vlc_va_dxva2_t *)calloc(1, sizeof(*va));
	if (!va)
		return NULL;

	va->codec_id = codec_id;

	/* Load dll*/
	va->hd3d9_dll = LoadLibrary(TEXT("D3D9.DLL"));
	if (!va->hd3d9_dll)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot load d3d9.dll\n");
		goto error;
	}
	va->hdxva2_dll = LoadLibrary(TEXT("DXVA2.DLL"));
	if (!va->hdxva2_dll) 
	{
		av_log(NULL, AV_LOG_ERROR, "cannot load dxva2.dll\n");
		goto error;
	}
	av_log(NULL, AV_LOG_INFO, "DLLs loaded\n");

	/* */
	if (D3dCreateDevice(va))
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to create Direct3D device\n");
		goto error;
	}
	av_log(NULL, AV_LOG_INFO, "D3dCreateDevice succeed\n");

	if (D3dCreateDeviceManager(va)) {
		av_log(NULL, AV_LOG_ERROR, "D3dCreateDeviceManager failed\n");
		goto error;
	}

	if (DxCreateVideoService(va))
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
	Close(va);
	return NULL;
}

/**
* It creates a DXVA2 decoder using the given video format
*/
static int DxCreateVideoDecoder(vlc_va_dxva2_t *va,
	int codec_id, const video_format_t *fmt)
{
	/* */
	av_log(NULL, AV_LOG_DEBUG, "DxCreateVideoDecoder id %d %dx%d\n",
		codec_id, fmt->i_width, fmt->i_height);

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
		surface->refcount = 0;
		surface->order = 0;
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


static void DxCreateVideoConversion(vlc_va_dxva2_t *va)
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


static int Setup(vlc_va_dxva2_t *external, void **hw, AVPixelFormat *chroma,
	int width, int height)
{
	vlc_va_dxva2_t *va = external;

	if (va->width == width && va->height == height && va->decoder)
		goto ok;

	/* */
	DxDestroyVideoConversion(va);
	DxDestroyVideoDecoder(va);

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
	vlc_va_dxva2_t  *pDec;
	AVFrame *pPic;
} SurfaceWrapper;

static int Get(vlc_va_dxva2_t *external, AVFrame *ff)
{
	vlc_va_dxva2_t *va = external;

	/* Check the device */
	HRESULT hr = va->devmng->TestDevice(va->device);
	if (hr == DXVA2_E_NEW_VIDEO_DEVICE) {
		if (DxResetVideoDecoder(va))
			return -1;
	} else if (FAILED(hr)) {
		av_log(NULL, AV_LOG_ERROR, "IDirect3DDeviceManager9_TestDevice %u", (unsigned)hr);
		return -1;
	}

	/* Grab an unused surface, in case none are, try the oldest
	* XXX using the oldest is a workaround in case a problem happens with ffmpeg */
	unsigned i, old;
	for (i = 0, old = 0; i < va->surface_count; i++) {
		vlc_va_surface_t *surface = &va->surface[i];

		if (!surface->refcount)
			break;

		if (surface->order < va->surface[old].order)
			old = i;
	}
	if (i >= va->surface_count)
		i = old;

	vlc_va_surface_t *surface = &va->surface[i];

	surface->refcount = 1;
	surface->order = va->surface_order++;

	/* */
	for (int i = 0; i < 4; i++) {
		ff->data[i] = NULL;
		ff->linesize[i] = 0;

		if (i == 0 || i == 3)
			ff->data[i] = (uint8_t *)surface->d3d;/* Yummie */
	}


	

	return 0;
}


static void Release(vlc_va_dxva2_t *external, AVFrame *ff)
{
	vlc_va_dxva2_t *va = external;
	LPDIRECT3DSURFACE9 d3d = (LPDIRECT3DSURFACE9)(uintptr_t)ff->data[3];

	for (unsigned i = 0; i < va->surface_count; i++) {
		vlc_va_surface_t *surface = &va->surface[i];

		if (surface->d3d == d3d)
			surface->refcount--;
	}
}

static void ffmpeg_ReleaseFrameBuf( void *p1,
	uint8_t *p2 )
{
	
	 SurfaceWrapper *surfaceWrapper=(SurfaceWrapper *)p1;

		AVFrame *p_ff_pic=(AVFrame *)surfaceWrapper->pPic;
	vlc_va_dxva2_t *va = (vlc_va_dxva2_t *)surfaceWrapper->pDec;

	if( va )
	{
		Release( va, p_ff_pic );
	}
	else if( !p_ff_pic->opaque )
	{
		/* We can end up here without the AVFrame being allocated by
		* avcodec_default_get_buffer() if VA is used and the frame is
		* released when the decoder is closed
		*/
		//if( p_ff_pic->pict_type== AV_PICTURE_TYPE_NONE )
			//avcodec_default_release_buffer( p_context, p_ff_pic );
	}
	delete surfaceWrapper;
	/*for( int i = 0; i < 4; i++ )
		p_ff_pic->data[i] = NULL;*/
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
	vlc_va_dxva2_t *va = (vlc_va_dxva2_t *)p_context->opaque;

	/* */
	p_ff_pic->reordered_opaque = p_context->reordered_opaque;
	p_ff_pic->opaque = NULL;

	AVPixelFormat chroma;
	if( va )
	{
		/* hwaccel_context is not present in old ffmpeg version */
		if( Setup( va,
			&p_context->hwaccel_context, &chroma,
			p_context->width, p_context->height ) )
		{
			av_log(NULL, AV_LOG_ERROR, "vlc_va_Setup failed" );
			return -1;
		}

		if( Get( va, p_ff_pic ) )
		{
			av_log(NULL, AV_LOG_ERROR, "VaGrabSurface failed" );
			return -1;
		}

		SurfaceWrapper *surfaceWrapper = new SurfaceWrapper();
		surfaceWrapper->pDec =va ;
		surfaceWrapper->pPic=p_ff_pic;
		////surfaceWrapper->sample = pSample;
		//surfaceWrapper->surface = surface->d3d;
		//surfaceWrapper->surface->AddRef();

		
		p_ff_pic->buf[0] = av_buffer_create(NULL, 0, ffmpeg_ReleaseFrameBuf, surfaceWrapper, 0);
		return 0;
	}
	else if( /*!p_sys->b_direct_rendering*/ 1 )
	{
		/* Not much to do in indirect rendering mode. */
		return avcodec_default_get_buffer2( p_context, p_ff_pic,AV_CODEC_CAP_DR1 );
	}
}


static int  ffmpeg_ReGetFrameBuf( struct AVCodecContext *p_context, AVFrame *p_ff_pic )
{
	p_ff_pic->reordered_opaque = p_context->reordered_opaque;

	return 0;
	/* We always use default reget function, it works perfectly fine */
	//return avcodec_
	//return avcodec_default_release_buffer( p_context, p_ff_pic );
}



static enum AVPixelFormat ffmpeg_GetFormat( AVCodecContext *p_context,
	const enum AVPixelFormat *pi_fmt )
{
	vlc_va_dxva2_t *va = (vlc_va_dxva2_t *)p_context->opaque;

	if( va )
	{
		Close( va );
		va = NULL;
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

		av_log(p_context, AV_LOG_DEBUG, "Available decoder output format %d (%s)\n", pi_fmt[i], ppsz_name[pi_fmt[i]] ? ppsz_name[pi_fmt[i]] : "Unknown" );

		/* Only VLD supported */
		if( pi_fmt[i] == AV_PIX_FMT_DXVA2_VLD )
		{
			av_log(p_context, AV_LOG_DEBUG, "Trying DXVA2\n" );
			va = vlc_va_NewDxva2( p_context->codec_id );
			if( !va )
				av_log(NULL, AV_LOG_ERROR, "Failed to open DXVA2\n" );
		}

		if( va &&
			p_context->width > 0 && p_context->height > 0 )
		{
			/* We try to call vlc_va_Setup when possible to detect errors when
			* possible (later is too late) */
			if( Setup( va,
				&p_context->hwaccel_context,
				&p_context->pix_fmt,
				p_context->width, p_context->height ) )
			{
				av_log(NULL, AV_LOG_ERROR, "vlc_va_Setup failed\n" );
				Close( va );
				va = NULL;
			}
		}

		if( va )
		{
			p_context->draw_horiz_band = NULL;
			p_context->opaque = va;
			return pi_fmt[i];
		}
	}

	/* Fallback to default behaviour */
	return avcodec_default_get_format( p_context, pi_fmt );
}

#define PRINT_LEN 10

static void hb_copy_from_nv12( uint8_t *dst, uint8_t *src[2], size_t src_pitch[2], unsigned width, unsigned height, AVFrame *ff )
{
	unsigned int i, j;
	uint8_t *dstU, *dstV;
	dstU = dst + width*height;
	dstV = dstU + width*height/4;
	unsigned int heithtUV, widthUV;
	heithtUV = height/2;
	widthUV = width/2;

	for( i = 0; i<height; i++ ) //Y
	{
		memcpy( dst+i*width, src[0]+i*src_pitch[0], width );
	}

	for( i = 0; i<heithtUV; i++ )
	{
		for( j = 0; j<widthUV; j++ )
		{
			dstU[i*widthUV+j] = *(src[1]+i*src_pitch[1]+2*j);
			dstV[i*widthUV+j] = *(src[1]+i*src_pitch[1]+2*j+1);
		}
	}
	/*
	for(int i = 0;i < PRINT_LEN;i++)
	{
		printf("dstU[%d] = %d,",i,dstU[i]);
	}
	printf("\n");
	for(int i = 0;i < PRINT_LEN;i++)
	{
		printf("dstV[%d] = %d,",i,dstV[i]);
	}
	printf("\n");
	*/
}

static void hb_copy_from_nv12_NEW( uint8_t *dst, uint8_t *src[2], size_t src_pitch[2], unsigned width, unsigned height, AVFrame *ff )
{
	unsigned int i, j;
	uint8_t *dstU, *dstV;
	dstU = dst + width*height*4;
	dstV = dstU + width*height;


	for( i = 0; i<height; i++ )
	{
		for( j = 0; j<width; j++ )
		{
			dst[4 * i*width + j] = *(src[0] + 2 * i * src_pitch[0] + j);
			dst[(4 * i + 1)*width + j] = *(src[0] + 2 * i * src_pitch[0] + j + width);
			dst[(4 * i + 2)*width + j] = *(src[0] + (2 * i + 1) * src_pitch[0] + j);
			dst[(4 * i + 3)*width + j] = *(src[0] + (2 * i + 1) * src_pitch[0] + j + width);

			dstU[i*width+j] = *(src[1]+i*src_pitch[1]+2*j);
			dstV[i*width+j] = *(src[1]+i*src_pitch[1]+2*j+1);
		}
	}
	
}

int hb_va_extract( vlc_va_dxva2_t *dxva2, uint8_t *dst, AVFrame *frame)

{
	LPDIRECT3DSURFACE9 d3d = (LPDIRECT3DSURFACE9)(uintptr_t)frame->data[3];
	D3DLOCKED_RECT lock;
	if( FAILED( IDirect3DSurface9_LockRect( d3d, &lock, NULL, D3DLOCK_READONLY )))
	{
		av_log(NULL, AV_LOG_ERROR, "Trying DXVA2\n" );
		return FALSE;
	}

	if( dxva2->render == MAKEFOURCC( 'N', 'V', '1', '2' ))
	{
		uint8_t *plane[2] =
		{
			(uint8_t *)lock.pBits,
			(uint8_t*)lock.pBits + lock.Pitch * dxva2->surface_height
		};
		size_t  pitch[2] =
		{
			lock.Pitch,
			lock.Pitch,
		};
		hb_copy_from_nv12_NEW(dst, plane, pitch, dxva2->width/2, dxva2->height/2, frame);
//		hb_copy_from_nv12( dst, plane, pitch, dxva2->width, dxva2->height, frame );
	}
	IDirect3DSurface9_UnlockRect( d3d );

	return TRUE;
}

static void CopyPlane(uint8_t *dst, size_t dst_pitch,
	const uint8_t *src, size_t src_pitch,
	unsigned width, unsigned height)
{
	unsigned y;
	for (y = 0; y < height; y++) {
		memcpy(dst, src, width);
		src += src_pitch;
		dst += dst_pitch;
	}
}

static void SplitPlanes(uint8_t *dstu, size_t dstu_pitch,
	uint8_t *dstv, size_t dstv_pitch,
	const uint8_t *src, size_t src_pitch,
	unsigned width, unsigned height)
{
	unsigned x,y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dstu[x] = src[2*x+0];
			dstv[x] = src[2*x+1];
		}
		src  += src_pitch;
		dstu += dstu_pitch;
		dstv += dstv_pitch;
	}
}

static void CopyFromNv12(AVFrame *dst, uint8_t *src[2], size_t src_pitch[2],
	unsigned width, unsigned height)
{
	CopyPlane(dst->data[0], dst->linesize[0],
		src[0], src_pitch[0],
		width, height);
	SplitPlanes(dst->data[2], dst->linesize[2],
		dst->data[1], dst->linesize[1],
		src[1], src_pitch[1],
		width/2, height/2);
}

static void CopyFromYv12(AVFrame *dst, uint8_t *src[3], size_t src_pitch[3],
	unsigned width, unsigned height)
{
	CopyPlane(dst->data[0], dst->linesize[0],
		src[0], src_pitch[0], width, height);
	CopyPlane(dst->data[1], dst->linesize[1],
		src[1], src_pitch[1], width / 2, height / 2);
	CopyPlane(dst->data[2], dst->linesize[2],
		src[1], src_pitch[2], width / 2, height / 2);
}

static int dxva_Extract(vlc_va_dxva2_t *va, AVFrame *src, AVFrame *dst)
{
	LPDIRECT3DSURFACE9 d3d = (LPDIRECT3DSURFACE9)(uintptr_t)src->data[3];

	if (!dst->data[0])
		return -1;

	if(va->output != MAKEFOURCC('Y','V','1','2'))
		return -1;

	D3DLOCKED_RECT lock;
	if (FAILED(IDirect3DSurface9_LockRect(d3d, &lock, NULL, D3DLOCK_READONLY))) {
		av_log(NULL,AV_LOG_ERROR, "Failed to lock surface");
		return -1;
	}

	if (va->render == MAKEFOURCC('Y','V','1','2') ||va->render == MAKEFOURCC('I','M','C','3'))
	{
		int  imc3 = va->render == MAKEFOURCC('I','M','C','3');
		size_t chroma_pitch = imc3 ? lock.Pitch : (lock.Pitch / 2);

		size_t pitch[3] = 
		{
			lock.Pitch,
			chroma_pitch,
			chroma_pitch,
		};

		uint8_t *plane[3] = 
		{
			(uint8_t*)lock.pBits,
			(uint8_t*)lock.pBits + pitch[0] * va->surface_height,
			(uint8_t*)lock.pBits + pitch[0] * va->surface_height
			+ pitch[1] * va->surface_height / 2,
		};

		if (imc3) 
		{
			uint8_t *V = plane[1];
			plane[1] = plane[2];
			plane[2] = V;
		}
		CopyFromYv12(dst, plane, pitch,va->width, va->height);
	}
	else
	{
		/*uint8_t *plane[2] = {
			(uint8_t*)lock.pBits,
			(uint8_t*)lock.pBits + lock.Pitch * va->surface_height
		};
		size_t  pitch[2] = {
			lock.Pitch,
			lock.Pitch,
		};*/

		// int alignedHeight = FFALIGN(va->height, 2);
		//CopyFromNv12(dst, plane, pitch,va->width, va->height);

		AVFrame * tmp_frame= av_frame_alloc();
		tmp_frame->width  =va->surface_width;
		tmp_frame->height =va->surface_height;
	    tmp_frame->format = AV_PIX_FMT_NV12;

		int ret = av_frame_get_buffer(tmp_frame, 32);
		if (ret < 0)
			return ret;

		CopyFrameNV12((BYTE *)lock.pBits,tmp_frame->data[0],tmp_frame->data[1], va->surface_height, va->height,lock.Pitch);
	}
	IDirect3DSurface9_UnlockRect(d3d);
	return 0;
}

int DxPictureCopy(struct AVCodecContext *avctx,AVFrame *src, AVFrame* dst)
{
	vlc_va_dxva2_t *p_va = (vlc_va_dxva2_t *)avctx->opaque;
	return dxva_Extract(p_va,src,dst);
}
static int dxva_Extract(vlc_va_dxva2_t *va, AVFrame *src)
{
    LPDIRECT3DSURFACE9 d3d = (LPDIRECT3DSURFACE9)(uintptr_t)src->data[3];
	if(va->output != MAKEFOURCC('Y','V','1','2'))
		return -1;

	D3DLOCKED_RECT lock;
	if (FAILED(IDirect3DSurface9_LockRect(d3d, &lock, NULL, D3DLOCK_READONLY))) {
		av_log(NULL,AV_LOG_ERROR, "Failed to lock surface");
		return -1;
	}

	if (va->render == MAKEFOURCC('Y','V','1','2') ||va->render == MAKEFOURCC('I','M','C','3'))
	{
		va->tmp_frame->width  =FFALIGN(va->width, 64);
		va->tmp_frame->height =FFALIGN(va->height, 2);
		va->tmp_frame->format = AV_PIX_FMT_YUV420P;

		int ret = av_frame_get_buffer(va->tmp_frame, 32);
		if (ret < 0)
			return ret;

		CopyFrameNV12((BYTE *)lock.pBits,va->tmp_frame->data[0],va->tmp_frame->data[1], va->surface_height, va->height,lock.Pitch);
		ret = av_frame_copy_props(va->tmp_frame, src);


		av_frame_unref(src);
		av_frame_move_ref(src, va->tmp_frame);
	}
	else
	{

		
		va->tmp_frame->width  =FFALIGN(va->width, 64);;
		va->tmp_frame->height =FFALIGN(va->height, 2);;
	    va->tmp_frame->format = AV_PIX_FMT_NV12;

		int ret = av_frame_get_buffer(va->tmp_frame, 32);
		if (ret < 0)
			return ret;

		CopyFrameNV12((BYTE *)lock.pBits,va->tmp_frame->data[0],va->tmp_frame->data[1], va->surface_height, va->height,lock.Pitch);
		ret = av_frame_copy_props(va->tmp_frame, src);
		

		//av_frame_unref(src);
		av_frame_move_ref(src, va->tmp_frame);
	}
	IDirect3DSurface9_UnlockRect(d3d);
	return 0;
}
int DxPictureCopy(struct AVCodecContext *avctx,AVFrame *src)
{
	vlc_va_dxva2_t *p_va = (vlc_va_dxva2_t *)avctx->opaque;

	return dxva_Extract(p_va,src);
}
//////////////////////////////////////////////////////////////////////////
UINT Ue(BYTE *pBuff, UINT nLen, UINT &nStartBit)
{
	//计算0bit的个数
	UINT nZeroNum = 0;
	while (nStartBit < nLen * 8)
	{
		if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:按位与，%取余
		{
			break;
		}
		nZeroNum++;
		nStartBit++;
	}
	nStartBit ++;

	//计算结果
	DWORD dwRet = 0;
	for (UINT i=0; i<nZeroNum; i++)
	{
		dwRet <<= 1;
		if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
		{
			dwRet += 1;
		}
		nStartBit++;
	}
	return (1 << nZeroNum) - 1 + dwRet;
}

int Se(BYTE *pBuff, UINT nLen, UINT &nStartBit)
{


	int UeVal=Ue(pBuff,nLen,nStartBit);
	double k=UeVal;
	int nValue=ceil(k/2);//ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00
	if (UeVal % 2==0)
		nValue=-nValue;
	return nValue;
}

DWORD u(UINT BitCount,BYTE * buf,UINT &nStartBit)
{
	DWORD dwRet = 0;
	for (UINT i=0; i<BitCount; i++)
	{
		dwRet <<= 1;
		if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
		{
			dwRet += 1;
		}
		nStartBit++;
	}
	return dwRet;
}

bool h264_decode_seq_parameter_set(BYTE * buf,UINT nLen,int &Width,int &Height)
{
	UINT StartBit=0; 
	buf = buf +4;
	int forbidden_zero_bit=u(1,buf,StartBit);
	int nal_ref_idc=u(2,buf,StartBit);
	int nal_unit_type=u(5,buf,StartBit);
	if(nal_unit_type==7)
	{
		int profile_idc=u(8,buf,StartBit);
		int constraint_set0_flag=u(1,buf,StartBit);//(buf[1] & 0x80)>>7;
		int constraint_set1_flag=u(1,buf,StartBit);//(buf[1] & 0x40)>>6;
		int constraint_set2_flag=u(1,buf,StartBit);//(buf[1] & 0x20)>>5;
		int constraint_set3_flag=u(1,buf,StartBit);//(buf[1] & 0x10)>>4;
		int reserved_zero_4bits=u(4,buf,StartBit);
		int level_idc=u(8,buf,StartBit);

		int seq_parameter_set_id=Ue(buf,nLen,StartBit);

		if( profile_idc == 100 || profile_idc == 110 ||
			profile_idc == 122 || profile_idc == 144 )
		{
			int chroma_format_idc=Ue(buf,nLen,StartBit);
			if( chroma_format_idc == 3 )
				int residual_colour_transform_flag=u(1,buf,StartBit);
			int bit_depth_luma_minus8=Ue(buf,nLen,StartBit);
			int bit_depth_chroma_minus8=Ue(buf,nLen,StartBit);
			int qpprime_y_zero_transform_bypass_flag=u(1,buf,StartBit);
			int seq_scaling_matrix_present_flag=u(1,buf,StartBit);

			int seq_scaling_list_present_flag[8];
			if( seq_scaling_matrix_present_flag )
			{
				for( int i = 0; i < 8; i++ ) {
					seq_scaling_list_present_flag[i]=u(1,buf,StartBit);
				}
			}
		}
		int log2_max_frame_num_minus4=Ue(buf,nLen,StartBit);
		int pic_order_cnt_type=Ue(buf,nLen,StartBit);
		if( pic_order_cnt_type == 0 )
			int log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,StartBit);
		else if( pic_order_cnt_type == 1 )
		{
			int delta_pic_order_always_zero_flag=u(1,buf,StartBit);
			int offset_for_non_ref_pic=Se(buf,nLen,StartBit);
			int offset_for_top_to_bottom_field=Se(buf,nLen,StartBit);
			int num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,StartBit);

			int *offset_for_ref_frame=new int[num_ref_frames_in_pic_order_cnt_cycle];
			for( int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
				offset_for_ref_frame[i]=Se(buf,nLen,StartBit);
			delete [] offset_for_ref_frame;
		}
		int num_ref_frames=Ue(buf,nLen,StartBit);
		int gaps_in_frame_num_value_allowed_flag=u(1,buf,StartBit);
		int pic_width_in_mbs_minus1=Ue(buf,nLen,StartBit);
		int pic_height_in_map_units_minus1=Ue(buf,nLen,StartBit);

		Width=(pic_width_in_mbs_minus1+1)*16;
		Height=(pic_height_in_map_units_minus1+1)*16;

		return true;
	}
	else
		return false;
}


int BindDxva2Module(	AVCodecContext  *pCodecCtx)
{
	vlc_va_dxva2_t *dxva = vlc_va_NewDxva2(pCodecCtx->codec_id);
	if(NULL == dxva)
		return -1;
	
	int res = Setup(dxva, &pCodecCtx->hwaccel_context, &pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	if (res < 0) 
		return res;

	
	pCodecCtx->opaque = dxva;
	pCodecCtx->get_format = ffmpeg_GetFormat;
	
	

	pCodecCtx->get_buffer2 = ffmpeg_GetFrameBuf;
	//pCodecCtx->reget_buffer2 = ffmpeg_ReGetFrameBuf;
	//pCodecCtx->release_buffer2 = ffmpeg_ReleaseFrameBuf;
	pCodecCtx->thread_count = 1;

	pCodecCtx->slice_flags    |= SLICE_FLAG_ALLOW_FIELD;
	return 0;
}