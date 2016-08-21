#ifndef Dxva2Def_H
#define Dxva2Def_H
#define COBJMACROS
#include <windows.h>
#include <d3d9.h>
#include <dxva2api.h>
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/dxva2.h"
#include "libswscale/swscale.h"
}
#define VA_DXVA2_MAX_SURFACE_COUNT (64)

#define H264_NONE   -1
#define H264_STP_A	0 /*STP_A÷°*/
#define H264_I		1 /*∆’Õ®I÷°*/

typedef struct {
	LPDIRECT3DSURFACE9 d3d;
	int                refcount;
	unsigned int       order;
} vlc_va_surface_t;

typedef struct
{
	int          codec_id;
	int          width;
	int          height;
    int          vendorId;
	/* DLL */
	HINSTANCE             hd3d9_dll;
	HINSTANCE             hdxva2_dll;

	/* Direct3D */
	D3DPRESENT_PARAMETERS  d3dpp;
	LPDIRECT3D9            d3dobj;
	D3DADAPTER_IDENTIFIER9 d3dai;
	LPDIRECT3DDEVICE9      d3ddev;

	/* Device manager */
	UINT                     token;
	IDirect3DDeviceManager9  *devmng;
	HANDLE                   device;

	/* Video service */
	IDirectXVideoDecoderService  *vs;
	GUID                         input;
	D3DFORMAT                    render;

	/* Video decoder */
	DXVA2_ConfigPictureDecode    cfg;
	IDirectXVideoDecoder         *decoder;

	/* Option conversion */
	D3DFORMAT                    output;

	/* */
	struct dxva_context hw;

	/* */
	unsigned     surface_count;
	unsigned     surface_order;
	int          surface_width;
	int          surface_height;

	vlc_va_surface_t surface[VA_DXVA2_MAX_SURFACE_COUNT];
	LPDIRECT3DSURFACE9 hw_surface[VA_DXVA2_MAX_SURFACE_COUNT];
	AVFrame * tmp_frame;
} vlc_va_dxva2_t;
typedef struct {
	const char   *name;
	D3DFORMAT    format;
	AVPixelFormat  codec;
} d3d_format_t;
/* XXX Prefered format must come first */
static const d3d_format_t d3d_formats[] = {
	{ "YV12",   (D3DFORMAT)MAKEFOURCC('Y','V','1','2'),    AV_PIX_FMT_YUV420P },
	{ "NV12",   (D3DFORMAT)MAKEFOURCC('N','V','1','2'),    AV_PIX_FMT_NV12 },

	{ NULL, (D3DFORMAT)0, AV_PIX_FMT_NONE }
};
/**
 * video format description
 */
struct video_format_t
{
    AVPixelFormat  i_chroma;                               /**< picture chroma */
 
    unsigned int i_width;                                 /**< picture width */
    unsigned int i_height;                               /**< picture height */
    unsigned int i_x_offset;               /**< start offset of visible area */
    unsigned int i_y_offset;               /**< start offset of visible area */
    unsigned int i_visible_width;                 /**< width of visible area */
    unsigned int i_visible_height;               /**< height of visible area */
 
    unsigned int i_bits_per_pixel;             /**< number of bits per pixel */
 
    unsigned int i_sar_num;                   /**< sample/pixel aspect ratio */
    unsigned int i_sar_den;
 
    unsigned int i_frame_rate;                     /**< frame rate numerator */
    unsigned int i_frame_rate_base;              /**< frame rate denominator */
 
    uint32_t i_rmask, i_gmask, i_bmask;          /**< color masks for RGB chroma */
    int i_rrshift, i_lrshift;
    int i_rgshift, i_lgshift;
    int i_rbshift, i_lbshift;
};

static const GUID DXVA2_ModeMPEG2_MoComp = {
	0xe6a9f44b, 0x61b0,0x4563, {0x9e,0xa4,0x63,0xd2,0xa3,0xc6,0xfe,0x66}
};
static const GUID DXVA2_ModeMPEG2_IDCT = {
	0xbf22ad00, 0x03ea,0x4690, {0x80,0x77,0x47,0x33,0x46,0x20,0x9b,0x7e}
};
static const GUID DXVA2_ModeMPEG2_VLD = {
	0xee27417f, 0x5e28,0x4e65, {0xbe,0xea,0x1d,0x26,0xb5,0x08,0xad,0xc9}
};
static const GUID DXVA2_ModeMPEG2and1_VLD = {
	0x86695f12, 0x340e,0x4f04, {0x9f,0xd3,0x92,0x53,0xdd,0x32,0x74,0x60}
};
static const GUID DXVA2_ModeMPEG1_VLD = {
	0x6f3ec719, 0x3735,0x42cc, {0x80,0x63,0x65,0xcc,0x3c,0xb3,0x66,0x16}
};

static const GUID DXVA2_ModeH264_A = {
	0x1b81be64, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeH264_B = {
	0x1b81be65, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeH264_C = {
	0x1b81be66, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeH264_D = {
	0x1b81be67, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeH264_E = {
	0x1b81be68, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeH264_F = {
	0x1b81be69, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA_ModeH264_VLD_WithFMOASO_NoFGT = {
	0xd5f04ff9, 0x3418,0x45d8, {0x95,0x61,0x32,0xa7,0x6a,0xae,0x2d,0xdd}
};
static const GUID DXVADDI_Intel_ModeH264_A = {
	0x604F8E64, 0x4951,0x4c54, {0x88,0xFE,0xAB,0xD2,0x5C,0x15,0xB3,0xD6}
};
static const GUID DXVADDI_Intel_ModeH264_C = {
	0x604F8E66, 0x4951,0x4c54, {0x88,0xFE,0xAB,0xD2,0x5C,0x15,0xB3,0xD6}
};
static const GUID DXVADDI_Intel_ModeH264_E = { // DXVA_Intel_H264_ClearVideo
	0x604F8E68, 0x4951,0x4c54, {0x88,0xFE,0xAB,0xD2,0x5C,0x15,0xB3,0xD6}
};
static const GUID DXVA2_ModeWMV8_A = {
	0x1b81be80, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeWMV8_B = {
	0x1b81be81, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeWMV9_A = {
	0x1b81be90, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeWMV9_B = {
	0x1b81be91, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeWMV9_C = {
	0x1b81be94, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};

static const GUID DXVA2_ModeVC1_A = {
	0x1b81beA0, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeVC1_B = {
	0x1b81beA1, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeVC1_C = {
	0x1b81beA2, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
static const GUID DXVA2_ModeVC1_D = {
	0x1b81beA3, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};
/* Conformity to the August 2010 update of the specification, ModeVC1_VLD2010 */
static const GUID DXVA2_ModeVC1_D2010 = {
	0x1b81beA4, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};

static const GUID DXVA_NoEncrypt = {
	0x1b81bed0, 0xa0c7,0x11d3, {0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5}
};

static const GUID DXVA_Intel_VC1_ClearVideo = {
	0xBCC5DB6D, 0xA2B6,0x4AF0, {0xAC,0xE4,0xAD,0xB1,0xF7,0x87,0xBC,0x89}
};

static const GUID DXVA_nVidia_MPEG4_ASP = {
	0x9947EC6F, 0x689B,0x11DC, {0xA3,0x20,0x00,0x19,0xDB,0xBC,0x41,0x84}
};
static const GUID DXVA_ModeMPEG4pt2_VLD_Simple = {
	0xefd64d74, 0xc9e8,0x41d7, {0xa5,0xe9,0xe9,0xb0,0xe3,0x9f,0xa3,0x19}
};
static const GUID DXVA_ModeMPEG4pt2_VLD_AdvSimple_NoGMC = {
	0xed418a9f, 0x10d,0x4eda,  {0x9a,0xe3,0x9a,0x65,0x35,0x8d,0x8d,0x2e}
};
static const GUID DXVA_ModeMPEG4pt2_VLD_AdvSimple_GMC = {
	0xab998b5b, 0x4258,0x44a9, {0x9f,0xeb,0x94,0xe5,0x97,0xa6,0xba,0xae}
};


static const GUID IID_IDirectXVideoDecoderService = {
	0xfc51a551, 0xd5e7, 0x11d9, {0xaf,0x55,0x00,0x05,0x4e,0x43,0xff,0x02}
};
static const GUID IID_IDirectXVideoAccelerationService = {
	0xfc51a550, 0xd5e7, 0x11d9, {0xaf,0x55,0x00,0x05,0x4e,0x43,0xff,0x02}
};
typedef struct {
	const char   *name;
	const GUID&  guid;
	int          codec;
} dxva2_mode_t;

/* XXX Prefered modes must come first */
static const dxva2_mode_t dxva2_modes[] = {
	{ "MPEG-2 variable-length decoder",            DXVA2_ModeMPEG2_VLD,     AV_CODEC_ID_MPEG2VIDEO },
	{ "MPEG-2 & MPEG-1 variable-length decoder",   DXVA2_ModeMPEG2and1_VLD, AV_CODEC_ID_MPEG2VIDEO },
	{ "MPEG-2 motion compensation",                DXVA2_ModeMPEG2_MoComp,  0 },
	{ "MPEG-2 inverse discrete cosine transform",  DXVA2_ModeMPEG2_IDCT,    0 },

	{ "MPEG-1 variable-length decoder",            DXVA2_ModeMPEG1_VLD,     0 },

	{ "H.264 variable-length decoder, film grain technology",                      DXVA2_ModeH264_F,                   AV_CODEC_ID_H264 },
	{ "H.264 variable-length decoder, no film grain technology",                   DXVA2_ModeH264_E,                   AV_CODEC_ID_H264 },
	{ "H.264 variable-length decoder, no film grain technology (Intel ClearVideo)",DXVADDI_Intel_ModeH264_E,           AV_CODEC_ID_H264 },
	{ "H.264 variable-length decoder, no film grain technology, FMO/ASO",          DXVA_ModeH264_VLD_WithFMOASO_NoFGT, AV_CODEC_ID_H264 },
	{ "H.264 inverse discrete cosine transform, film grain technology",            DXVA2_ModeH264_D,                   0             },
	{ "H.264 inverse discrete cosine transform, no film grain technology",         DXVA2_ModeH264_C,                   0             },
	{ "H.264 inverse discrete cosine transform, no film grain technology (Intel)", DXVADDI_Intel_ModeH264_C,           0             },
	{ "H.264 motion compensation, film grain technology",                          DXVA2_ModeH264_B,                   0             },
	{ "H.264 motion compensation, no film grain technology",                       DXVA2_ModeH264_A,                   0             },
	{ "H.264 motion compensation, no film grain technology (Intel)",               DXVADDI_Intel_ModeH264_A,           0             },

	{ "Windows Media Video 8 motion compensation", DXVA2_ModeWMV8_B, 0 },
	{ "Windows Media Video 8 post processing",     DXVA2_ModeWMV8_A, 0 },

	{ "Windows Media Video 9 IDCT",                DXVA2_ModeWMV9_C, 0 },
	{ "Windows Media Video 9 motion compensation", DXVA2_ModeWMV9_B, 0 },
	{ "Windows Media Video 9 post processing",     DXVA2_ModeWMV9_A, 0 },

	{ "VC-1 variable-length decoder",              DXVA2_ModeVC1_D, AV_CODEC_ID_VC1 },
	{ "VC-1 variable-length decoder",              DXVA2_ModeVC1_D, AV_CODEC_ID_WMV3 },
	{ "VC-1 variable-length decoder",              DXVA2_ModeVC1_D2010, AV_CODEC_ID_VC1 },
	{ "VC-1 variable-length decoder",              DXVA2_ModeVC1_D2010, AV_CODEC_ID_WMV3 },
	{ "VC-1 inverse discrete cosine transform",    DXVA2_ModeVC1_C, 0 },
	{ "VC-1 motion compensation",                  DXVA2_ModeVC1_B, 0 },
	{ "VC-1 post processing",                      DXVA2_ModeVC1_A, 0 },

	{ "VC-1 variable-length decoder (Intel)",      DXVA_Intel_VC1_ClearVideo, 0 },

	{ "MPEG-4 Part 2 nVidia bitstream decoder",                                                         DXVA_nVidia_MPEG4_ASP,                 0 },
	{ "MPEG-4 Part 2 variable-length decoder, Simple Profile",                                          DXVA_ModeMPEG4pt2_VLD_Simple,          0 },
	{ "MPEG-4 Part 2 variable-length decoder, Simple&Advanced Profile, no global motion compensation",  DXVA_ModeMPEG4pt2_VLD_AdvSimple_NoGMC, 0 },
	{ "MPEG-4 Part 2 variable-length decoder, Simple&Advanced Profile, global motion compensation",     DXVA_ModeMPEG4pt2_VLD_AdvSimple_GMC,   0 },

	{ NULL, GUID_NULL, 0 }
};

/** Timeout parameter passed to DtsProcOutput() in us */
#define OUTPUT_PROC_TIMEOUT 50
/** Step between fake timestamps passed to hardware in units of 100ns */
#define TIMESTAMP_UNIT 100000
/** Initial value in us of the wait in decode() */
#define BASE_WAIT 10000
/** Increment in us to adjust wait in decode() */
#define WAIT_UNIT 1000


/*****************************************************************************
 * Module private data
 ****************************************************************************/

typedef enum {
    RET_ERROR           = -1,
    RET_OK              = 0,
    RET_COPY_AGAIN      = 1,
    RET_SKIP_NEXT_COPY  = 2,
    RET_COPY_NEXT_FIELD = 3,
} CopyRet;

typedef struct OpaqueList {
    struct OpaqueList *next;
    uint64_t fake_timestamp;
    uint64_t reordered_opaque;
    uint8_t pic_type;
} OpaqueList;

typedef struct {
    AVClass *av_class;
    AVCodecContext *avctx;
    AVFrame pic;
    HANDLE dev;

    uint8_t *orig_extradata;
    uint32_t orig_extradata_size;

    AVBitStreamFilterContext *bsfc;
    AVCodecParserContext *parser;

    uint8_t is_70012;
    uint8_t *sps_pps_buf;
    uint32_t sps_pps_size;
    uint8_t is_nal;
    uint8_t output_ready;
    uint8_t need_second_field;
    uint8_t skip_next_output;
    uint64_t decode_wait;

    uint64_t last_picture;

    OpaqueList *head;
    OpaqueList *tail;

    /* Options */
    uint32_t sWidth;
    uint8_t bframe_bug;
} CHDContext;

static int D3dCreateDevice(vlc_va_dxva2_t *);
static void D3dDestroyDevice(vlc_va_dxva2_t *);
//static char *DxDescribe(vlc_va_dxva2_t *);

static int D3dCreateDeviceManager(vlc_va_dxva2_t *);
static void D3dDestroyDeviceManager(vlc_va_dxva2_t *);

static int DxCreateVideoService(vlc_va_dxva2_t *);
static void DxDestroyVideoService(vlc_va_dxva2_t *);
static int DxFindVideoServiceConversion(vlc_va_dxva2_t *, GUID *input, D3DFORMAT *output);

static int DxCreateVideoDecoder(vlc_va_dxva2_t *, int codec_id, const video_format_t *fmt);
static void DxDestroyVideoDecoder(vlc_va_dxva2_t *);
static int DxResetVideoDecoder(vlc_va_dxva2_t *);

static void DxCreateVideoConversion(vlc_va_dxva2_t *);
static void DxDestroyVideoConversion(vlc_va_dxva2_t *);


#endif