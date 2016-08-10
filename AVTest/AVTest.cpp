// ffmpegTest.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include "windows.h"
#include "time.h"

extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/mem.h>
#include <libavutil/avstring.h>
#include <libavutil/display.h>
#include <libavutil/eval.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>


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

#include "../KKPlayerCore/rtmp/AV_FLv.h"
CAV_Flv G_CAV_Flv;
static int autorotate = 1;
static const char **vfilters_list = NULL;
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

typedef struct KK_AVFilter
{	
    AVFormatContext *ic;
	
	AVStream *video_st;
	AVFilterGraph *VideoGraph;
	AVFilterContext *in_video_filter;   // the first filter in the video chain
	AVFilterContext *out_video_filter;  // the last filter in the video chain

	/****音频过滤***/
	AVFilterGraph *AudioGraph;
	AVFilterContext *InAudioSrc, *OutAudioSink;

	SKK_AudioParams audio_filter_src;
}KK_AVFilter;
#define MAX_BUF_SIZE 256*1024	

static int select_channel_layout(AVCodec *codec)
{
	const uint64_t *p;
	uint64_t best_ch_layout = 0;
	int best_nb_channels   = 0;

	if (!codec->channel_layouts)
		return AV_CH_LAYOUT_STEREO;

	p = codec->channel_layouts;
	while (*p) {
		int nb_channels = av_get_channel_layout_nb_channels(*p);

		if (nb_channels > best_nb_channels) {
			best_ch_layout    = *p;
			best_nb_channels = nb_channels;
		}
		p++;
	}
	return best_ch_layout;
}


static inline int64_t get_valid_channel_layout(int64_t channel_layout, int channels)
{
	if (channel_layout && av_get_channel_layout_nb_channels(channel_layout) == channels)
		return channel_layout;
	else
		return 0;
}
static inline int cmp_audio_fmts(enum AVSampleFormat fmt1, int64_t channel_count1,
								 enum AVSampleFormat fmt2, int64_t channel_count2)
{
	/* If channel count == 1, planar and non-planar formats are the same */
	if (channel_count1 == 1 && channel_count2 == 1)
	return av_get_packed_sample_fmt(fmt1) != av_get_packed_sample_fmt(fmt2);
	else
		return channel_count1 != channel_count2 || fmt1 != fmt2;
}
static int configure_audio_filters(KK_AVFilter  *is, const char *afilters, int force_output_format);
static int configure_video_filters(AVFilterGraph *graph, KK_AVFilter  *is, const char *vfilters, AVFrame *frame);
char *fiv="movie=\"/c/ff.png\"[wm];[in][wm]overlay=5:5[out]";
int main(int argc, char* argv[])
{
	//avdevice_register_all();
	av_register_all();
	avfilter_register_all();
	avformat_network_init();

	int ret=-1, i=0, videoindex=-1, audioindex=1, nComplete=0, len=0, frame_index=0;
	unsigned char *pEnCodeBuf = new unsigned char[MAX_BUF_SIZE];
	char *sourceFile = "c:\\jq.mp4";//"c:\\盗墓笔记11.mp4";
	char *destFile = "123.mp4";
	av_register_all();

	AVFormatContext *pInputFormatContext=NULL;
	AVCodec *pInputVideoCodec = NULL;
	AVCodec *pInputAudioCodec = NULL;
	AVCodecContext *pInputVideoCodecContext = NULL;
	AVCodecContext *pInputAudioCodecContext = NULL;
	AVStream *pVideo_Src=NULL;
	AVPacket InPack;
	KK_AVFilter kk_AVFilter;
	int videoWidth, videoHeight;

	AVFrame *OutVideoFrame;
	OutVideoFrame= avcodec_alloc_frame();  

	AVFrame *OutAudioFrame;
	OutAudioFrame = avcodec_alloc_frame();  

	memset(&kk_AVFilter,0,sizeof(kk_AVFilter));
	kk_AVFilter.ic=pInputFormatContext;

	kk_AVFilter.VideoGraph= avfilter_graph_alloc();
	if(avformat_open_input(&pInputFormatContext, sourceFile, NULL, NULL) !=0 )
	{
		//打开输入文件
		printf("can't open the file %s\n",sourceFile);
		exit(1);
	}

	if(avformat_find_stream_info(pInputFormatContext,NULL)<0)
	{
		//查找流信息
		printf("can't find suitable codec parameters\n");
	 
	}

	for(i=0; i<pInputFormatContext->nb_streams; i++)
	{
		if(pInputFormatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			videoindex=i;
			kk_AVFilter.video_st=pInputFormatContext->streams[i];
		}else if(pInputFormatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
		{
			audioindex=i;
		}
		
	}

	if(-1 == videoindex)
	{
		
	}

	pInputVideoCodecContext = pInputFormatContext->streams[videoindex]->codec;
	pInputVideoCodec = avcodec_find_decoder(pInputVideoCodecContext->codec_id);
	if(NULL == pInputVideoCodec)
	{
		//没有找到解码器
		
	}
	if(avcodec_open2(pInputVideoCodecContext, pInputVideoCodec,NULL) != 0)
	{
		//打开解码器失败
		
	}

	pInputAudioCodecContext = pInputFormatContext->streams[audioindex]->codec;
	pInputAudioCodec = avcodec_find_decoder(pInputAudioCodecContext->codec_id);
	if(NULL == pInputAudioCodec)
	{
		//没有找到解码器

	}
	if(avcodec_open2(pInputAudioCodecContext, pInputAudioCodec,NULL) != 0)
	{
		//打开解码器失败

	}

		AVCodec *codec;
		AVCodecContext *CtxH264= NULL;
		AVCodecContext *CtxAAC= NULL;
		AVFrame *Audio_frame=NULL;
		AVDictionary *ParamH264=NULL;
		int size =0,got_picture = 0;
	
		//编码时间搓
		int64_t cur_pts_v=0,cur_pts_a=0;
		
		codec= avcodec_find_encoder(AV_CODEC_ID_AAC);
		CtxAAC =avcodec_alloc_context3(codec);
	
		
		CtxAAC->bit_rate=pInputAudioCodecContext->bit_rate;
		CtxAAC->sample_fmt=AV_SAMPLE_FMT_S16;
	
		
		
		CtxAAC->sample_rate    = pInputAudioCodecContext->sample_rate;
		CtxAAC->channel_layout =AV_CH_LAYOUT_STEREO;select_channel_layout(codec);// pInputAudioCodecContext->channel_layout;
		CtxAAC->channels       = av_get_channel_layout_nb_channels(CtxAAC->channel_layout);
	
		/* open it */
		if (avcodec_open2(CtxAAC, codec, NULL) < 0) {
			fprintf(stderr, "Could not open codec\n");
			exit(1);
		}
	
	
		codec= avcodec_find_encoder(AV_CODEC_ID_H264);
		//codec= avcodec_find_encoder(AV_CODEC_ID_MPEG4);
		CtxH264 =avcodec_alloc_context3(codec);
	
	
		
		CtxH264->width=pInputVideoCodecContext->width;
		CtxH264->height=pInputVideoCodecContext->height;
		
		
		Audio_frame=av_frame_alloc();
		
	
		{
			CtxH264->me_range=8;
			//，QP取最大值51时，表示量化是最粗糙的。0-51
			CtxH264->max_qdiff=30;
			/******影响图像质量******/
			CtxH264->qmin=28;
			CtxH264->qmax=40;
	
			//向前预测帧
			CtxH264->pre_me=0;//pInputVideoCodecContext->pre_me;
			CtxH264->rc_eq="tex";
	
			//gop_size 是指一个GOP包含多少帧，-g 500就是每500帧一个I帧
			CtxH264->gop_size=pInputVideoCodecContext->gop_size;
			//量化器压缩比率0-1 ，越小，文件越小
			CtxH264->qcompress=0.5;
	
			
			CtxH264->bit_rate = pInputVideoCodecContext->bit_rate;
			//
			CtxH264->time_base=pInputVideoCodecContext->time_base;

			CtxH264->max_qdiff=3;
			CtxH264->me_method=5;
	
		}
		CtxH264->pix_fmt=pInputVideoCodecContext->pix_fmt;
			if(avcodec_open2(CtxH264, codec,&ParamH264) < 0) {
				fprintf(stderr, "Could not open codec\n");
				exit(1);
			}

	AVPacket OutVideoPack;
	av_init_packet(&OutVideoPack);

	AVPacket OutAudioPack;
	av_init_packet(&OutAudioPack);

	int64_t lsatpts=0;
	struct SwrContext *swr_ctx=NULL;
	unsigned int data_size = 0;
	uint8_t *audio_buf1=NULL;//(uint8_t *)av_malloc(data_size);
	
	int last_w = 0;
	int last_h = 0;
	enum AVPixelFormat last_format =(AVPixelFormat) -2;
	int last_serial = -1;
	int last_vfilter_idx = 0;

	while(av_read_frame(pInputFormatContext, &InPack) >= 0)
	{
		if(InPack.stream_index ==videoindex)
		{
				len = avcodec_decode_video2(pInputVideoCodecContext, OutVideoFrame, &nComplete, &InPack);
				OutVideoFrame->format=pInputVideoCodecContext->pix_fmt;
				if(nComplete > 0)
				{
					if (   last_w != OutVideoFrame->width
						|| last_h != OutVideoFrame->height
						|| last_format != OutVideoFrame->format
						) 
					{
							
							avfilter_graph_free(&kk_AVFilter.VideoGraph);
							kk_AVFilter.VideoGraph = avfilter_graph_alloc();

							if ((ret = configure_video_filters(kk_AVFilter.VideoGraph, &kk_AVFilter,fiv , OutVideoFrame)) < 0) {
								int xx=0;
								xx++;
							}
						
							last_w = OutVideoFrame->width;
							last_h = OutVideoFrame->height;
						
					}

					ret = av_buffersrc_add_frame(kk_AVFilter.in_video_filter,OutVideoFrame);
					if (ret < 0)
					{
						int xx=0;
						xx++;
					}

					while (ret >= 0) 
					{
                           ret = av_buffersink_get_frame_flags(kk_AVFilter.out_video_filter, OutVideoFrame, 0);
						   if (ret < 0) {
							   ret = 0;
							   break;
						   }
						   //找到pts
						   if((OutVideoFrame->pts = av_frame_get_best_effort_timestamp(OutVideoFrame)) == AV_NOPTS_VALUE) 
						   {
							   OutVideoFrame->pts  = 0;
						   }

						   if(lsatpts==0)
							   lsatpts=OutVideoFrame->pts;
						   if(lsatpts>OutVideoFrame->pts)
						   {
							   int lx=0;
							   lx++;
						   }
						   lsatpts=OutVideoFrame->pts;
						   ret = avcodec_encode_video2(CtxH264, &OutVideoPack,OutVideoFrame, &got_picture);
						   if(got_picture>0)
						   {
							   int64_t pts=OutVideoPack.pts*1000* av_q2d(pInputFormatContext->streams[videoindex]->time_base);
							   G_CAV_Flv.LocalRecord(true,OutVideoPack.data,OutVideoPack.size,pts);
							   av_free_packet(&OutVideoPack);
						   }
					}

					frame_index++;
				}
				av_free_packet(&InPack);
		}else if(InPack.stream_index ==audioindex)
		{

			len = avcodec_decode_audio4(pInputAudioCodecContext, OutAudioFrame, &nComplete, &InPack);
			if(nComplete > 0)
			{
		 	    int dec_channel_layout = get_valid_channel_layout(OutAudioFrame->channel_layout, av_frame_get_channels(OutAudioFrame));
			    bool reconfigure = cmp_audio_fmts(kk_AVFilter.audio_filter_src.fmt, kk_AVFilter.audio_filter_src.channels,
				(AVSampleFormat)OutAudioFrame->format, av_frame_get_channels(OutAudioFrame))    ||
				kk_AVFilter.audio_filter_src.channel_layout != dec_channel_layout ||
				kk_AVFilter.audio_filter_src.freq           != OutAudioFrame->sample_rate;

				if (reconfigure) 
				{

					char buf1[1024], buf2[1024];
					av_get_channel_layout_string(buf1, sizeof(buf1), -1, kk_AVFilter.audio_filter_src.channel_layout);
					av_get_channel_layout_string(buf2, sizeof(buf2), -1, dec_channel_layout);
				
					kk_AVFilter.audio_filter_src.fmt            = (AVSampleFormat)OutAudioFrame->format;
					kk_AVFilter.audio_filter_src.channels       = av_frame_get_channels(OutAudioFrame);
					kk_AVFilter.audio_filter_src.channel_layout = dec_channel_layout;
					kk_AVFilter.audio_filter_src.freq           = OutAudioFrame->sample_rate;
					
                   
					
					if ((ret = configure_audio_filters(&kk_AVFilter, NULL, 1)) < 0)
					{
						 int xxll=0;
						 xxll++;
					}
				
				}

				if ((ret = av_buffersrc_add_frame(kk_AVFilter.InAudioSrc, OutAudioFrame)) < 0)
				{
					
					continue;
				}
				while ((ret = av_buffersink_get_frame_flags(kk_AVFilter.OutAudioSink, OutAudioFrame, 0)) >= 0) 
				{

					AVRational tb = kk_AVFilter.OutAudioSink->inputs[0]->time_base;
					int wanted_nb_samples=OutAudioFrame->nb_samples;
					//avcodec_fill_audio_frame(OutAudioFrame,wanted_nb_samples,CtxAAC->sample_fmt,audio_buf1,len2,0);
					ret=avcodec_encode_audio2(CtxAAC,&OutAudioPack, OutAudioFrame, &got_picture);
					if(got_picture>0)
					{
						OutAudioPack.pts=InPack.pts;
						int side_size=0;
						uint8_t *side = av_packet_get_side_data(&OutAudioPack, AV_PKT_DATA_NEW_EXTRADATA, &side_size);
						//int 

						int64_t pts= av_rescale_q_rnd(OutAudioPack.pts,tb, 
							CtxAAC->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));  
						pts=pts*1000*av_q2d(CtxAAC->time_base);

						G_CAV_Flv.LocalRecord(false,OutAudioPack.data,OutAudioPack.size, pts);
						av_free_packet(&OutAudioPack);
					}
				
				}

			   
			}
			av_free_packet(&InPack);
		}else{
			av_free_packet(&InPack);
		}

	}

	
	av_frame_free(&OutAudioFrame);
	avcodec_close(pInputAudioCodecContext);
	av_free(pInputFormatContext);
	return 0;
}

static void ffp_show_dict(const char *tag, AVDictionary *dict)
{
	AVDictionaryEntry *t = NULL;

	while ((t = av_dict_get(dict, "", t, AV_DICT_IGNORE_SUFFIX))) 
	{

	}
}

static int configure_filtergraph(AVFilterGraph *graph, const char *filtergraph,
								 AVFilterContext *source_ctx, AVFilterContext *sink_ctx)
{
	int ret, i;
	int nb_filters = graph->nb_filters;
	AVFilterInOut *outputs = NULL, *inputs = NULL;

	if (filtergraph) {
		outputs = avfilter_inout_alloc();
		inputs  = avfilter_inout_alloc();
		if (!outputs || !inputs) {
			ret = AVERROR(ENOMEM);
			goto fail;
		}

		outputs->name       = av_strdup("in");
		outputs->filter_ctx = source_ctx;
		outputs->pad_idx    = 0;
		outputs->next       = NULL;

		inputs->name        = av_strdup("out");
		inputs->filter_ctx  = sink_ctx;
		inputs->pad_idx     = 0;
		inputs->next        = NULL;

		if ((ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, NULL)) < 0)
			goto fail;
	} else {
		if ((ret = avfilter_link(source_ctx, 0, sink_ctx, 0)) < 0)
			goto fail;
	}

	/* Reorder the filters to ensure that inputs of the custom filters are merged first */
	for (i = 0; i < graph->nb_filters - nb_filters; i++)
		FFSWAP(AVFilterContext*, graph->filters[i], graph->filters[i + nb_filters]);

	ret = avfilter_graph_config(graph, NULL);
fail:
	avfilter_inout_free(&outputs);
	avfilter_inout_free(&inputs);
	return ret;
}




double get_rotation(AVStream *st)
{
	AVDictionaryEntry *rotate_tag = av_dict_get(st->metadata, "rotate", NULL, 0);
	uint8_t* displaymatrix = av_stream_get_side_data(st,
		AV_PKT_DATA_DISPLAYMATRIX, NULL);
	double theta = 0;

	if (rotate_tag && *rotate_tag->value && strcmp(rotate_tag->value, "0")) {
		char *tail;
		theta = av_strtod(rotate_tag->value, &tail);
		if (*tail)
			theta = 0;
	}
	if (displaymatrix && !theta)
		theta = -av_display_rotation_get((int32_t*) displaymatrix);

	theta -= 360*floor(theta/360 + 0.9/360);

	/*if (fabs(theta - 90*round(theta/90)) > 2)
		av_log(NULL, AV_LOG_WARNING, "Odd rotation angle.\n"
		"If you want to help, upload a sample "
		"of this file to ftp://upload.ffmpeg.org/incoming/ "
		"and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)");*/

	return theta;
}
static int configure_video_filters(AVFilterGraph *graph, KK_AVFilter  *is, const char *vfilters, AVFrame *frame)
{
    static const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    char sws_flags_str[512] = "";
    char buffersrc_args[256];
    int ret;
    AVFilterContext *filt_src = NULL, *filt_out = NULL, *last_filter = NULL;
    AVCodecContext *codec = is->video_st->codec;
    AVRational fr = av_guess_frame_rate(is->ic, is->video_st, NULL);
    AVDictionaryEntry *e = NULL;
     AVDictionary *sws_dict=NULL;
    while ((e = av_dict_get(sws_dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
        if (!strcmp(e->key, "sws_flags")) {
            av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", "flags", e->value);
        } else
            av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", e->key, e->value);
    }
    if (strlen(sws_flags_str))
        sws_flags_str[strlen(sws_flags_str)-1] = '\0';

    graph->scale_sws_opts = av_strdup(sws_flags_str);

    snprintf(buffersrc_args, sizeof(buffersrc_args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             frame->width, frame->height, frame->format,
             is->video_st->time_base.num, is->video_st->time_base.den,
             codec->sample_aspect_ratio.num, FFMAX(codec->sample_aspect_ratio.den, 1));
    if (fr.num && fr.den)
        av_strlcatf(buffersrc_args, sizeof(buffersrc_args), ":frame_rate=%d/%d", fr.num, fr.den);

    if ((ret = avfilter_graph_create_filter(&filt_src,
                                            avfilter_get_by_name("buffer"),
                                            "ffplay_buffer", buffersrc_args, NULL,
                                            graph)) < 0)
        goto fail;

    ret = avfilter_graph_create_filter(&filt_out,
                                       avfilter_get_by_name("buffersink"),
                                       "ffplay_buffersink", NULL, NULL, graph);
    if (ret < 0)
        goto fail;

    if ((ret = av_opt_set_int_list(filt_out, "pix_fmts", pix_fmts,  AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
        goto fail;

    last_filter = filt_out;

/* Note: this macro adds a filter before the lastly added filter, so the
 * processing order of the filters is in reverse */
#define INSERT_FILT(name, arg) do {                                          \
    AVFilterContext *filt_ctx;                                               \
                                                                             \
    ret = avfilter_graph_create_filter(&filt_ctx,                            \
                                       avfilter_get_by_name(name),           \
                                       "ffplay_" name, arg, NULL, graph);    \
    if (ret < 0)                                                             \
        goto fail;                                                           \
                                                                             \
    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
    if (ret < 0)                                                             \
        goto fail;                                                           \
                                                                             \
    last_filter = filt_ctx;                                                  \
} while (0)

    /* SDL YUV code is not handling odd width/height for some driver
     * combinations, therefore we crop the picture to an even width/height. */
    INSERT_FILT("crop", "floor(in_w/2)*2:floor(in_h/2)*2");

    if (autorotate) {
        double theta  = get_rotation(is->video_st);

        if (fabs(theta - 90) < 1.0) {
            INSERT_FILT("transpose", "clock");
        } else if (fabs(theta - 180) < 1.0) {
            INSERT_FILT("hflip", NULL);
            INSERT_FILT("vflip", NULL);
        } else if (fabs(theta - 270) < 1.0) {
            INSERT_FILT("transpose", "cclock");
        } else if (fabs(theta) > 1.0) {
            char rotate_buf[64];
            snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
            INSERT_FILT("rotate", rotate_buf);
        }
    }

    if ((ret = configure_filtergraph(graph, vfilters, filt_src, last_filter)) < 0)
        goto fail;

    is->in_video_filter  = filt_src;
    is->out_video_filter = filt_out;

fail:
    return ret;
}

static int configure_audio_filters(KK_AVFilter  *is, const char *afilters, int force_output_format)
{
	static  enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
	int sample_rates[2] = { 0, -1 };
	int64_t channel_layouts[2] = { 0, -1 };
	int channels[2] = { 0, -1 };
	AVFilterContext *filt_asrc = NULL, *filt_asink = NULL;
	char aresample_swr_opts[512] = "";
	AVDictionaryEntry *e = NULL;
	AVFilter*  Avin;
	int ret;
	char asrc_args[256];
	memset(asrc_args,0,256);


	avfilter_free(is->InAudioSrc);
	is->InAudioSrc=NULL;
	avfilter_free(is->OutAudioSink);
	is->OutAudioSink=NULL;
	avfilter_graph_free(&is->AudioGraph);
	is->AudioGraph=NULL;
	if (!(is->AudioGraph= avfilter_graph_alloc()))
		return AVERROR(ENOMEM);



	AVDictionary *swr_opts=NULL;
	ffp_show_dict("swr-opts   ", swr_opts);
	//while ((e = av_dict_get(swr_opts, "", e, AV_DICT_IGNORE_SUFFIX)))
	//	av_strlcatf(aresample_swr_opts, sizeof(aresample_swr_opts), "%s=%s:", e->key, e->value);/**/
	if (strlen(aresample_swr_opts))
		aresample_swr_opts[strlen(aresample_swr_opts)-1] = '\0';
	av_opt_set(is->AudioGraph, "aresample_swr_opts", aresample_swr_opts, 0);


	ret = snprintf(asrc_args, sizeof(asrc_args),
		"sample_rate=%d:sample_fmt=%s:channels=%d:time_base=%d/%d",
		is->audio_filter_src.freq, av_get_sample_fmt_name(is->audio_filter_src.fmt),
		is->audio_filter_src.channels,
		1, is->audio_filter_src.freq);

	if (is->audio_filter_src.channel_layout)
#ifdef WIN32
		snprintf(asrc_args + ret, sizeof(asrc_args) - ret,
		":channel_layout=0x%d",  is->audio_filter_src.channel_layout);/**/
#else
		snprintf(asrc_args + ret, sizeof(asrc_args) - ret,
		":channel_layout=0x%lld",  is->audio_filter_src.channel_layout);/**/
#endif


	ret = avfilter_graph_create_filter(&filt_asrc,
		avfilter_get_by_name("abuffer"), "ffplay_abuffer",
		asrc_args, NULL, is->AudioGraph);

	if (ret < 0)
		goto endXX;

	Avin=avfilter_get_by_name("abuffersink");
	ret = avfilter_graph_create_filter(&filt_asink,
		Avin, "ffplay_abuffersink",
		NULL, NULL, is->AudioGraph);
	if (ret < 0)
		goto endXX;


	if ((ret = av_opt_set_int_list(filt_asink, "sample_fmts", sample_fmts,  AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
		goto endXX;
	if ((ret = av_opt_set_int(filt_asink, "all_channel_counts", 1, AV_OPT_SEARCH_CHILDREN)) < 0)
		goto endXX;

	if (force_output_format) {
		channel_layouts[0] = is->audio_filter_src.channel_layout;
		channels       [0] = is->audio_filter_src.channels;
		sample_rates   [0] = is->audio_filter_src.freq;
		//	sample_fmts[0]=is->audio_filter_src.fmt;
		if ((ret = av_opt_set_int(filt_asink, "all_channel_counts", 0, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto endXX;
		if ((ret = av_opt_set_int_list(filt_asink, "channel_layouts", channel_layouts,  -1, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto endXX;
		if ((ret = av_opt_set_int_list(filt_asink, "channel_counts" , channels       ,  -1, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto endXX;
		if ((ret = av_opt_set_int_list(filt_asink, "sample_rates"   , sample_rates   ,  -1, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto endXX;
		if ((ret = av_opt_set_int_list(filt_asink, "sample_fmts", sample_fmts,  AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto endXX;
	}


	if ((ret = configure_filtergraph(is->AudioGraph, afilters, filt_asrc, filt_asink)) < 0)
		goto endXX;

	is->InAudioSrc  = filt_asrc;
	is->OutAudioSink = filt_asink;
endXX:if (ret < 0)
	  {
		  avfilter_free(is->InAudioSrc);
		  avfilter_free(is->OutAudioSink);
		  avfilter_graph_free(&is->AudioGraph);
	  }
	  return ret;
}