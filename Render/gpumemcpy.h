#ifndef gpumemcpy_H_
#define gpumemcpy_H_
#include "stdafx.h"

void IniCopyFrameNV12(int m_dwVendorId,int cpu_flags);
void CopyFrameNV12(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch);
#endif