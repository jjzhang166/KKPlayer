#include "mfx_utils.h"

#include <math.h>
mfxStatus ConvertFrameRate(mfxF64 dFrameRate, mfxU32* pnFrameRateExtN, mfxU32* pnFrameRateExtD)
{
	MSDK_CHECK_POINTER(pnFrameRateExtN, MFX_ERR_NULL_PTR);
	MSDK_CHECK_POINTER(pnFrameRateExtD, MFX_ERR_NULL_PTR);

	mfxU32 fr;

	fr = (mfxU32)(dFrameRate + .5);

	if (fabs(fr - dFrameRate) < 0.0001)
	{
		*pnFrameRateExtN = fr;
		*pnFrameRateExtD = 1;
		return MFX_ERR_NONE;
	}

	fr = (mfxU32)(dFrameRate * 1.001 + .5);

	if (fabs(fr * 1000 - dFrameRate * 1001) < 10)
	{
		*pnFrameRateExtN = fr * 1000;
		*pnFrameRateExtD = 1001;
		return MFX_ERR_NONE;
	}

	*pnFrameRateExtN = (mfxU32)(dFrameRate * 10000 + .5);
	*pnFrameRateExtD = 10000;

	return MFX_ERR_NONE;
}

mfxF64 CalculateFrameRate(mfxU32 nFrameRateExtN, mfxU32 nFrameRateExtD)
{
	if (nFrameRateExtN && nFrameRateExtD)
		return (mfxF64)nFrameRateExtN / nFrameRateExtD;
	else
		return 0;
}

mfxU16 GetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize)
{
	if (pSurfacesPool)
	{
		for (mfxU16 i = 0; i < nPoolSize; i++)
		{
			if (0 == pSurfacesPool[i].Data.Locked)
			{
				return i;
			}
		}
	}

	return MSDK_INVALID_SURF_IDX;
}

mfxU16 GetFreeSurface(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize)
{
	mfxU32 SleepInterval = 10; // milliseconds

	mfxU16 idx = MSDK_INVALID_SURF_IDX;

	//wait if there's no free surface
	for (mfxU32 i = 0; i < MSDK_WAIT_INTERVAL; i += SleepInterval)
	{
		idx = GetFreeSurfaceIndex(pSurfacesPool, nPoolSize);

		if (MSDK_INVALID_SURF_IDX != idx)
		{
			break;
		}
		else
		{
			MSDK_SLEEP(SleepInterval);
		}
	}

	return idx;
}

mfxU16 GetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize, mfxU16 step)
{
	if (pSurfacesPool)
	{
		for (mfxU16 i = 0; i < nPoolSize; i = (mfxU16)(i + step), pSurfacesPool += step)
		{
			if (0 == pSurfacesPool[0].Data.Locked)
			{
				return i;
			}
		}
	}

	return MSDK_INVALID_SURF_IDX;
}

mfxStatus InitMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize)
{
	//check input params
	MSDK_CHECK_POINTER(pBitstream, MFX_ERR_NULL_PTR);
	MSDK_CHECK_ERROR(nSize, 0, MFX_ERR_NOT_INITIALIZED);

	//prepare pBitstream
	WipeMfxBitstream(pBitstream);

	//prepare buffer
	pBitstream->Data = new mfxU8[nSize];
	MSDK_CHECK_POINTER(pBitstream->Data, MFX_ERR_MEMORY_ALLOC);

	pBitstream->MaxLength = nSize;

	return MFX_ERR_NONE;
}

mfxStatus ExtendMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize)
{
	MSDK_CHECK_POINTER(pBitstream, MFX_ERR_NULL_PTR);

	MSDK_CHECK_ERROR(nSize <= pBitstream->MaxLength, true, MFX_ERR_UNSUPPORTED);

	mfxU8* pData = new mfxU8[nSize];
	MSDK_CHECK_POINTER(pData, MFX_ERR_MEMORY_ALLOC);

	memmove(pData, pBitstream->Data + pBitstream->DataOffset, pBitstream->DataLength);

	WipeMfxBitstream(pBitstream);

	pBitstream->Data       = pData;
	pBitstream->DataOffset = 0;
	pBitstream->MaxLength  = nSize;

	return MFX_ERR_NONE;
}

void WipeMfxBitstream(mfxBitstream* pBitstream)
{
	MSDK_CHECK_POINTER(pBitstream);

	//free allocated memory
	MSDK_SAFE_DELETE_ARRAY(pBitstream->Data);
}

bool CheckVersion(mfxVersion* version, msdkAPIFeature feature)
{
	if (!version->Major || (version->Major > 1)) {
		return false;
	}

	switch (feature) {
	case MSDK_FEATURE_NONE:
		return true;
	case MSDK_FEATURE_MVC:
		if ((version->Major == 1) && (version->Minor >= 3)) {
			return true;
		}
		break;
	case MSDK_FEATURE_JPEG_DECODE:
		if ((version->Major == 1) && (version->Minor >= 3)) {
			return true;
		}
		break;
	case MSDK_FEATURE_LOW_LATENCY:
		if ((version->Major == 1) && (version->Minor >= 3)) {
			return true;
		}
		break;
	case MSDK_FEATURE_MVC_VIEWOUTPUT:
		if ((version->Major == 1) && (version->Minor >= 4)) {
			return true;
		}
		break;
	case MSDK_FEATURE_JPEG_ENCODE:
		if ((version->Major == 1) && (version->Minor >= 6)) {
			return true;
		}
		break;
	case MSDK_FEATURE_LOOK_AHEAD:
		if ((version->Major == 1) && (version->Minor >= 7)) {
			return true;
		}
		break;
	case MSDK_FEATURE_PLUGIN_API:
		if ((version->Major == 1) && (version->Minor >= 8)) {
			return true;
		}
		break;
	default:
		return false;
	}
	return false;
}
#include <sstream>
std::vector<mfxU8> InitSEIUserData(bool use_cbr, const mfxVideoParam& params, const mfxVersion& ver)
{
#define TO_STR(x) #x
	static const char *usage_str[] = {
		TO_STR(MFX_TARGETUSAGE_UNKNOWN),
		TO_STR(MFX_TARGETUSAGE_BEST_QUALITY),
		TO_STR(MFX_TARGETUSAGE_2),
		TO_STR(MFX_TARGETUSAGE_3),
		TO_STR(MFX_TARGETUSAGE_BALANCED),
		TO_STR(MFX_TARGETUSAGE_5),
		TO_STR(MFX_TARGETUSAGE_6),
		TO_STR(MFX_TARGETUSAGE_BEST_SPEED)
	};
#undef TO_STR
	using namespace std;
	vector<mfxU8> data;

	const mfxU8 UUID[] = { 0x6d, 0x1a, 0x26, 0xa0, 0xbd, 0xdc, 0x11, 0xe2,   //ISO-11578 UUID
		0x90, 0x24, 0x00, 0x50, 0xc2, 0x49, 0x00, 0x48 }; //6d1a26a0-bddc-11e2-9024-0050c2490048
	

#if _MSC_VER >1500
	data.insert(data.end(),begin(UUID),end(UID));
#else
	int count=sizeof(UUID);
	for(int i=0;i<count;i++){
		data.push_back(UUID[i]);
	}
#endif
	//data.insert(data.end(),UUID, );

	int method = params.mfx.RateControlMethod;

	ostringstream str;
	str << "QSV hardware encoder options:"
		<< " rate control: "        << "CBR";
	switch (method)
	{
	case MFX_RATECONTROL_CBR:
	case MFX_RATECONTROL_VBR:
	case MFX_RATECONTROL_VCM:
		str << "; target bitrate: "  << params.mfx.TargetKbps;
		if (method != MFX_RATECONTROL_CBR)
			str << "; max bitrate: " << params.mfx.MaxKbps;
		str << "; buffersize: "      << params.mfx.BufferSizeInKB * 8;
		break;

	case MFX_RATECONTROL_AVBR:
		str << "; target bitrate: "  << params.mfx.TargetKbps
			<< "; accuracy: "        << params.mfx.Accuracy
			<< "; convergence: "     << params.mfx.Convergence;
		break;

	case MFX_RATECONTROL_CQP:
		str << "; QPI: "             << params.mfx.QPI
			<< "; QPP: "             << params.mfx.QPP
			<< "; QPB: "             << params.mfx.QPB;
		break;
	}
	str << "; API level: "           << ver.Major << "." << ver.Minor
		<< "; Target Usage: "        << usage_str[params.mfx.TargetUsage];

	string str_(str.str());

#if _MSC_VER >1500
	data.insert(end(data), begin(str_),end(str_));
#else
	data.insert(data.end(), str_.begin(),str_.end());
#endif

	return data;
}


void GenSEIData(mfxPayload& sei_payload,const mfxVideoParam& params, const mfxVersion& ver)
{
	std::vector<mfxU8> data=InitSEIUserData(true,params,ver);
	unsigned payload_size = data.size();
	std::vector<mfxU8> buffer;
	mfxU16 type=5;
	mfxU16 type_ = type;
	while(type_ > 255)
	{
#if _MSC_VER >1500
		buffer.emplace_back(0xff);
#else
		buffer.push_back(0xff);
#endif
		type_ -= 255;
	}
#if _MSC_VER >1500
	buffer.emplace_back((mfxU8)type_);
#else
	buffer.push_back((mfxU8)type_);
#endif
	while(payload_size > 255)
	{
#if _MSC_VER >1500
		buffer.emplace_back(0xff);
#else
		buffer.push_back(0xff);
#endif
		
		payload_size -= 255;
	}
#if _MSC_VER >1500
	buffer.emplace_back(payload_size);
#else
	buffer.push_back(payload_size);
#endif

#if _MSC_VER >1500
	buffer.insert(end(buffer), begin(data), end(data));
#else
	buffer.insert(buffer.end(),data.begin(),data.end());
#endif
	std::vector<std::vector<mfxU8>> data_buffers;

#if _MSC_VER >1500
	data_buffers.emplace_back(buffer);
#else
	data_buffers.push_back(buffer);
#endif
	//
	
	
	
	MSDK_ZERO_MEMORY(sei_payload);
	sei_payload.Type = type;
	sei_payload.BufSize = buffer.size();
	sei_payload.NumBit = sei_payload.BufSize*8;
	sei_payload.Data = &data_buffers.back().front();
	unsigned char* Bufs=(unsigned char*)::malloc(sei_payload.NumBit);
	memset( Bufs,0,sei_payload.NumBit);
	memcpy(Bufs,sei_payload.Data,sei_payload.BufSize);
	sei_payload.Data =Bufs;

}