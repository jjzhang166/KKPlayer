/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "Includeffmpeg.h"
#include "KKLock.h"
#include "KKCond_t.h"
#include "IKKAudio.h"
#include "KKPlugin.h"
#include <queue>
#ifndef KKVideoInfo_H_
#define KKVideoInfo_H_

#define SAMPLE_ARRAY_SIZE (8 * 65536)
#define KK_AUDIO_MIN_BUFFER_SIZE 512
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 9
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE  FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))
#ifndef INT64_MAX
#define INT64_MAX 0x7fffffffffffffffLL 
#endif
#ifndef INT64_MIN
#define INT64_MIN (-0x7fffffffffffffffLL-1)
#endif
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0
/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

//AV包列表
typedef struct SKK_AVPacketList {
	AVPacket pkt;
	struct SKK_AVPacketList *next;
	int serial;
} SKK_AVPacketList;
/**********线程信息************/
typedef struct SKK_ThreadInfo
{
#ifdef WIN32_KK
	HANDLE ThreadHandel;
#else
	pthread_t Tid_task;
#endif
   UINT Addr;
   volatile	bool ThOver;
} SKK_ThreadInfo;

//包队列
typedef struct SKK_PacketQueue 
{
	SKK_AVPacketList *first_pkt, *last_pkt;/***********包首，包尾*************/
	/******包数******/
	int nb_packets;
	/***包大小***/
	int size;
	int abort_request;
	/*******序列号*********/
	int serial;
	CKKLock *pLock;
	//等待事件
	CKKCond_t* m_pWaitCond;
} SKK_PacketQueue;
//音频参数
typedef struct SKK_AudioParams 
{
	int freq;
	int channels;
	int64_t channel_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
} SKK_AudioParams;
//时钟
typedef struct SKK_Clock 
{
	double pts;           /* clock base */
	double pts_drift;     /* lock base minus time at which we updated the clock */
	double last_updated; /**上一次更新时间**/
	double speed;
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} SKK_Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
/********解码后的数据帧*******/
typedef struct SKK_Frame 
{
	AVFrame *frame;
	AVSubtitle sub;
	//序列号--刷新时更新。
	int serial;
	int PktNumber;/*****包序号，递增*****/
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame 这一帧持续的时间*/ 
	int64_t pos;          /* byte position of the frame in the input file */
	/***********位图数据*************/
	AVPicture Bmp;
	uint8_t *buffer;
	int dataLen;
	int buflen;

	/***是否分配内存对frame***/
	int allocated;
	CKKLock *BmpLock;
} SKK_Frame;

//帧队列(解码后的数据为帧)
typedef struct SKK_FrameQueue
{
	SKK_Frame queue[FRAME_QUEUE_SIZE];
	//读的位置
	int rindex;
	//写的位置
	int windex;
	int size;
	//queue最大大小
	int max_size;
	int keep_last;
	int rindex_shown;

	
	CKKLock *mutex;
	//等待事件
	CKKCond_t* m_pWaitCond;
	SKK_PacketQueue  *pktq;
	
} SKK_FrameQueue;


//同步方式
enum EKK_AV_SYNC
{
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};
//解码
typedef struct SKK_Decoder 
{

	AVPacket pkt;
	AVPacket pkt_temp;

	//与队列中的serial对应
	int pkt_serial;
	int finished;
	int packet_pending;	
	
	int Isflush;
	AVCodecContext *avctx;	
	/**线程信息**/
	SKK_ThreadInfo decoder_tid;
    //对应的队列
	SKK_PacketQueue* pQueue;
	int64_t start_pts;
	AVRational start_pts_tb;
	int64_t next_pts;
	AVRational next_pts_tb;

	//当前时间
	int64_t current_pts_time;
	int64_t pts;
	int64_t dts;
} SKK_Decoder;

//视频音频信息
typedef struct SKK_VideoState 
{

	int decoder_reorder_pts;
	/********包序号递增*********/
	int PktNumber;
	double duration;
    //视频已经准备好了
	int IsReady;
	//读线程信息
	SKK_ThreadInfo read_tid;
	AVInputFormat *iformat;
	/********1 中断 ************/
	volatile int abort_request;
	int force_refresh;
	/**********1暂停***************/
	int paused;
	int last_paused;
	int queue_attachments_req;
	/*********快进***************/
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	int64_t seek_rel;
	int read_pause_return;
	AVFormatContext *pFormatCtx;
	int realtime;

	SKK_Clock audclk;
	SKK_Clock vidclk;
	SKK_Clock extclk;

	/*******视频图片队列*******/
	SKK_FrameQueue pictq;
	/******字幕图片队列******/
	SKK_FrameQueue subpq;
	/******音频采样队列******/
	SKK_FrameQueue sampq;
    /******音频解码器*******/
	SKK_Decoder auddec;
	/******视频解码器*******/
	SKK_Decoder viddec;
	/******字幕解码器*******/
	SKK_Decoder subdec;

	/*****音频流Id*******/
	int audio_stream;

	/*******时中类型*********/
	int av_sync_type;

	double audio_clock;
	double Baseaudio_clock;
	int audio_clock_serial;
	/*********音频时间**************/
	int64_t audio_callback_time;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_avg_coef;
	double audio_diff_threshold;
	int audio_diff_avg_count;
	IKKAudio *pKKAudio;

	//视频过滤器
	int vfilter_idx;
	AVFilterContext *in_video_filter;   // the first filter in the video chain
	AVFilterContext *out_video_filter;  // the last filter in the video chain

    /****音频过滤***/
	AVFilterGraph *AudioGraph;
	AVFilterContext *InAudioSrc, *OutAudioSink;
    SKK_AudioParams audio_filter_src;
	char Atempo[50];
	//倍速
    int AVRate;//100正常速率
	int LastAVRate;//100正常速率




	/***************************/
	//原音频流
	AVStream *audio_st;
	//音频队列
	SKK_PacketQueue audioq;
	int audio_hw_buf_size;
	uint8_t silence_buf[KK_AUDIO_MIN_BUFFER_SIZE];
	uint8_t *audio_buf;
	uint8_t *audio_buf1;
	unsigned int audio_buf1_size;
	unsigned int audio_buf_size; /* in bytes */
	int audio_buf_index; /* in bytes */
	int audio_write_buf_size;
	//原音频参数
	struct SKK_AudioParams audio_src;
    //目标音频参数
	struct SKK_AudioParams audio_tgt;
	struct SwrContext *swr_ctx;
	int frame_drops_early;
	int frame_drops_late;

	enum ShowMode 
	{
		SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
	} show_mode;
	int16_t sample_array[SAMPLE_ARRAY_SIZE];
	int sample_array_index;
	int last_i_start;
	RDFTContext *rdft;
	int rdft_bits;
	FFTSample *rdft_data;
	int xpos;
	double last_vis_time;

	int subtitle_stream;
	AVStream *subtitle_st;
	SKK_PacketQueue subtitleq;

	double frame_timer;
	double frame_last_returned_time;
	double frame_last_filter_delay;
	int video_stream;
	//视频流
	AVStream *video_st;
	/******视频包队列******/
	SKK_PacketQueue videoq;
	double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity

	struct SwsContext *img_convert_ctx;
    struct SwsContext *sub_convert_ctx;
	int eof;
    /*****文件名******/
	char filename[1024];
	int fileSize;
	/*******视频大小信息********/
	int viddec_width,viddec_height;
	
	int step;

	
	AVPacket *pflush_pkt;
	int last_video_stream, last_audio_stream, last_subtitle_stream;

	double remaining_time;
	void *pScreen;

	double last_duration;
	/****当前帧直接的延迟s****/
	double delay;

	/***********插件信息*****************/
	KKPluginInfo *pKKPluginInfo;
} SKK_VideoState;
#endif