#ifndef mfx_utils_H_
#define mfx_utils_H_
#include "mfx_defs.h"
#include "mfxdefs.h"
#include "mfxvideo++.h"
#include "mfxvideo.h"
#include <vector>
mfxStatus ConvertFrameRate(mfxF64 dFrameRate, mfxU32* pnFrameRateExtN, mfxU32* pnFrameRateExtD);
mfxU16 GetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize);
mfxU16 GetFreeSurface(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize);
void WipeMfxBitstream(mfxBitstream* pBitstream);
mfxStatus ExtendMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize);

enum msdkAPIFeature {
	MSDK_FEATURE_NONE,
	MSDK_FEATURE_MVC,
	MSDK_FEATURE_JPEG_DECODE,
	MSDK_FEATURE_LOW_LATENCY,
	MSDK_FEATURE_MVC_VIEWOUTPUT,
	MSDK_FEATURE_JPEG_ENCODE,
	MSDK_FEATURE_LOOK_AHEAD,
	MSDK_FEATURE_PLUGIN_API
};
bool CheckVersion(mfxVersion* version, msdkAPIFeature feature);
void GenSEIData(mfxPayload& sei_payload,const mfxVideoParam& params, const mfxVersion& ver);
#endif