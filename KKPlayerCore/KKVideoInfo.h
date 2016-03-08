/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "Includeffmpeg.h"
#include "KKLock.h"
#include "IKKAudio.h"
#include <queue>
#ifndef KKVideoInfo_H_
#define KKVideoInfo_H_

#define SAMPLE_ARRAY_SIZE (8 * 65536)
#define KK_AUDIO_MIN_BUFFER_SIZE 512
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE  FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))
#define INT64_MAX 0x7fffffffffffffffLL 
#define INT64_MIN (-0x7fffffffffffffffLL-1)
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
	HANDLE ThreadHandel;
	UINT Addr;
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
	HANDLE m_WaitEvent;
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
	int serial;
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame 持续的时间*/ 
	int64_t pos;          /* byte position of the frame in the input file */
	/***********位图数据*************/
	void *buffer;
	/***是否分贝***/
	int allocated;
	int reallocate;
	int width;
	int height;
	AVRational sar;
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
	HANDLE m_WaitEvent;
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
	int pkt_serial;
	int finished;
	int packet_pending;	
	
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
    //视频已经准备好了
	int IsReady;
	//读线程信息
	SKK_ThreadInfo read_tid;
	AVInputFormat *iformat;
	/********1 中断 ************/
	int abort_request;
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
	int audio_clock_serial;
	/*********音频时间**************/
	int64_t audio_callback_time;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_avg_coef;
	double audio_diff_threshold;
	int audio_diff_avg_count;
	IKKAudio *pKKAudio;

    /****音频过滤***/
	AVFilterGraph *graph;
	AVFilterContext *src, *sink;
	/***************************/
	//原音频流
	AVStream *audio_st;
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
	int DestWidth, DestHeight, xleft, ytop;
	//显示区域宽度，高度
	int DisplayWidth,DisplayHeight;
	int step;

	
	AVPacket *pflush_pkt;
	int last_video_stream, last_audio_stream, last_subtitle_stream;

	double remaining_time;
	void *pScreen;

	double last_duration;
	/****当前帧直接的延迟s****/
	double delay;

	/**********输出环境,单独存储文件*****************/
	bool IsOutFile; 
	AVFormatContext *ofmt_ctx;  
	AVOutputFormat *ofmt;
	int videoindex_out;  
	int audioindex_out;  
	AVStream *out_audios;
	AVStream *out_videos;
	
	/****************推流环境************************/
    AVFormatContext *PushOfmt_ctx;
	char PushURL[1024];
	int m_nLiveType;//0:不退流; 1:rtmp 推流

	/****************以下windows专用************************/
	struct SwsContext *PicGrab_Img_convert_ctx;
	HDC Source_Hdc;
	
	int DevBpp;
	HDC Dest_Hdc;
	
#ifdef WIN32
	/******截屏数据类型*******/
	int PicGrabType;
	void *PicGrabBuf;
	RECT virtual_rect;
	BITMAPINFO bmi;
	HBITMAP hbmp;
	BITMAP bmp;
#endif
	int frame_size;
	int header_size;
    

	/**********测试用编码成h264************/
	AVCodec *pH264Codec;  
	AVCodecContext *pH264CodecCtx;  
	std::queue<AVPacket*> pH264Pkt;
	struct SwsContext *H264_Img_convert_ctx;
	FILE* fp_out;
} SKK_VideoState;
#endif