/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
//#define __STDC_HOSTED__ 0
#include "stdafx.h"
#ifndef Includeffmpeg_H_
#define Includeffmpeg_H_
#ifdef  WIN32
#include <float.h>
#endif
//typedef unsigned char	Uint8;
extern "C"
{
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#define UINT64_C(x)  (x ## ULL)
//#include <stdint.h>
#include "libavutil/avutil.h"
#include "libavutil/avstring.h"
#include "libavutil/mem.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavutil/base64.h"
#include "libavutil/mathematics.h"
#include "libavutil/aes.h"
#include "libavutil/lfg.h"
#include "libavutil/log.h"
#include "libavutil/opt.h"



#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavdevice/avdevice.h"

#include "libswscale/swscale.h"

#include "libavcodec/avfft.h"
#include "libavcodec/avcodec.h"

#include "libswresample/swresample.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"




		typedef union {
			uint64_t u64[2];
			uint32_t u32[4];
			uint8_t u8x4[4][4];
			uint8_t u8[16];
		} av_aes_block;

		typedef struct AVAES {
			// Note: round_key[16] is accessed in the init code, but this only
			// overwrites state, which does not matter (see also commit ba554c0).
			DECLARE_ALIGNED(16, av_aes_block, round_key)[15];
			DECLARE_ALIGNED(16, av_aes_block, state)[2];
			int rounds;
			void (*crypt)(struct AVAES *a, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int rounds);
		} AVAES;

#ifdef WIN32
	#ifdef _DEBUG
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\avcodec.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\avdevice.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\avfilter.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\avformat.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\avutil.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\postproc.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\swresample.lib")
		#pragma comment (lib,"..\\libx86\\ffmpeg\\lib\\swscale.lib")
	#else
		#pragma comment (lib,"..\\libx86\\ffmpeg\\vs2008lib\\avcodec-56.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\avdevice-56.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\avfilter-5.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\avformat-56.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\avutil-54.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\postproc-53.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\swresample-1.lib")
		#pragma comment (lib,"../libx86/ffmpeg/vs2008lib\\swscale-3.lib")
	#endif
#endif
}


#define MAX_QUEUE_SIZE (1024 * 1024*8)  //1048576*8  * 8  8M
#define MIN_FRAMES 5

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512*8
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10
#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000

#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
#define AV_NOSYNC_THRESHOLD 10.0
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1


#endif

#ifndef Nan_h__
#define Nan_h__


//#define NAN log(-1.0) //无效值NaN
#define isNAN(x) ((x)!=(x))
#define NINF log(0.0) //负无穷大
#define INF -NINF //无穷大
#define PINF -NINF //正无穷大
#define isINF(x) (((x)==PINF)||((x)==NINF))
#define isPINF(x) ((x)==PINF)
#define isNINF(x) ((x)==NINF)
#endif // Nan_h__

//AV_OPT_TYPE_BOOL