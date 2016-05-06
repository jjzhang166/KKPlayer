/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "KKInternal.h"
#include <math.h>
#include <assert.h>
#include <time.h>
 //<tgmath.h> 
/***********KKPlaye 内部实现*************/
static int lowres = 0;
int cxlp=0;
static int64_t sws_flags = SWS_BICUBIC;
unsigned __stdcall  Audio_Thread(LPVOID lpParameter);
unsigned __stdcall  Video_thread(LPVOID lpParameter);
#ifdef WIN32_KK
inline long rint(double x) 
{ 
	if(x >= 0.)
		return (long)(x + 0.5); 
	else 
		return (long)(x - 0.5); 
}
#endif



int quit = 0;

void packet_queue_init(SKK_PacketQueue  *q) 
{
	memset(q, 0, sizeof(SKK_PacketQueue));
	q->pLock = new CKKLock();
	q->abort_request = 1;
}
//包入队列
int packet_queue_put(SKK_PacketQueue *q, AVPacket *pkt,AVPacket *flush_pkt) 
{

	SKK_AVPacketList *pkt1;
	if(av_dup_packet(pkt) < 0) 
	{
		return -1;
	}
	pkt1 = (SKK_AVPacketList *)av_malloc(sizeof(SKK_AVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	if (pkt != flush_pkt)
		q->serial++;
	pkt1->serial = q->serial;

	q->pLock->Lock();

	if (!q->last_pkt)
	{
		q->first_pkt = pkt1;//第一个包
	}
	else
		q->last_pkt->next = pkt1;
	
	q->last_pkt = pkt1;
	q->nb_packets++;
	q->size += pkt1->pkt.size;
	
	q->pLock->Unlock();
	return 0;
}

int packet_queue_get(SKK_PacketQueue *q, AVPacket *pkt, int block, int *serial)
{
	SKK_AVPacketList *pkt1;
	int ret=-1;

	q->pLock->Lock();

	for(;;) 
	{
		pkt1 = q->first_pkt;
		if (pkt1) 
		{
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			q->size -= pkt1->pkt.size;
			*pkt = pkt1->pkt;
			*serial=pkt1->serial;
			av_free(pkt1);
			ret = 1;
			break;
		} else if (!block) 
		{
			ret = 0;
			break;
		} else 
		{
			break;
		}
	}
	q->pLock->Unlock();

	return ret;
}

void frame_queue_destory(SKK_FrameQueue *f)
{
	int i;
	for (i = 0; i < f->max_size; i++) 
	{
		SKK_Frame *vp = &f->queue[i];
		frame_queue_unref_item(vp);
		av_frame_free(&vp->frame);
		av_free(vp->buffer);
	}
}
/**********清空队列***********/
void packet_queue_flush(SKK_PacketQueue *q)
{
	SKK_AVPacketList *pkt, *pkt1;

	q->pLock->Lock();
	for (pkt = q->first_pkt; pkt; pkt = pkt1) 
	{
		pkt1 = pkt->next;
		av_free_packet(&pkt->pkt);
		av_freep(&pkt);
	}
	q->last_pkt = NULL;
	q->first_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;

	q->pLock->Unlock();
}
//时钟操作
double get_clock(SKK_Clock *c)
{
	if (*c->queue_serial < c->serial&&*c->queue_serial>0&& c->serial>0)
		return NAN;
	if (c->paused) 
	{
		return c->pts;
	} 
	else 
	{
		//当前系统操作
		double time = av_gettime_relative() / 1000000.0;
		double a=c->pts_drift + time;
		double b=(time - c->last_updated) * (1.0 - c->speed);
		return a-b;
		//return  c->pts_drift + time- (time - c->last_updated) * (1.0 - c->speed);
	}
}

void set_clock_at(SKK_Clock *c, double pts, int serial, double time)
{
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = serial;
}

void set_clock(SKK_Clock *c, double pts, int serial)
{
	if(c->serial!=serial)
	{
		double time = av_gettime_relative() / 1000000.0;
		set_clock_at(c, pts, serial, time);
	}
	
}

void set_clock_speed(SKK_Clock *c, double speed)
{
	set_clock(c, get_clock(c), c->serial);
	c->speed = speed;
}

void init_clock(SKK_Clock *c, int *queue_serial)
{
	c->speed = 1.0;
	c->paused = 0;
	c->queue_serial = queue_serial;
	set_clock(c, NAN, -1);
}

static void sync_clock_to_slave(SKK_Clock *c, SKK_Clock *slave)
{
	double clock = get_clock(c);
	double slave_clock = get_clock(slave);
	if (!isNAN(slave_clock) && (isNAN(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		set_clock(c, slave_clock, slave->serial);
}

int get_master_sync_type(SKK_VideoState *is)
{
	if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) 
	{
		if (is->video_st)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;
	} else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) 
	{
		if (is->audio_st)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;
	} else 
	{
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

/* get the current master clock value */
double get_master_clock(SKK_VideoState *is)
{
	double val;

	switch (get_master_sync_type(is)) 
	{
		case AV_SYNC_VIDEO_MASTER:
			val = get_clock(&is->vidclk);
			break;
		case AV_SYNC_AUDIO_MASTER:
			val = get_clock(&is->audclk);
			break;
		default:
			val = get_clock(&is->extclk);
			break;
	}
	return val;
}

static int synchronize_audio(SKK_VideoState *is, int nb_samples)
{
    int wanted_nb_samples = nb_samples;

    /* if not master, then we try to remove or add samples to correct the clock */
    if (get_master_sync_type(is) != AV_SYNC_AUDIO_MASTER) 
	{
        double diff, avg_diff;
        int min_nb_samples, max_nb_samples;

        diff = get_clock(&is->audclk) - get_master_clock(is);

        if (!isNAN(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) 
		{
					is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
					if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) 
					{
						/* not enough measures to have a correct estimate */
						is->audio_diff_avg_count++;
					} else 
					{
						/* estimate the A-V difference */
						avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

						if (fabs(avg_diff) >= is->audio_diff_threshold) 
						{
							wanted_nb_samples = nb_samples + (int)(diff * is->audio_src.freq);
							min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
							max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
							wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
						}
						/*av_log(NULL, AV_LOG_TRACE, "diff=%f adiff=%f sample_diff=%d apts=%0.3f %f\n",
								diff, avg_diff, wanted_nb_samples - nb_samples,
								is->audio_clock, is->audio_diff_threshold);*/
					}
				} else 
		         {
					/* too big difference : may be initial PTS errors, so
					   reset A-V filter */
					is->audio_diff_avg_count = 0;
					is->audio_diff_cum       = 0;
				}
      }

    return wanted_nb_samples;
}


int frame_queue_init(SKK_FrameQueue *f, SKK_PacketQueue *pktq, int max_size, int keep_last)
{
	int i;
	memset(f, 0, sizeof(SKK_FrameQueue));

	f->mutex = new CKKLock();
	f->pktq = pktq;
	f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
	f->keep_last = !!keep_last;
	for (i = 0; i < f->max_size; i++)
		if (!(f->queue[i].frame = av_frame_alloc()))
			return AVERROR(ENOMEM);
	return 0;
}

//音量调节
#define VOLUME_VAL 1//0.90
static int init_filter_graph(AVFilterGraph **graph, AVFilterContext **src,
                             AVFilterContext **sink,int sample_rate,int nb_channels ,int channel_layout,AVSampleFormat sample_fmt)
{
    AVFilterGraph *filter_graph;
    AVFilterContext *abuffer_ctx;
    AVFilter        *abuffer;
    AVFilterContext *volume_ctx;
    AVFilter        *volume;
    AVFilterContext *aformat_ctx;
    AVFilter        *aformat;
    AVFilterContext *abuffersink_ctx;
    AVFilter        *abuffersink;

    AVDictionary *options_dict = NULL;
    uint8_t options_str[1024];
    uint8_t ch_layout[64];

    int err;

    /* Create a new filtergraph, which will contain all the filters. */
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        fprintf(stderr, "Unable to create filter graph.\n");
        return AVERROR(ENOMEM);
    }

    /* Create the abuffer filter;
     * it will be used for feeding the data into the graph. */
    abuffer = avfilter_get_by_name("abuffer");
    if (!abuffer) {
        fprintf(stderr, "Could not find the abuffer filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffer_ctx = avfilter_graph_alloc_filter(filter_graph, abuffer, "src");
    if (!abuffer_ctx) {
        fprintf(stderr, "Could not allocate the abuffer instance.\n");
        return AVERROR(ENOMEM);
    }

	 AVRational avr={ 1, 44100 };
    /* Set the filter options through the AVOptions API. */
    av_get_channel_layout_string((char*)ch_layout, sizeof(ch_layout), 0, channel_layout);
    av_opt_set    (abuffer_ctx, "channel_layout",(char*)ch_layout,                            AV_OPT_SEARCH_CHILDREN);
    av_opt_set    (abuffer_ctx, "sample_fmt",     av_get_sample_fmt_name(sample_fmt), AV_OPT_SEARCH_CHILDREN);
    av_opt_set_q  (abuffer_ctx, "time_base",    avr ,  AV_OPT_SEARCH_CHILDREN);
    av_opt_set_int(abuffer_ctx, "sample_rate",    sample_rate,                     AV_OPT_SEARCH_CHILDREN);

    /* Now initialize the filter; we pass NULL options, since we have already
     * set all the options above. */
    err = avfilter_init_str(abuffer_ctx, NULL);
    if (err < 0) {
        fprintf(stderr, "Could not initialize the abuffer filter.\n");
        return err;
    }

    /* Create volume filter. */
    volume = avfilter_get_by_name("volume");
    if (!volume) {
        fprintf(stderr, "Could not find the volume filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    volume_ctx = avfilter_graph_alloc_filter(filter_graph, volume, "volume");
    if (!volume_ctx) {
        fprintf(stderr, "Could not allocate the volume instance.\n");
        return AVERROR(ENOMEM);
    }

    /* A different way of passing the options is as key/value pairs in a
     * dictionary. */
    av_dict_set(&options_dict, "volume", AV_STRINGIFY(VOLUME_VAL), 0);
    err = avfilter_init_dict(volume_ctx, &options_dict);
    av_dict_free(&options_dict);
    if (err < 0) {
        fprintf(stderr, "Could not initialize the volume filter.\n");
        return err;
    }

    /* Create the aformat filter;
     * it ensures that the output is of the format we want. */
    aformat = avfilter_get_by_name("aformat");
    if (!aformat) {
        fprintf(stderr, "Could not find the aformat filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    aformat_ctx = avfilter_graph_alloc_filter(filter_graph, aformat, "aformat");
    if (!aformat_ctx) {
        fprintf(stderr, "Could not allocate the aformat instance.\n");
        return AVERROR(ENOMEM);
    }

    /* A third way of passing the options is in a string of the form//
     * key1=value1:key2=value2.... */
    sprintf((char*)options_str, 
             "sample_fmts=%s:sample_rates=%d:channel_layouts=0x%d",
             av_get_sample_fmt_name(AV_SAMPLE_FMT_S16), 44100,
            2);// (uint64_t)AV_CH_LAYOUT_STEREO);
    err = avfilter_init_str(aformat_ctx,(char*) options_str);
    if (err < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not initialize the aformat filter.\n");
        return err;
    }

    /* Finally create the abuffersink filter;
     * it will be used to get the filtered data out of the graph. */
    abuffersink = avfilter_get_by_name("abuffersink");
    if (!abuffersink) {
        fprintf(stderr, "Could not find the abuffersink filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");
    if (!abuffersink_ctx) {
        fprintf(stderr, "Could not allocate the abuffersink instance.\n");
        return AVERROR(ENOMEM);
    }

    /* This filter takes no options. */
    err = avfilter_init_str(abuffersink_ctx, NULL);
    if (err < 0) {
        fprintf(stderr, "Could not initialize the abuffersink instance.\n");
        return err;
    }

    /* Connect the filters;
     * in this simple case the filters just form a linear chain. */
    err = avfilter_link(abuffer_ctx, 0, volume_ctx, 0);
    if (err >= 0)
        err = avfilter_link(volume_ctx, 0, aformat_ctx, 0);
    if (err >= 0)
        err = avfilter_link(aformat_ctx, 0, abuffersink_ctx, 0);
    if (err < 0) {
        fprintf(stderr, "Error connecting filters\n");
        return err;
    }

    /* Configure the graph. */
    err = avfilter_graph_config(filter_graph, NULL);
    if (err < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error configuring the filter graph\n");
        return err;
    }

    *graph = filter_graph;
    *src   = abuffer_ctx;
    *sink  = abuffersink_ctx;

    return 0;
}
//解码音频
int audio_decode_frame( SKK_VideoState *pVideoInfo) 
{  
    int n=0;
	AVCodecContext *aCodecCtx=pVideoInfo->auddec.avctx;	
	AVPacket pkt;
	memset(&pkt,0,sizeof(pkt));
    int audio_pkt_size = 0;
	AVFrame frame;
	memset(&frame,0,sizeof(frame));
    int64_t dec_channel_layout;
	int len1, resampled_data_size, data_size = 0;
    int wanted_nb_samples;

	for(;;) 
	{
		while(audio_pkt_size > 0) 
		{
			//AV_BUFFERSRC_FLAG_KEEP_REF
			int got_frame = 0;//av_frame_free
			len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
			if(len1 < 0) 
			{
				/* if error, skip frame */
				audio_pkt_size = 0;
				break;
			}
			audio_pkt_size -= len1;
			if(audio_pkt_size>0)
			{
				assert(0);
			}
			data_size=0;
			if(got_frame) 
			{
					
				if(0)
				{/* Send the frame to the input of the filtergraph. */
							int err = av_buffersrc_add_frame(pVideoInfo->src, &frame);
							if (err < 0) 
							{
								av_frame_unref(&frame);
								return -1;
							}
 /* Get all the filtered output that is available. */
							err = av_buffersink_get_frame(pVideoInfo->sink, &frame);
							if(err<0)
							{
								av_frame_unref(&frame);
								return -1;
							}

			     }
							data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels,frame.nb_samples,(AVSampleFormat)frame.format,1);
							dec_channel_layout =
								(frame.channel_layout && av_frame_get_channels(&frame) == av_get_channel_layout_nb_channels(frame.channel_layout)) ?
								 frame.channel_layout : av_get_default_channel_layout(av_frame_get_channels(&frame));
							AVSampleFormat bc=(AVSampleFormat)frame.format;
							
							wanted_nb_samples = synchronize_audio(pVideoInfo, frame.nb_samples);
							

							 if (
								 frame.format        != pVideoInfo->audio_src.fmt            ||
								 dec_channel_layout       != pVideoInfo->audio_src.channel_layout ||
								 frame.sample_rate   != pVideoInfo->audio_src.freq           ||
								 (wanted_nb_samples       != frame.nb_samples && !pVideoInfo->swr_ctx)
								 ) 
							 {	
									  swr_free(&pVideoInfo->swr_ctx);
									 /* pVideoInfo->swr_ctx =swr_alloc();
									  av_opt_set_int(pVideoInfo->swr_ctx, "ich", frame.channels, 0);
									  av_opt_set_int(pVideoInfo->swr_ctx, "och", pVideoInfo->audio_tgt.channels, 0);
									  av_opt_set_int(pVideoInfo->swr_ctx, "in_sample_rate",  frame.sample_rate, 0);
									  av_opt_set_int(pVideoInfo->swr_ctx, "out_sample_rate",  pVideoInfo->audio_tgt.freq, 0);
									  av_opt_set_sample_fmt(pVideoInfo->swr_ctx, "in_sample_fmt", (AVSampleFormat)frame.format, 0);
									  av_opt_set_sample_fmt(pVideoInfo->swr_ctx, "out_sample_fmt", pVideoInfo->audio_tgt.fmt, 0);*/
									  pVideoInfo->swr_ctx =swr_alloc_set_opts(NULL,
										  pVideoInfo->audio_tgt.channel_layout, pVideoInfo->audio_tgt.fmt,pVideoInfo->audio_tgt.freq,
										  dec_channel_layout,           (AVSampleFormat)frame.format, frame.sample_rate,
										  0, NULL);

									 if (!pVideoInfo->swr_ctx || swr_init(pVideoInfo->swr_ctx) < 0) 
									 {
										 swr_free(&pVideoInfo->swr_ctx);
										 return -1;
									 }
									 pVideoInfo->audio_src.channel_layout = dec_channel_layout;
									 pVideoInfo->audio_src.channels       = av_frame_get_channels(&frame);
									 pVideoInfo->audio_src.freq =frame.sample_rate;
									 pVideoInfo->audio_src.fmt = (AVSampleFormat)frame.format;
							 }


							 if (pVideoInfo->swr_ctx) 
							 {
								 //数据指针  &frame.data[0];//
								 const uint8_t **inextended_data = (const uint8_t **)frame.extended_data;
								 
								 int out_count = (int64_t)wanted_nb_samples * pVideoInfo->audio_tgt.freq / frame.sample_rate + 256;
								 //输出大小
								 int out_size  = av_samples_get_buffer_size(NULL, pVideoInfo->audio_tgt.channels, out_count, pVideoInfo->audio_tgt.fmt, 0);
								 int len2;
								 if (out_size < 0) 
								 {
									// av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
									 return -1;
								 }
								 if (wanted_nb_samples != frame.nb_samples) 
								 {
									 if (swr_set_compensation(pVideoInfo->swr_ctx, (wanted_nb_samples - frame.nb_samples) * pVideoInfo->audio_tgt.freq / frame.sample_rate,
										 wanted_nb_samples * pVideoInfo->audio_tgt.freq / frame.sample_rate) < 0) 
									 {
											// av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
											 return -1;
									 }
								 }
								
								
								 //分配内存
								 av_fast_malloc(&pVideoInfo->audio_buf1, &pVideoInfo->audio_buf_size, out_size);
								
								 if (!pVideoInfo->audio_buf1)
								 {
									 return AVERROR(ENOMEM);
								 }
								 memset(pVideoInfo->audio_buf1,0,out_size);

								 //输出地址
								 uint8_t **OutData = &pVideoInfo->audio_buf1;

								int ll=out_size / pVideoInfo->audio_tgt.channels  / av_get_bytes_per_sample(pVideoInfo->audio_tgt.fmt);
								 //音频转化
								 len2 = swr_convert(
									                  pVideoInfo->swr_ctx, 
													  OutData,               out_count, 
									                  inextended_data, frame.nb_samples);
								 if (len2 < 0) 
								 {
									 //av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
									 return -1;
								 }
								 if (len2 >= out_count) 
								 {
									 //av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
									 if (swr_init(pVideoInfo->swr_ctx) < 0)
										 swr_free(&pVideoInfo->swr_ctx);
								 }
								 pVideoInfo->audio_buf = pVideoInfo->audio_buf1;

								 resampled_data_size = len2 *pVideoInfo->audio_tgt.channels * av_get_bytes_per_sample(pVideoInfo->audio_tgt.fmt);							
							 }else 
							 {
								 resampled_data_size = data_size;
								 memcpy(pVideoInfo->audio_buf1,frame.data[0],data_size);
								 pVideoInfo->audio_buf = pVideoInfo->audio_buf1;
							 }
							  av_frame_unref(&frame);

			}
			if(data_size <= 0) 
			{
				/* No data yet, get more frames */
				continue;
			}
			n = 2 * pVideoInfo->auddec.avctx->channels;
			pVideoInfo->audio_clock += (double)data_size /(double)(n * pVideoInfo->auddec.avctx->sample_rate);

			if(pkt.data)
			{
				av_free_packet(&pkt);
				pkt.data=NULL;
			}
			return resampled_data_size;
		}
		if(pkt.data)
		{
			av_free_packet(&pkt);
		}
        av_frame_unref(&frame);
		if(quit) 
		{
			return -1;
		}

		if(packet_queue_get(&pVideoInfo->audioq, &pkt, 1,&pVideoInfo->auddec.pkt_serial) < 0) 
		{
			return -1;
		}
		audio_pkt_size = pkt.size;

		/*if (!isNAN(frame.pts))
			  pVideoInfo->audio_clock = pkt.pts + (double) frame.nb_samples / frame.sample_rate;
		else*/
		if(pkt.pts != AV_NOPTS_VALUE) 
              pVideoInfo->audio_clock = av_q2d(pVideoInfo->audio_st->time_base)*pkt.pts;

		if ((av_gettime_relative() - pVideoInfo->audio_callback_time) > 1000000LL * pVideoInfo->audio_hw_buf_size / pVideoInfo->audio_tgt.bytes_per_sec / 2)
		    return -1;	
	}
}
void audio_callback2(void *userdata, char *stream, int len)
{
    //return;  
   audio_callback(userdata, stream,len);
}
void sdl_audio_callback(void *userdata, char *stream, int len)
{
	//audio_callback(userdata, stream,len);
	//return;
    while(1)
	{
	   Sleep(10000);
	}
}

/* prepare a new audio buffer */
/*******此处有bug********/
void audio_callback(void *userdata, char *stream, int len)
{
	SKK_VideoState *pVideoInfo=(SKK_VideoState *)userdata;
	memset(stream,0,len);
	if (pVideoInfo->paused)
	{
		return;
	}
	int audio_size=0, len1=0;
	int slen=len;
	//获取现在的时间
	pVideoInfo->audio_callback_time = av_gettime_relative();
	//Sleep(1);
	while (len > 0) 
	{
		if (pVideoInfo->audio_buf_index >= pVideoInfo->audio_buf_size) 
		{
			audio_size = audio_decode_frame(pVideoInfo);
			if (audio_size < 0)
			{
				//printf("silence_buf\n");
				/* if error, just output silence */
				pVideoInfo->audio_buf      = pVideoInfo->silence_buf;
				pVideoInfo->audio_buf_size = sizeof(pVideoInfo->silence_buf) / pVideoInfo->audio_tgt.frame_size * pVideoInfo->audio_tgt.frame_size;
			} else 
			{
				pVideoInfo->audio_buf_size = audio_size;
			}
			pVideoInfo->audio_buf_index = 0;
		}
		len1 = pVideoInfo->audio_buf_size - pVideoInfo->audio_buf_index;

#ifdef WIN32_KK
		if(audio_size>0){
		char abcd[100]="";
		sprintf(abcd,"\n slen:%d, len：%d,len1:%d",slen,len,len1);
		::OutputDebugStringA(abcd);
		}
#endif
		

		if (len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t *)pVideoInfo->audio_buf + pVideoInfo->audio_buf_index, len1);
		len -= len1;
		stream += len1;
		pVideoInfo->audio_buf_index += len1;
	}
	pVideoInfo->audio_write_buf_size = pVideoInfo->audio_buf_size - pVideoInfo->audio_buf_index;
	if (!isNAN(pVideoInfo->audio_clock)) 
	{
		set_clock_at(&pVideoInfo->audclk,     
			pVideoInfo->audio_clock - (double)(2 * pVideoInfo->audio_hw_buf_size + pVideoInfo->audio_write_buf_size) / pVideoInfo->audio_tgt.bytes_per_sec, 
			pVideoInfo->audio_clock_serial, 
			pVideoInfo->audio_callback_time / 1000000.0);
		sync_clock_to_slave(&pVideoInfo->extclk, &pVideoInfo->audclk);
	}
}


//打开音频
static int audio_open2( void *opaque,                               int64_t wanted_channel_layout, 
					    int wanted_nb_channels,                     int wanted_sample_rate, 
                        struct SKK_AudioParams *audio_hw_params)
{
	
	const char *env;
	static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
	static const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	env = "2";
	if (env) 
	{
		wanted_nb_channels = atoi(env);
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
	}
	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}
	wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
	
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);

	audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
	audio_hw_params->freq = 44100;
	audio_hw_params->channel_layout = wanted_channel_layout;
	audio_hw_params->channels = 2;
	audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
	audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
	if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0)
	{
		//av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
		return -1;
	}
	return 1024;
}


//初始化解码器
static void decoder_init(SKK_Decoder *d, AVCodecContext *avctx, SKK_PacketQueue *queue) 
{
	memset(d, 0, sizeof(SKK_Decoder));
	d->avctx = avctx;
	d->pQueue = queue;
	d->start_pts = AV_NOPTS_VALUE;
}

//开始解码
static void decoder_start(SKK_Decoder *d,unsigned (__stdcall* _StartAddress) (void *),SKK_VideoState* is)
{
	d->pQueue->abort_request = 0;
	packet_queue_put(d->pQueue, is->pflush_pkt,is->pflush_pkt);
#ifdef WIN32_KK
	d->decoder_tid.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, _StartAddress, (LPVOID)is, 0,&d->decoder_tid.Addr);
#else
	d->decoder_tid.Addr = pthread_create(&d->decoder_tid.Tid_task, NULL, (void* (*)(void*))_StartAddress, (LPVOID)is);
#endif
}
/****刷新队列,更新对列的大小****/
void frame_queue_push(SKK_FrameQueue *f)
{
	if (++f->windex >= f->max_size)
		f->windex = 0;
	f->mutex->Lock();
	f->size++;
	f->mutex->Unlock();
}
//返回 1 流媒体
int is_realtime(AVFormatContext *s)
{
	if(   !strcmp(s->iformat->name, "rtp")    || 
		  !strcmp(s->iformat->name, "rtsp")   || 
		  !strcmp(s->iformat->name, "sdp")
		)
		return 1;


	if( s->pb && !strncmp(s->filename, "rtmp:",5)&&strstr(s->filename,"live=1")>=0)
	{
		return 1;
	}
	if(  s->pb && 
		(  !strncmp(s->filename, "rtp:", 4)|| 
		   !strncmp(s->filename, "udp:", 4) 
		 )
	)
		return 1;
	return 0;
}
//打开流
int stream_component_open(SKK_VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->pFormatCtx;
    AVCodecContext *avctx=NULL;
	AVCodecContext  *aCodecCtx = NULL;
    AVCodec *codec=NULL;
    const char *forced_codec_name = NULL;
    AVDictionary *opts=NULL;
    AVDictionaryEntry *t = NULL;
    int sample_rate, nb_channels;
    int64_t channel_layout;
	int stream_lowres = lowres;
    int ret = 0;
   

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;
    avctx = ic->streams[stream_index]->codec;

	//if(avctx->codec_type==AVMEDIA_TYPE_VIDEO&&avctx->codec_id==AV_CODEC_ID_NONE)
	//    avctx->codec_id=AV_CODEC_ID_H264;
    codec = avcodec_find_decoder(avctx->codec_id);

    switch(avctx->codec_type)
	{
        case AVMEDIA_TYPE_AUDIO   : 
			//LOGE("Code:AVMEDIA_TYPE_AUDIO ");
			                         is->last_audio_stream= stream_index;
									 break;
        case AVMEDIA_TYPE_SUBTITLE: 
			      //    LOGE("Code:AVMEDIA_TYPE_SUBTITLE");              
					  is->last_subtitle_stream = stream_index;
									 break;
        case AVMEDIA_TYPE_VIDEO   : 
			//LOGE("Code:AVMEDIA_TYPE_VIDEO");  
			                         is->last_video_stream= stream_index;
									 break;
    }
    
    if (!codec) 
	{
		//LOGE("Code:-1");  
        return -1;
    }

    avctx->codec_id = codec->id;
   
	if(stream_lowres > av_codec_get_max_lowres(codec))
	{
	
		av_codec_get_max_lowres(codec);
		stream_lowres = av_codec_get_max_lowres(codec);
	}
	av_codec_set_lowres(avctx, stream_lowres);

	if(stream_lowres) 
		avctx->flags |= CODEC_FLAG_EMU_EDGE;
	//if (fast)   
		//avctx->flags2 |= CODEC_FLAG2_FAST;
	if(codec->capabilities & CODEC_CAP_DR1)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;

	//打开解码器
	if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) 
	{
		//LOGE("avcodec_open2 %d",avctx->codec_type);  
		//失败
		assert(0);
	}
   	//aCodecCtx = avcodec_alloc_context3(codec);
	//if(avcodec_copy_context(aCodecCtx, avctx) != 0) {
	//	fprintf(stderr, "Couldn't copy codec context");
	//	return -1; // Error copying codec context
	//}
	//avcodec_flush_buffers(avctx);
    is->eof = 0;
    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch (avctx->codec_type)
	{
        case AVMEDIA_TYPE_AUDIO:
							sample_rate    = avctx->sample_rate;
							nb_channels    = avctx->channels;
							channel_layout = avctx->channel_layout;
							
							/* Set up the filtergraph. */
							ret = init_filter_graph(&is->graph, &is->src, &is->sink,sample_rate, nb_channels ,channel_layout ,avctx->sample_fmt);
							if (ret < 0) 
							{
								//fprintf(stderr, "Unable to init filter graph:");
							}
							/* prepare audio output */
							if ((ret = audio_open2(is, channel_layout, nb_channels, sample_rate, &is->audio_tgt)) < 0)
							{
								assert(0);
								goto fail;
							}
							is->audio_hw_buf_size = ret;
							is->audio_src = is->audio_tgt;
							is->audio_buf_size  = 0;
							is->audio_buf_index = 0;

							/* init averaging filter */
							is->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
							is->audio_diff_avg_count = 0;
							/* since we do not have a precise anough audio fifo fullness,
							   we correct audio sync only if larger than this threshold */
							is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

							is->audio_stream = stream_index;
							is->audio_st = ic->streams[stream_index];

							decoder_init(&is->auddec, avctx, &is->audioq);
							if ((ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && 
								!ic->iformat->read_seek) 
							{
								is->auddec.start_pts = is->audio_st->start_time;
								is->auddec.start_pts_tb = is->audio_st->time_base;
							}
							is->auddec.decoder_tid.ThOver=false;
							decoder_start(&is->auddec,&Audio_Thread,is);
                            break;
    case AVMEDIA_TYPE_VIDEO:
							is->video_stream = stream_index;
							is->video_st = ic->streams[stream_index];

							
							decoder_init(&is->viddec, avctx, &is->videoq);
							is->queue_attachments_req = 1;
							is->viddec.decoder_tid.ThOver=false;
							decoder_start(&is->viddec,&Video_thread,is);
                            break;
    case AVMEDIA_TYPE_SUBTITLE:
							is->subtitle_stream = stream_index;
							is->subtitle_st = ic->streams[stream_index];

							decoder_init(&is->subdec, avctx, &is->subtitleq);
							is->subdec.decoder_tid.ThOver=true;
							//decoder_start(&is->subdec);
							break;
    default:
        break;
    }

fail:
    av_dict_free(&opts);

    return ret;
}






//音频解码线程
unsigned __stdcall  Audio_Thread(LPVOID lpParameter)
{
	
	//return 1;
	SKK_VideoState *pIs=(SKK_VideoState *)lpParameter;
	if(pIs->pKKAudio!=NULL)
	{
		pIs->pKKAudio->Start();
		while(!pIs->abort_request && pIs->pKKAudio!=NULL)
		{
			//LOGE("ReadAudio");
			pIs->pKKAudio->ReadAudio();
		}
	}
	LOGE("Audio_Thread over");
	pIs->auddec.decoder_tid.ThOver=true;
	return 1;
}
//字幕线程
unsigned __stdcall  Subtitle_thread(LPVOID lpParameter)
{
	SKK_VideoState *pIs=(SKK_VideoState *)lpParameter;
	pIs->auddec.decoder_tid.ThOver=true;
	return 1;
}

/*******同步视频********/
double synchronize_video(SKK_VideoState *is, AVFrame *src_frame, double pts)  
{  
	double frame_delay;  

	if(pts != 0)  
	{  
		/* if we have pts, set video clock to it */  
		is->audclk.pts = pts;  
	}  
	else  
	{  
		/* if we aren't given a pts, set it to the clock */  
		pts = is->audclk.pts;
	}  
	/* update the video clock */  
	frame_delay = av_q2d(is->video_st->codec->time_base);  
	/* if we are repeating a frame, adjust clock accordingly */  
	frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);  
	//is->video_clock += frame_delay;  
	return pts;  
} 



//队列是否能写
static SKK_Frame *frame_queue_peek_writable(SKK_FrameQueue *f)
{
	f->mutex->Lock();
	bool mm=true;
	if(f->size >= f->max_size &&
		!f->pktq->abort_request) 
	{
		/*****无信号******/
		//ResetEvent(f->m_WaitEvent);
		//LOGE("queue ResetCond");
		f->m_pWaitCond->ResetCond();
		f->mutex->Unlock();
		mm=false;
		f->m_pWaitCond->WaitCond(1);
	}
	if(mm)
	f->mutex->Unlock();

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[f->windex];
}
/********可写入的位置*******/
SKK_Frame *frame_queue_peek(SKK_FrameQueue *f)
{
	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}
/* return the number of undisplayed frames in the queue */
/********未显示的图片数**********/
int frame_queue_nb_remaining(SKK_FrameQueue *f)
{
	return f->size - f->rindex_shown;
}
/**********最后一次读的位置*************/
SKK_Frame *frame_queue_peek_last(SKK_FrameQueue *f)
{
	return &f->queue[f->rindex];
}
/* Unreference all the buffers referenced by frame and reset the frame fields.*/
void frame_queue_unref_item(SKK_Frame *vp)
{
	av_frame_unref(vp->frame);
	avsubtitle_free(&vp->sub);
}
void frame_queue_next(SKK_FrameQueue *f,bool NeedLock)
{
	/*****是否保存上一次的值******/
	if (f->keep_last && !f->rindex_shown)
	{
		f->rindex_shown = 1;
		return;
	}
	frame_queue_unref_item(&f->queue[f->rindex]);
	/******一个队列已经读取玩****/
	if (++f->rindex == f->max_size)
	{
		f->rindex = 0;
	}
	if(NeedLock)
	   f->mutex->Lock();
	f->size--;
	if(f->size<0)
		f->size=0;
	if(f->size<f->max_size)
	{
		//将事件有效
		//::SetEvent(f->m_WaitEvent);
		//LOGE("SetCond over");
		f->m_pWaitCond->SetCond();
		//LOGE("SetCond end");
	}
	if(NeedLock)
	f->mutex->Unlock();
}
SKK_Frame *frame_queue_peek_next(SKK_FrameQueue *f)
{
	return &f->queue[(f->rindex + f->rindex_shown + 1) % f->max_size];
}



double vp_duration(SKK_VideoState *is, SKK_Frame *vp, SKK_Frame *nextvp) 
{
	if (vp->serial != nextvp->serial) 
	{
		double duration = nextvp->pts - vp->pts;
		if (isNAN(duration) || duration <= 0 || duration > is->max_frame_duration)
			return vp->duration;
		else
			return duration;
	} else 
	{
		return 0.0;
	}
}

//更新显示时间
void update_video_pts(SKK_VideoState *is, double pts, int64_t pos, int serial) 
{
	/* update current video pts */
	set_clock(&is->vidclk, pts, serial);
	sync_clock_to_slave(&is->extclk, &is->vidclk);
}
/*****计算延迟时间******/
double compute_target_delay(double delay, SKK_VideoState *is)
{
    double sync_threshold, diff = 0;

    /* update delay to follow master synchronisation source */
    if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) 
	{
        /* if video is slave, we try to correct big delays by
           duplicating or deleting a frame */
		double a=get_clock(&is->vidclk);
		double b=get_master_clock(is);
        diff =  a- b;
        if(diff<0)
			return diff;
        /* skip or repeat frame. We take into account the
           delay to compute the threshold. I still don't know
           if it is the best guess */
        sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
        if (!isNAN(diff) && fabs(diff) < is->max_frame_duration) 
		{
            if (diff <= -sync_threshold)
                delay = FFMAX(0, delay + diff);
            else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                delay = delay + diff;
            else if (diff >= sync_threshold)
                delay = 2 * delay;
        }
    }


    return delay;
}

int GetBmpSize(int w,int h)
{
	return avpicture_get_size(PIX_FMT_RGB24, w,h);
}
struct SwsContext *BMPimg_convert_ctx=NULL;

#ifdef WIN32_KK
//位图
void saveBMP(SKK_VideoState *is,AVFrame *pSrcFrame, int width, int height, int index, int bpp=24)
{
	
	int w = width;
	int h = height;

	int numBytes=avpicture_get_size(PIX_FMT_RGB24, w,h);
	uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	AVFrame *pFrameRGB;
	pFrameRGB = avcodec_alloc_frame();
	avpicture_fill((AVPicture *)pFrameRGB, buffer,PIX_FMT_RGB24,  w, h);


	BMPimg_convert_ctx = sws_getCachedContext(BMPimg_convert_ctx,
		pSrcFrame->width,pSrcFrame->height, (PixelFormat)(pSrcFrame->format), w, h,PIX_FMT_BGR24, sws_flags, NULL, NULL, NULL);

	if (is->img_convert_ctx == NULL) 
	{
		fprintf(stderr, "Cannot initialize the conversion context\n");
		exit(1);
	}/**/
	sws_scale(is->img_convert_ctx, pSrcFrame->data, pSrcFrame->linesize,
		0, h, pFrameRGB->data, pFrameRGB->linesize);/**/

    //CreateDIBSection
	//2 构造 BITMAPINFOHEADER

	BITMAPINFOHEADER header;
	header.biSize = sizeof(BITMAPINFOHEADER);

	header.biWidth = w;
	header.biHeight = h*(-1);
	header.biBitCount = 24;
	header.biCompression = 0;
	header.biSizeImage = 0;
	header.biClrImportant = 0;
	header.biClrUsed = 0;
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biPlanes = 1;


	//3 构造文件头
	BITMAPFILEHEADER bmpFileHeader;
	HANDLE hFile = NULL;
	DWORD dwTotalWriten = 0;
	DWORD dwWriten;

	bmpFileHeader.bfType = 0x4d42; //'BM';
	bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+ width*height*bpp/8;

	char buf[256]="";
	sprintf_s(buf,255,"%s%d.bmp","D:/pic/",index);  
	FILE* pf = fopen(buf, "wb");
	fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, pf);
	fwrite(&header, sizeof(BITMAPINFOHEADER), 1, pf);
	fwrite(pFrameRGB->data[0], 1, numBytes, pf);
	fclose(pf);
	//sws_freeContext(img_convert_ctx);
	//释放资源
	av_free(buffer);
	buffer=NULL;
	avcodec_free_frame(&pFrameRGB);
	av_free(pFrameRGB);
	pFrameRGB=NULL;
}


int iiii=0;

#define CopyCursor(pcur) ((HCURSOR)CopyIcon((HICON)(pcur)))
void paint_mouse_pointer(SKK_VideoState *is)
{
    CURSORINFO ci = {0};

    ci.cbSize = sizeof(ci);
    if (GetCursorInfo(&ci)) 
	{
		HCURSOR icon =  ((HCURSOR)CopyIcon((HICON)(ci.hCursor)));//::CopyCursor(ci.hCursor);
        ICONINFO info;
        POINT pos;
        RECT clip_rect = is->virtual_rect;
        HWND hwnd = NULL;
        info.hbmMask = NULL;
        info.hbmColor = NULL;

        if (ci.flags != CURSOR_SHOWING)
            return;

        if (!icon) {
            /* Use the standard arrow cursor as a fallback.
             * You'll probably only hit this in Wine, which can't fetch
             * the current system cursor. */
            icon = CopyCursor(LoadCursor(NULL, IDC_ARROW));
        }

        if (!GetIconInfo(icon, &info)) 
		{
            goto icon_error;
        }

        pos.x = ci.ptScreenPos.x - clip_rect.left - info.xHotspot;
        pos.y = ci.ptScreenPos.y - clip_rect.top - info.yHotspot;

        if (hwnd) 
		{
            RECT rect;
            if (GetWindowRect(hwnd, &rect)) 
			{
                pos.x -= rect.left;
                pos.y -= rect.top;
            } else 
			{
                goto icon_error;
            }
        }


        if (pos.x >= 0 && pos.x <= clip_rect.right - clip_rect.left &&
                pos.y >= 0 && pos.y <= clip_rect.bottom - clip_rect.top) {
            if (!DrawIcon(is->Dest_Hdc, pos.x, pos.y, icon))
			{
				;//CURSOR_ERROR("Couldn't draw icon");
			}   
        }

icon_error:
        if (info.hbmMask)
            DeleteObject(info.hbmMask);
        if (info.hbmColor)
            DeleteObject(info.hbmColor);
        if (icon)
            DestroyCursor(icon);
    } else 
	{
        
    }
}
#endif
//图片队列 图片
int queue_picture(SKK_VideoState *is, AVFrame *pFrame, double pts,double duration, int64_t pos, int serial)
{  

	SKK_FrameQueue *pPictq=&is->pictq;
	time_t t_start, t_end;
	t_start = time(NULL) ;
	///***找到一个可用的SKK_Frame***/
	SKK_Frame *vp = frame_queue_peek_writable(pPictq);
	if(vp==NULL)
		return -1;
	
	if(is->abort_request)
		return -1;
	t_end = time(NULL) ;
    /*LOGE("Waittime:%f",difftime(t_end,t_start));
	char abcd[100]="";
	sprintf(abcd,"\n XXId：%f",difftime(t_end,t_start));
	::OutputDebugStringA(abcd);*/
	//锁
	//LOGE("queue_picture Lock \n");
	pPictq->mutex->Lock();
	vp->sar = pFrame->sample_aspect_ratio;
    vp->duration=duration;
	vp->pos=pos;
	vp->serial=serial;
	vp->pts=pts;
	if( !vp->buffer || 
		vp->width != pFrame->width ||
		vp->height != pFrame->height) 
	{
			vp->allocated = 0;
			vp->reallocate=0;
			vp->width = is->viddec.avctx->width;
			vp->height = is->viddec.avctx->height;
			vp->allocated = 1;
			
	}
    int w=pFrame->width;
	int h=pFrame->height;
#ifdef WIN32
	AVPixelFormat ff=AV_PIX_FMT_BGRA; //AVPixelFormat::AV_PIX_FMT_RGB24;//
#else
	AVPixelFormat ff=AV_PIX_FMT_RGBA;
#endif
	//LOGE("WindowWidth:%d,WindowHeight:%d pFrame->width:%d pFrame->height:%d\n",is->DestWidth,is->DestHeight,w,h);

	if(vp->buffer!=NULL)
	{	
		av_free(vp->buffer);
		vp->buffer=NULL;
	}
	if( pFrame->width>0&&pFrame->height>0)
	{
		/*1280×720   16:9
			1920×1080  16:9
			720×480    4:3*/
		//if(0)
#ifdef Android_PlatXXX
			if((pFrame->width==1280&&pFrame->height==720)||(pFrame->width==1920&&pFrame->height==1080))
			{
				//显示区域小鱼目标宽度
				if(is->DisplayWidth<pFrame->width)
				{
					float ff= (float)is->DisplayWidth/pFrame->width;
					is->DestWidth=pFrame->width*ff;
					is->DestHeight=pFrame->height*ff;
				}else
				{
                   is->DestWidth=pFrame->width;
                   is->DestHeight=pFrame->height;
				}
			}else if(pFrame->width<=720&&pFrame->height<=480)
			{
				//显示区域小鱼目标宽度
				if(is->DisplayWidth<pFrame->width)
				{
					float ff=(float) is->DisplayWidth/pFrame->width;
					is->DestWidth=pFrame->width*ff;
					is->DestHeight=pFrame->height*ff;
				}else
				{
					is->DestWidth=pFrame->width;
					is->DestHeight=pFrame->height;
				}
			}else
			{
				/*LOGE("No s,%d:%d",is->DisplayWidth,is->DestHeight);
				float ff=(float) is->DisplayWidth/pFrame->width;
				float ff1=(float) is->DestHeight/pFrame->height;*/
				/*is->DestWidth=pFrame->width*ff;
				is->DestHeight=pFrame->height*ff;*/
				is->DestWidth=pFrame->width;
				is->DestHeight=pFrame->height;
			}
#else
		is->DestWidth=pFrame->width;
		is->DestHeight=pFrame->height;
#endif
		
		w=is->DestWidth;
		h=is->DestHeight;
	}
	
	
	//time_t t_start, t_end;
	t_start = time(NULL) ;
	
	
    if(1)
	{
			is->img_convert_ctx = sws_getCachedContext(is->img_convert_ctx,
				pFrame->width,  pFrame->height , (PixelFormat)(pFrame->format),
				w,              h,                ff,                
				SWS_FAST_BILINEAR,
				NULL, NULL, NULL);
			if (is->img_convert_ctx == NULL) 
			{
				fprintf(stderr, "Cannot initialize the conversion context\n");
				exit(1);
			}
		    AVPicture pict = { { 0 } };
			int numBytes=avpicture_get_size(ff, w,h);
			uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
			avpicture_fill((AVPicture *)&pict, buffer,ff,  w, h);
			sws_scale(is->img_convert_ctx, pFrame->data, pFrame->linesize,
				0,pFrame->height, pict.data, pict.linesize);
            vp->buffer=buffer;
		    vp->width=is->DestWidth;
		    vp->height=is->DestHeight;
	}
   	pPictq->mutex->Unlock();

	t_end = time(NULL) ;
	
	//LOGE("cx time:%f",difftime(t_end,t_start));
	frame_queue_push(&is->pictq);
	return 0;
   
}  
//视频线程
unsigned __stdcall  Video_thread(LPVOID lpParameter)
{
	//LOGE("Video_thread start");
	SKK_VideoState *is=(SKK_VideoState*)lpParameter;
	AVPacket pkt1, *packet = &pkt1;  
	int len1, got_frame,ret;  
	AVFrame *pFrame=NULL;  
	double pts;  
	//double duration;
	//大概的速率
    AVRational frame_rate = av_guess_frame_rate(is->pFormatCtx, is->video_st, NULL);

	pFrame = avcodec_alloc_frame();  
    
	for(;;)  
	{
		    if(is->abort_request)
			{//
				//LOGE("Video_thread break");
                break;
			}
				
			//获取包
			//LOGE("Get video pkt");
			if(packet_queue_get(&is->videoq, packet, 1,&is->viddec.pkt_serial) < 0)  
			{  
			   // means we quit getting packets  
				Sleep(20);
				continue;
			}  
			//LOGE("Get video pkt Ok");
			pts = 0; 


			SKK_Decoder* d=&is->viddec;
			d->pts=packet->pts;
			d->dts=packet->dts;
			d->current_pts_time=av_gettime();
			
			time_t t_start, t_end;
			t_start = time(NULL) ;
			
			
			//视频解码
			ret = avcodec_decode_video2(d->avctx, pFrame, &got_frame, packet);
			
			t_end = time(NULL) ;
			//LOGE("de time:%f",difftime(t_end,t_start));
			
			// 
			//找到pts
			if((pts = av_frame_get_best_effort_timestamp(pFrame)) == AV_NOPTS_VALUE) 
			{
				pts = 0;
			}
			pts *= av_q2d(is->video_st->time_base);
            AVRational  fun={frame_rate.den, frame_rate.num};
			is->duration = (frame_rate.num && frame_rate.den ? av_q2d(fun) : 0);

		    //LOGE("Video_thread got_frame=%d",got_frame);
			// Did we get a video frame?  
			if(got_frame)  
			{  
				//pts = synchronize_video(is, pFrame, pts);  
				
				t_start = time(NULL) ;
				//LOGE("Get pic");
				if(queue_picture(is, pFrame, pts, is->duration , av_frame_get_pkt_pos(pFrame), is->viddec.pkt_serial) < 0)  
				{  
					//break;  
				}  
				t_end = time(NULL) ;
			//	LOGE("queue_picture time:%f",difftime(t_end,t_start));
				//LOGE("Get pic Ok");
				 av_frame_unref(pFrame);
			}
			
			
			av_free_packet(packet);  
	}
	av_frame_free(&pFrame);
	LOGE("Video_thread Over");

	is->viddec.decoder_tid.ThOver=true;
	return 0;
}

/* seek in the stream */
void stream_seek(SKK_VideoState *is, int64_t pos, int64_t rel, int seek_by_bytes)
{
	if (!is->seek_req) 
	{
		is->seek_pos = pos;
		is->seek_rel = rel;
		is->seek_flags &= ~AVSEEK_FLAG_BYTE;
		if (seek_by_bytes)
			is->seek_flags |= AVSEEK_FLAG_BYTE;
		is->seek_req = 1;
	}
}