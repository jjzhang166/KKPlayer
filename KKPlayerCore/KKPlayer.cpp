/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "stdafx.h"
#include "KKPlayer.h"
#include "KKInternal.h"

static AVPacket flush_pkt;
static int decoder_reorder_pts = -1;
static int framedrop = -1;
static int fast = 0;
static int lowres = 0;
static int64_t sws_flags = SWS_BICUBIC;
static int av_sync_type =AV_SYNC_AUDIO_MASTER;//AV_SYNC_VIDEO_MASTER;// AV_SYNC_AUDIO_MASTER;
double rdftspeed = 0.02;
KKPlayer::KKPlayer(IKKPlayUI* pPlayUI,IKKAudio* pSound):m_pSound(pSound),m_pPlayUI(pPlayUI), m_OpenMediaEnum(OpenMediaEnum::No)
{
//	assert(m_pPlayUI!=NULL);
	m_DisplayVp=NULL;
	pVideoInfo=NULL;
	m_bOpen=false;
	avdevice_register_all();
    av_register_all();
	avfilter_register_all();
	avformat_network_init();
	WindowWidth=0;
	WindowHeight=0;
	//avio_alloc_context
	start_time=AV_NOPTS_VALUE;
	m_CurTime=0;
}
void KKPlayer::CloseMedia()
{
    m_CloseLock.Lock();
    m_pSound->Stop();
	m_bOpen=false;
	if(pVideoInfo==NULL) 
		return;
		
	HANDLE wiatHandle[5]={m_ReadThreadInfo.ThreadHandel,m_VideoRefreshthreadInfo.ThreadHandel,pVideoInfo->viddec.decoder_tid.ThreadHandel,
		pVideoInfo->auddec.decoder_tid.ThreadHandel,pVideoInfo->subdec.decoder_tid.ThreadHandel};
	pVideoInfo->abort_request=1;
	::WaitForMultipleObjects(5,wiatHandle,TRUE,INFINITE);
	

	//SDL_CloseAudio();
	//关闭读取线程
	::TerminateThread(m_ReadThreadInfo.ThreadHandel,0);
	::CloseHandle(m_ReadThreadInfo.ThreadHandel);
    
	::TerminateThread(m_VideoRefreshthreadInfo.ThreadHandel,0);
	::CloseHandle(m_VideoRefreshthreadInfo.ThreadHandel);
	
	//关闭相关解码线程
	::TerminateThread(pVideoInfo->viddec.decoder_tid.ThreadHandel,0);
	::CloseHandle(pVideoInfo->viddec.decoder_tid.ThreadHandel);
	
	::TerminateThread(pVideoInfo->auddec.decoder_tid.ThreadHandel,0);
	::CloseHandle(pVideoInfo->auddec.decoder_tid.ThreadHandel);
	
	::TerminateThread(pVideoInfo->subdec.decoder_tid.ThreadHandel,0);
	::CloseHandle(pVideoInfo->subdec.decoder_tid.ThreadHandel);
	
	
    /*******事件*********/
	::CloseHandle(pVideoInfo->videoq.m_WaitEvent);
	//音频包
	::CloseHandle(pVideoInfo->audioq.m_WaitEvent);
	//字幕包
	::CloseHandle(pVideoInfo->subtitleq.m_WaitEvent);
	::CloseHandle(pVideoInfo->pictq.m_WaitEvent);
	::CloseHandle(pVideoInfo->subpq.m_WaitEvent);
	::CloseHandle(pVideoInfo->sampq.m_WaitEvent);
	PacketQueuefree();
	delete pVideoInfo->videoq.pLock;
	pVideoInfo->videoq.pLock=NULL;
	delete pVideoInfo->subtitleq.pLock;
	pVideoInfo->subtitleq.pLock=NULL;
	delete pVideoInfo->audioq.pLock;
	pVideoInfo->audioq.pLock=NULL;
	
	delete pVideoInfo->pictq.mutex;
	pVideoInfo->pictq.mutex=NULL;
	delete pVideoInfo->subpq.mutex;
	pVideoInfo->subpq.mutex=NULL;
	delete pVideoInfo->sampq.mutex;
	pVideoInfo->sampq.mutex=NULL;

	swr_free(&pVideoInfo->swr_ctx);
	pVideoInfo->swr_ctx=NULL;
	sws_freeContext(pVideoInfo->img_convert_ctx);
	pVideoInfo->img_convert_ctx=NULL;
	sws_freeContext(pVideoInfo->sub_convert_ctx);
	pVideoInfo->sub_convert_ctx=NULL;

	sws_freeContext(pVideoInfo->PicGrab_Img_convert_ctx);
	pVideoInfo->PicGrab_Img_convert_ctx=NULL;


	if(pVideoInfo->IsOutFile)
	{
		//Write file trailer  
		av_write_trailer(pVideoInfo->ofmt_ctx);  

		/* close output */  
		if (pVideoInfo->ofmt_ctx && !(pVideoInfo->ofmt->flags & AVFMT_NOFILE))  
			avio_close(pVideoInfo->ofmt_ctx->pb);  
		avformat_free_context(pVideoInfo->ofmt_ctx);  
	}

	//av_packet_unref(pVideoInfo->viddec.p)
	avcodec_close(pVideoInfo->viddec.avctx);
	avcodec_close(pVideoInfo->auddec.avctx);
	avcodec_close(pVideoInfo->subdec.avctx);

	avformat_free_context(pVideoInfo->pFormatCtx);
	av_free(pVideoInfo);
	pVideoInfo=NULL;

	start_time=AV_NOPTS_VALUE;
	m_CurTime=0;
	m_CloseLock.Unlock();
}
KKPlayer::~KKPlayer(void)
{
    
}

void KKPlayer::SetWindowHwnd(HWND hwnd)
{
	m_hwnd=hwnd;
	m_pSound->SetWindowHAND((int)m_hwnd);
	
}
void KKPlayer::InitSound()
{
   m_pSound->InitAudio();
}
unsigned __stdcall  ReadAV_thread(LPVOID lpParameter)
{
	KKPlayer *pPlayer=(KKPlayer *  )lpParameter;
	pPlayer->ReadAV();
	return 1;
}
int index=0;

SYSTEMTIME Time_tToSystemTime(time_t t)
{
	tm temptm = *localtime(&t);
	SYSTEMTIME st = {1900 + temptm.tm_year, 
		1 + temptm.tm_mon, 
		temptm.tm_wday, 
		temptm.tm_mday, 
		temptm.tm_hour, 
		temptm.tm_min, 
		temptm.tm_sec, 
		0};
	return st;
}
//视频刷新函数
void KKPlayer::video_image_refresh(SKK_VideoState *is)
{
	//没有数据
    if (frame_queue_nb_remaining(&is->pictq) <= 0)
	{
	   return;
	}
	SKK_Frame *vp;
	int redisplay=0;
	is->pictq.mutex->Lock();
	/**********获取包位置**********/
	vp = frame_queue_peek(&is->pictq);
	double time=0;
	if(vp!=NULL)
	{
		//获取上一次的读取位置
	    SKK_Frame *lastvp = frame_queue_peek_last(&is->pictq);
		/******上一次更新和这一次时间的差值。图片之间差值******/
	    is->last_duration = vp_duration(is, lastvp, vp);/******pts-pts********/
	    is->delay = compute_target_delay(is->last_duration, is);

		/*******时间**********/
		if (lastvp->serial != vp->serial && !redisplay)
		{
			is->frame_timer = av_gettime_relative() / 1000000.0;
		}

		//is->frame_timer += delay;
		time= av_gettime_relative()/1000000.0;
		if (is->delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
			is->frame_timer = time+is->delay;
		/******上一次更新和这一次时间的差值。图片之间差值******/
		double DiffCurrent=(is->frame_timer -is->vidclk.last_updated);
		if (vp->buffer)
		{
            if(!isNAN(vp->pts))
			{
				update_video_pts(is, vp->pts, vp->pos, vp->serial);
				if(start_time==AV_NOPTS_VALUE)
				{
                  start_time=vp->pts;
				}
				m_CurTime=vp->pts-start_time;
				if(DiffCurrent>=is->last_duration+is->delay || DiffCurrent<0.000000 || vp->pts<is->audio_clock )//||is->delay >60
				{
					frame_queue_next(&is->pictq);
				}
			}
			
		}
		
	}
	is->pictq.mutex->Unlock();
}


void KKPlayer::VideoRefresh()
{
	if(pVideoInfo==NULL)
		return;
	if(pVideoInfo->IsReady==0)
		return;
	if (pVideoInfo->paused)
		return;
	if (pVideoInfo->audio_st) 
	{
	
	}
	
	if (pVideoInfo->video_st) 
	{
		/*********/
		int redisplay = 0;
		video_image_refresh(pVideoInfo);
	}
}

void KKPlayer::OnDrawImageByDc(HDC memdc)
{
	//return;
	SKK_Frame *vp;
	if(pVideoInfo->IsReady==0)
		return;
	pVideoInfo->pictq.mutex->Lock();
	/**********获取包位置**********/
	vp = frame_queue_peek(&pVideoInfo->pictq);
   VideoDisplay(vp->buffer,vp->width,vp->height,&memdc,pVideoInfo->last_duration,vp->pts,vp->duration,vp->pos,pVideoInfo->delay);
   pVideoInfo->pictq.mutex->Unlock();
}
void KKPlayer::RenderImage(CRender *pRender)
{
	SKK_Frame *vp;
	if(pVideoInfo==NULL)
	{
		int len=0;
		unsigned char* pBkImage=m_pPlayUI->GetBkImage(len);
        pRender->renderBk(pBkImage,len);
		return;
	}else 
	{
		if(pVideoInfo->IsReady==0)
		{
			int len=0;
			unsigned char* pBkImage=m_pPlayUI->GetWaitImage(len,0);
			if(pBkImage!=NULL)
			{
                 pRender->SetWaitPic(pBkImage,len);
				 pBkImage=m_pPlayUI->GetBkImage(len);
				 pRender->SetBkImagePic(pBkImage,len);
				 pRender->render(NULL,0,0);
			}
			
		}else
		{
			m_CloseLock.Lock();
			if(!m_bOpen)
			{
               m_CloseLock.Unlock();
			   return;
			}
		      
			pVideoInfo->pictq.mutex->Lock();
			
			/**********获取包位置**********/
			vp = frame_queue_peek(&pVideoInfo->pictq);
			if(vp!=m_DisplayVp&&m_DisplayVp!=NULL)
			{
				//if(vp->width!=WindowWidth&&WindowHeight!=vp->height)
				//  pRender->resize(vp->width,vp->height);
				    pRender->render((char*)vp->buffer,vp->width,vp->height);
			}
			WindowWidth=vp->width;
			WindowHeight=vp->height;
			m_DisplayVp=vp;
			pVideoInfo->pictq.mutex->Unlock();
			m_CloseLock.Unlock();
		}
	}
	
}

void KKPlayer::AdjustDisplay(int w,int h)
{
     if(pVideoInfo!=NULL)
	 {
		 pVideoInfo->DisplayWidth=w;
		 pVideoInfo->DisplayHeight=h;
	 }
}
/******显示函数*****/
void KKPlayer::VideoDisplay(void *buf,int w,int h,void *usadata,double last_duration,double pts,double duration,int64_t pos,double diff)
{
	/***这里没有内存泄露***/
//	return;
	HDC dc=*(HDC*)usadata;
	BITMAPINFOHEADER header;
	header.biSize = sizeof(BITMAPINFOHEADER);

	header.biWidth = w;
	header.biHeight = h*(-1);
	header.biBitCount = 32;
	header.biCompression = 0;
	header.biSizeImage = 0;
	header.biClrImportant = 0;
	header.biClrUsed = 0;
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biPlanes = 1;


	/*HBRUSH 	m_SelectDotHbr=CreateSolidBrush(RGB(255, 0, 0));
	RECT rt={0,0,100,100};
	::FillRect(dc,&rt,m_SelectDotHbr);
	::DeleteObject(m_SelectDotHbr);*/
	{			
		//速率
		char t[256]="";
		sprintf(t, "VPdur:%f",duration);
		RECT rt2={w,10,w+150,30};
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		//PTS
		memset(t,0,256);
		sprintf(t, "VPpts:%f",pts);
		rt2.top=30;
		rt2.bottom=60;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		//pos
		memset(t,0,256);
		sprintf(t, "pos:%ld",pos);
		rt2.top=60;
		rt2.bottom=90;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		memset(t,0,256);
		int64_t timer=pVideoInfo->pFormatCtx->duration/1000;
		int h=(timer/(1000*60*60));
		int m=(timer%(1000*60*60))/(1000*60);
		int s=((timer%(1000*60*60))%(1000*60))/1000;
		sprintf(t, "timer:%d:%d:%d",h,m,s);
		rt2.top=90;
		rt2.bottom=120;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		memset(t,0,256);
		sprintf(t, "Vp差值:%f",last_duration);
		rt2.top=120;
		rt2.bottom=150;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		//PTS
		memset(t,0,256);
		int ll2=pts;
		h=(ll2/(60*60));
		m=(ll2%(60*60))/(60);
		s=((ll2%(60*60))%(60));
		sprintf(t, "timer:%d:%d:%d",h,m,s);
		rt2.top=150;
		rt2.bottom=180;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);
		
		//音频
		memset(t,0,256);
		sprintf(t, "audio:%.3f",pVideoInfo->audio_clock);
		rt2.top=180;
		rt2.bottom=210;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		//误差
		memset(t,0,256);
		sprintf(t, "V_Adif:%.3f",abs(pts-pVideoInfo->audio_clock));
		rt2.top=210;
		rt2.bottom=240;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		double V_ADiff=pts-pVideoInfo->audio_clock+diff;
		memset(t,0,256);
		sprintf(t, "视频快:%.3f",V_ADiff);
		
		rt2.top=240;
		rt2.bottom=270;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);

		memset(t,0,256);
		sprintf(t, "delay:%.3f",diff);
		rt2.top=270;
		rt2.bottom=300;
		::SetTextColor(dc,RGB(255,255,255));
		::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);
		if(buf==NULL)
		{
		   memset(t,0,256);
		   sprintf(t, "丢:%f",pts);
		   rt2.top=300;
		   rt2.bottom=330;
		   ::SetTextColor(dc,RGB(255,255,255));
		  ::DrawTextA(dc,t,strlen(t),&rt2,DT_CENTER);
		}
	}
	//拷贝图像
	StretchDIBits(dc, 0,   0, 
		w,   h, 
		0,   0, 
		w,   h, 
		buf, (BITMAPINFO*)&header,
		DIB_RGB_COLORS, SRCCOPY);/**/
	if(0)
	{
		//3 构造文件头
		BITMAPFILEHEADER bmpFileHeader;
		HANDLE hFile = NULL;
		DWORD dwTotalWriten = 0;
		DWORD dwWriten;

		bmpFileHeader.bfType = 0x4d42; //'BM';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
		bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+ w*h*24/8;

		char buf[256]="";
		sprintf_s(buf,255,"%s%d.bmp","D:/pic/",index++);  
		FILE* pf = fopen(buf, "wb");
		fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, pf);
		fwrite(&header, sizeof(BITMAPINFOHEADER), 1, pf);
		int numBytes=avpicture_get_size(PIX_FMT_RGB24, w,h);
		fwrite(buf, 1, numBytes, pf);
		fclose(pf);
	}
}
int KKPlayer::OpenMedia(std::string fileName,OpenMediaEnum en,std::string FilePath)
{
	m_StrFilePath=FilePath;
	this->m_OpenMediaEnum=en;
	m_bOpen=true;
	/*m_RtmpIns.SetupURL(fileName.c_str());
	int ll=m_RtmpIns.RtmpConnect();
	ll=m_RtmpIns.RtmpConnectStream(0);
	
		m_RtmpIns.ReadRtmpStream();*/
	
	//初始化SDL
	//int flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	//int flags = SDL_INIT_AUDIO ;//| SDL_INIT_TIMER;
	//if (SDL_Init (flags)) 
	//{
	//	av_log(NULL, AV_LOG_FATAL, "Could not initialize SDL - %s\n", SDL_GetError());
	//	av_log(NULL, AV_LOG_FATAL, "(Did you set the DISPLAY variable?)\n");
	//	exit(1);
	//}

	
	pVideoInfo = (SKK_VideoState*)av_mallocz(sizeof(SKK_VideoState));
	memset(pVideoInfo,0,sizeof(SKK_VideoState));
	pVideoInfo->pflush_pkt =(AVPacket*)av_mallocz(sizeof(AVPacket));

	RECT rt;
	::GetClientRect(m_hwnd,&rt);
	pVideoInfo->DisplayWidth=rt.right-rt.left;
	pVideoInfo->DisplayHeight=rt.bottom-rt.top;
	

	memset(pVideoInfo->pflush_pkt,0,sizeof(AVPacket));
	av_init_packet(pVideoInfo->pflush_pkt);
	flush_pkt.data = (uint8_t *)pVideoInfo->pflush_pkt;

	av_strlcpy(pVideoInfo->filename, fileName.c_str(), fileName.length()+1);

	//pVideoInfo->pFile=fopen("E:\\output.pcm", "wb");  
	//初始化队列
	packet_queue_init(&pVideoInfo->videoq);
	pVideoInfo->videoq.m_WaitEvent=::CreateEvent(NULL,TRUE,FALSE,NULL);
    //音频包
	packet_queue_init(&pVideoInfo->audioq);
	pVideoInfo->audioq.m_WaitEvent=::CreateEvent(NULL,TRUE,FALSE,NULL);
	//字幕包
	packet_queue_init(&pVideoInfo->subtitleq);
	pVideoInfo->subtitleq.m_WaitEvent=::CreateEvent(NULL,TRUE,FALSE,NULL);


	init_clock(&pVideoInfo->vidclk, &pVideoInfo->videoq.serial);
	init_clock(&pVideoInfo->audclk, &pVideoInfo->audioq.serial);
	init_clock(&pVideoInfo->extclk, &pVideoInfo->extclk.serial);

	pVideoInfo->video_stream=-1;
	pVideoInfo->audio_stream=-1;
	pVideoInfo->subtitle_stream=-1;

	pVideoInfo->audio_clock_serial = -1;
	pVideoInfo->av_sync_type = av_sync_type;

	/* start video display */
	if (frame_queue_init(&pVideoInfo->pictq, &pVideoInfo->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
	{

	}
	pVideoInfo->pictq.m_WaitEvent=::CreateEvent(NULL,TRUE,TRUE,NULL);

	if (frame_queue_init(&pVideoInfo->subpq, &pVideoInfo->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)
	{

	}
	pVideoInfo->subpq.m_WaitEvent=::CreateEvent(NULL,TRUE,TRUE,NULL);
	if (frame_queue_init(&pVideoInfo->sampq, &pVideoInfo->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
	{

	}
	pVideoInfo->sampq.m_WaitEvent=::CreateEvent(NULL,TRUE,TRUE,NULL);

	m_pSound->SetAudioCallBack(audio_callback2);
	m_pSound->SetUserData(pVideoInfo);
	pVideoInfo->pKKAudio=m_pSound;
	m_ReadThreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, ReadAV_thread, (LPVOID)this, 0,&m_ReadThreadInfo.Addr);
    UINT addrr;
	m_VideoRefreshthreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, VideoRefreshthread, (LPVOID)this, 0,&m_VideoRefreshthreadInfo.Addr);

	
	return 0;
}
/*********视频刷新线程********/
 unsigned WINAPI KKPlayer::VideoRefreshthread(LPVOID lpParameter)
 {
     KKPlayer* pPlayer=(KKPlayer*)lpParameter;
	 while(1)
	 {
		//av_usleep(2000);
		Sleep(15);
		if(pPlayer->pVideoInfo!=NULL)
		{
			if(pPlayer->pVideoInfo->abort_request==1)
				break;
            pPlayer->VideoRefresh();
		}
	 }
	 return 1;
 }
 int KKPlayer::GetCurTime()
 {

	 return m_CurTime;
 }

/*****读取视频信息******/
void KKPlayer::ReadAV()
{

	AVFormatContext *pFormatCtx= avformat_alloc_context();
	AVDictionary *format_opts=NULL;
	int err=-1;
	pVideoInfo->iformat=NULL;
	err = avformat_open_input(
		&pFormatCtx,                    pVideoInfo->filename,
		pVideoInfo->iformat,    &format_opts);
    AVPacket cpypkt;
	
    //文件打开失败
	if(err<0)
	{
		pVideoInfo->abort_request=1;
        if(m_pPlayUI!=NULL)
		{
			Sleep(1000);
			std::string URL(pVideoInfo->filename);
			m_pPlayUI->OpenMediaFailure(URL);
		}
		return;
		
	}
    
	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
		return; // Couldn't find stream information

	if(this->m_OpenMediaEnum==OpenMediaEnum::Dump)
	{
			    pVideoInfo->IsOutFile=1;
				char Outfile[256]="";
				strcpy(Outfile,m_StrFilePath.c_str());
			
				//打开输出文件
				if(pVideoInfo->IsOutFile)
				{
						av_dump_format(pFormatCtx, 0,  pVideoInfo->filename, 0);
						int Ret=avformat_alloc_output_context2(&pVideoInfo->ofmt_ctx, NULL, NULL,Outfile); 
						if (!pVideoInfo->ofmt_ctx)
						{  
							assert(0);
						}

						pVideoInfo->ofmt = pVideoInfo->ofmt_ctx->oformat;  
						for (int i = 0; i < pFormatCtx->nb_streams; i++)
						{
							//根据输入流创建输出流（Create output AVStream according to input AVStream）
							AVStream *in_stream = pFormatCtx->streams[i];
							AVStream *out_stream = avformat_new_stream( pVideoInfo->ofmt_ctx, in_stream->codec->codec);
							if (!out_stream)
							{
								assert(0);
								/*printf( "Failed allocating output stream\n");
								ret = AVERROR_UNKNOWN;
								goto end;*/
							}
							//复制AVCodecContext的设置（Copy the settings of AVCodecContext）
							if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) 
							{
								assert(0);
								/*printf( "Failed to copy context from input to output stream codec context\n");
								goto end;*/
							}
							out_stream->codec->codec_tag = 0;
							if ( pVideoInfo->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
								out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
							if(out_stream->codec->codec_type==AVMEDIA_TYPE_AUDIO)
							{
							   pVideoInfo->out_audios=out_stream;
							}else if(out_stream->codec->codec_type==AVMEDIA_TYPE_VIDEO)
							{
								 pVideoInfo->out_videos=out_stream;
							}
						}
						//输出一下格式------------------
						av_dump_format(pVideoInfo->ofmt_ctx, 0, Outfile, 1);
						//打开输出文件（Open output file）
						if (!(pVideoInfo->ofmt->flags & AVFMT_NOFILE)) 
						{
							int ret = avio_open(&pVideoInfo->ofmt_ctx->pb, Outfile, AVIO_FLAG_WRITE);
							if (ret < 0) 
							{
								assert(0);
								/*printf( "Could not open output file '%s'", out_filename);
								goto end;*/
							}
						}
						//写文件头（Write file header）
						if (avformat_write_header(pVideoInfo->ofmt_ctx, NULL) < 0) 
						{
							assert(0);
							/*printf( "Error occurred when opening output file\n");
							goto end;*/
						}
						
				}
	
	}


	pVideoInfo->pFormatCtx = pFormatCtx;
	pVideoInfo->PicGrabType=0;
	
	int  i, ret=-1;
	int st_index[AVMEDIA_TYPE_NB]={-1,-1,-1,-1,-1};
	AVPacket pkt1, *pkt = &pkt1;
	int64_t stream_start_time;
	int pkt_in_play_range = 0;
	AVDictionaryEntry *t;
	AVDictionary **opts=NULL;
	int orig_nb_streams;
	int64_t pkt_ts;
    int64_t duration= AV_NOPTS_VALUE;
 
	
	av_format_inject_global_side_data(pFormatCtx);

	if (start_time != AV_NOPTS_VALUE) 
	{
		int64_t timestamp;

		timestamp = start_time;
		/* add the stream start time */
		if (pFormatCtx->start_time != AV_NOPTS_VALUE)
			timestamp += pFormatCtx->start_time;
		//ret = avformat_seek_file(ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
		if (ret < 0) 
		{

		}
	}

	pVideoInfo->realtime = is_realtime(pFormatCtx);

	for (i = 0; i < pFormatCtx->nb_streams; i++) 
	{
		AVStream *st = pFormatCtx->streams[i];
		enum AVMediaType type = st->codec->codec_type;
		st_index[type] = i;
	}

	/* open the streams */
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) 
	{
		stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) 
	{
		ret = stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_VIDEO]);
	}

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) 
	{
		stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}

	if(pVideoInfo->iformat==NULL)
	{
		pVideoInfo->iformat=pVideoInfo->pFormatCtx->iformat;
	}
	pVideoInfo->max_frame_duration = (pVideoInfo->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;
	pVideoInfo->IsReady=1;	
	
	pVideoInfo->m_nLiveType=1;
    strcpy(pVideoInfo->PushURL,"rtmp://192.9.8.239/live/xxxx");
	if(pVideoInfo->m_nLiveType==1)
	{
		avformat_alloc_output_context2(& pVideoInfo->PushOfmt_ctx, NULL, "flv", pVideoInfo->PushURL); //RTMP  
		for (int i = 0; i < pVideoInfo->pFormatCtx->nb_streams; i++) 
		{  
			//根据输入流创建输出流（Create output AVStream according to input AVStream）  
			AVStream *in_stream = pVideoInfo->pFormatCtx->streams[i];  
			AVStream *out_stream = avformat_new_stream( pVideoInfo->PushOfmt_ctx, in_stream->codec->codec);  
			if (!out_stream) {  
				printf( "Failed allocating output stream\n");  
				ret = AVERROR_UNKNOWN;  
				//	goto end;  
			}  
			//复制AVCodecContext的设置（Copy the settings of AVCodecContext）  
			ret = avcodec_copy_context(out_stream->codec, in_stream->codec);  
			if (ret < 0) 
			{  
				//			printf( "Failed to copy context from input to output stream codec context\n");  

			}  
			out_stream->codec->codec_tag = 0;  
			if ( pVideoInfo->PushOfmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)  
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;  
		}  
		//Dump Format------------------  
		av_dump_format(pVideoInfo->PushOfmt_ctx, 0,  pVideoInfo->PushURL, 1);  
		//打开输出URL（Open output URL）  
		if (!( pVideoInfo->PushOfmt_ctx->oformat ->flags & AVFMT_NOFILE))
		{  
			ret = avio_open(& pVideoInfo->PushOfmt_ctx->pb,  pVideoInfo->PushURL, AVIO_FLAG_WRITE);  
			if (ret < 0) {  
				printf( "Could not open output URL '%s'",  pVideoInfo->PushURL);  
				//goto end;  
			}  
		} 
		//写文件头（Write file header）  
		ret = avformat_write_header( pVideoInfo->PushOfmt_ctx, NULL);  
		if (ret < 0)
		{  
			printf( "Error occurred when opening output URL\n");  
		} 
	}

    pVideoInfo->PicGrabType=0;
	/****************以下windows 录屏专用************************/
	if(pVideoInfo->PicGrabType==1)
	{
		pVideoInfo->fp_out= fopen("C:\\kktest.h264", "wb");  
		//H264 编码器设定
		{
			pVideoInfo->pH264Codec=avcodec_find_encoder(AV_CODEC_ID_H264); 
			pVideoInfo->pH264CodecCtx=avcodec_alloc_context3(pVideoInfo->pH264Codec);  

			//预先设定一下
			pVideoInfo->pH264CodecCtx->bit_rate = pVideoInfo->viddec.avctx->bit_rate;
			pVideoInfo->pH264CodecCtx->width =500;  
			pVideoInfo->pH264CodecCtx->height = 298;  
			pVideoInfo->pH264CodecCtx->time_base=pVideoInfo->viddec.avctx->time_base;    
			pVideoInfo->pH264CodecCtx->gop_size = pVideoInfo->viddec.avctx->gop_size;
			pVideoInfo->pH264CodecCtx->max_b_frames = pVideoInfo->viddec.avctx->max_b_frames;
			pVideoInfo->pH264CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

			//			if (codec_id == AV_CODEC_ID_H264)  
			av_opt_set(pVideoInfo->pH264CodecCtx->priv_data, "preset", "slow", 0);  

			if (avcodec_open2(pVideoInfo->pH264CodecCtx, pVideoInfo->pH264Codec, NULL) < 0) {  
				printf("Could not open codec\n");  
				assert(0);
			}  
		}


		pVideoInfo->Source_Hdc=GetDC(NULL);
		pVideoInfo->DevBpp=GetDeviceCaps(pVideoInfo->Source_Hdc, BITSPIXEL);

		int vertres = GetDeviceCaps(pVideoInfo->Source_Hdc, VERTRES);
		int desktopvertres = GetDeviceCaps(pVideoInfo->Source_Hdc, DESKTOPVERTRES);
		pVideoInfo->virtual_rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		pVideoInfo->virtual_rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		pVideoInfo->virtual_rect.right = (pVideoInfo->virtual_rect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN)) * desktopvertres / vertres;
		pVideoInfo->virtual_rect.bottom = (pVideoInfo->virtual_rect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN)) * desktopvertres / vertres;

		pVideoInfo->Dest_Hdc= CreateCompatibleDC(pVideoInfo->Source_Hdc);

		BITMAPINFO bmi;
		HBITMAP hbmp   = NULL;
		BITMAP bmp;
		/* Create a DIB and select it into the dest_hdc */
		bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth         = pVideoInfo->virtual_rect.right - pVideoInfo->virtual_rect.left;
		bmi.bmiHeader.biHeight        = -(pVideoInfo->virtual_rect.bottom - pVideoInfo->virtual_rect.top);
		bmi.bmiHeader.biPlanes        = 1;
		bmi.bmiHeader.biBitCount      = pVideoInfo-> DevBpp;
		bmi.bmiHeader.biCompression   = BI_RGB;
		bmi.bmiHeader.biSizeImage     = 0;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed       = 0;
		bmi.bmiHeader.biClrImportant  = 0;
		hbmp = CreateDIBSection(pVideoInfo->Source_Hdc, &bmi, DIB_RGB_COLORS,
			&pVideoInfo->PicGrabBuf, NULL, 0);
		if (!hbmp) 
		{

		}

		if (!SelectObject(pVideoInfo->Dest_Hdc, hbmp)) {

		}

		/* Get info from the bitmap */
		GetObject(hbmp, sizeof(BITMAP), &bmp);

		pVideoInfo->frame_size  = bmp.bmWidthBytes * bmp.bmHeight * bmp.bmPlanes;
		pVideoInfo->header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			(pVideoInfo->DevBpp <= 8 ? (1 << pVideoInfo->DevBpp) : 0) * sizeof(RGBQUAD) /* palette size */;


		pVideoInfo->bmi=bmi;
		pVideoInfo->hbmp=hbmp;
		pVideoInfo->bmp=bmp;
	}

    if(pVideoInfo->m_nLiveType=1)
	{
		unsigned int addrr=0;
		_beginthreadex(NULL, NULL, PushStream, (LPVOID)this, 0,&addrr);
	}
	AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb"); 
	for (;;) 
	{
		
		if (pVideoInfo->abort_request)
		{
			break;
		}
		/********************实时流媒体不支持暂停******************************/
		if (pVideoInfo->paused != pVideoInfo->last_paused&&!pVideoInfo->realtime) 
		{
			pVideoInfo->last_paused = pVideoInfo->paused;
			if (pVideoInfo->paused)
			{
				pVideoInfo->read_pause_return = av_read_pause(pFormatCtx);
				Sleep(1);
			}
			else
			{
				av_read_play(pFormatCtx);
			}
		}
        /******快进*******/
		if (pVideoInfo->seek_req&&!pVideoInfo->realtime)
		{
			int64_t seek_target = pVideoInfo->seek_pos;
			int64_t seek_min    = pVideoInfo->seek_rel > 0 ? seek_target - pVideoInfo->seek_rel + 2: INT64_MIN;
			int64_t seek_max    = pVideoInfo->seek_rel < 0 ? seek_target - pVideoInfo->seek_rel - 2: INT64_MAX;
			ret = avformat_seek_file(pVideoInfo->pFormatCtx, -1, seek_min, seek_target, seek_max, pVideoInfo->seek_flags);
			if (ret < 0) 
			{
			     assert(0);
				//失败
			}else
			{
				if (pVideoInfo->audio_stream >= 0) 
				{
					packet_queue_flush(&pVideoInfo->audioq);
					packet_queue_put(&pVideoInfo->audioq,pVideoInfo->pflush_pkt, &flush_pkt);
				}
				if (pVideoInfo->subtitle_stream >= 0) 
				{
					packet_queue_flush(&pVideoInfo->subtitleq);
					packet_queue_put(&pVideoInfo->subtitleq, pVideoInfo->pflush_pkt,&flush_pkt);
				}
				if (pVideoInfo->video_stream >= 0) 
				{
					packet_queue_flush(&pVideoInfo->videoq);
					packet_queue_put(&pVideoInfo->videoq, pVideoInfo->pflush_pkt,&flush_pkt);
					pVideoInfo->pictq.mutex->Lock();
					for(int i=0;i<pVideoInfo->pictq.max_size;i++)
					{
                          SKK_Frame *p=& pVideoInfo->pictq.queue[i];
                          /*******这里不释放内存是为了保证图像连续********/						 
						  if(p->buffer!=NULL)
						  {
							 // av_free(p->buffer);
							 //  p->buffer=NULL;
						  }
					}
                    pVideoInfo->pictq.size=0;
					pVideoInfo->pictq.rindex=0;
					pVideoInfo->pictq.windex=0;
					pVideoInfo->pictq.rindex_shown=1;
                    SetEvent(pVideoInfo->pictq.m_WaitEvent);
					pVideoInfo->pictq.mutex->Unlock();
				}
				if (pVideoInfo->seek_flags & AVSEEK_FLAG_BYTE) 
				{
					set_clock(&pVideoInfo->extclk, NAN, 0);
				} else 
				{
					set_clock(&pVideoInfo->extclk, seek_target / (double)AV_TIME_BASE, 0);
				}
			}
			int ii=0;
			ii++;
			pVideoInfo->seek_req=0;
		}

		/******缓存满了*******/
		while(1)
		{
			if(pVideoInfo->audioq.size + pVideoInfo->videoq.size + pVideoInfo->subtitleq.size > MAX_QUEUE_SIZE)
			{
				//等待一会
				Sleep(30);
			}else
			{
				break;
			}
		}
        /********读取一个pkt**********/
		ret = av_read_frame(pFormatCtx, pkt);
		if (ret < 0) 
		{
			 if ((ret == AVERROR_EOF || avio_feof(pFormatCtx->pb)) && !pVideoInfo->eof) 
			 {
				    
                    pVideoInfo->eof=1;
					break;
			 }
			if (pFormatCtx->pb && pFormatCtx->pb->error)
				break;
			Sleep(1);
			continue;
		} else 
		{
			pVideoInfo->eof = 0;
		}
		if(pVideoInfo->IsOutFile)//Write 
		{
		//av_new_packet(&cpypkt,pkt->size);
		av_copy_packet(&cpypkt,pkt);
		}
		

		/* check if packet is in play range specified by user, then queue, otherwise discard */
		stream_start_time = pFormatCtx->streams[pkt->stream_index]->start_time;
		
		INT64 xx=(INT64)pkt->dts ;
		xx=AV_NOPTS_VALUE;
		pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
		
		int64_t  a1=(pkt_ts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0));
		int64_t a2= av_q2d(pFormatCtx->streams[pkt->stream_index]->time_base);
		pkt_in_play_range = duration == AV_NOPTS_VALUE ||
			a1 * a2-(double)(start_time != AV_NOPTS_VALUE ? start_time : 0) / 1000000 <= ((double)duration / 1000000);
		pkt_in_play_range =1;


		if(pVideoInfo->m_nLiveType==1)
		{
			AVPacket *pRtmpPkt =(AVPacket *)av_malloc(sizeof(AVPacket));
			av_copy_packet(pRtmpPkt,pkt);

			pRtmpPkt->pts = av_rescale_q_rnd(pRtmpPkt->pts, pFormatCtx->streams[pkt->stream_index]->time_base, pVideoInfo->PushOfmt_ctx->streams[pkt->stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
			pRtmpPkt->dts = av_rescale_q_rnd(pRtmpPkt->dts,pFormatCtx->streams[pkt->stream_index]->time_base,  pVideoInfo->PushOfmt_ctx->streams[pkt->stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
			pRtmpPkt->duration = av_rescale_q(pRtmpPkt->duration,pFormatCtx->streams[pkt->stream_index]->time_base,  pVideoInfo->PushOfmt_ctx->streams[pkt->stream_index]->time_base);

			
			m_PushStreamLock.Lock();
			m_PushPktQue.push(pRtmpPkt);
			m_PushStreamLock.Unlock();
		}
		//音频
		if (pkt->stream_index == pVideoInfo->audio_stream && pkt_in_play_range) 
		{
			packet_queue_put(&pVideoInfo->audioq, pkt,pVideoInfo->pflush_pkt);
			if(pVideoInfo->IsOutFile)//Write 
			{
				cpypkt.pts = av_rescale_q_rnd(cpypkt.pts, pFormatCtx->streams[pkt->stream_index]->time_base,pVideoInfo->out_audios->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
				cpypkt.dts = av_rescale_q_rnd(cpypkt.dts, pFormatCtx->streams[pkt->stream_index]->time_base, pVideoInfo->out_audios->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
				cpypkt.duration = av_rescale_q(cpypkt.duration, pFormatCtx->streams[pkt->stream_index]->time_base, pVideoInfo->out_audios->time_base);
				if (av_interleaved_write_frame(pVideoInfo->ofmt_ctx, &cpypkt) < 0)
				{  
					assert(0);
					break;
				}			
			}
		} //视频
		else if (
			pkt->stream_index == pVideoInfo->video_stream && pkt_in_play_range
			&& !(pVideoInfo->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC)
			) 
		{
			/********无内存泄露*******/
			packet_queue_put(&pVideoInfo->videoq, pkt,pVideoInfo->pflush_pkt);
			if(pVideoInfo->IsOutFile)//Write 
			{
				cpypkt.pts = av_rescale_q_rnd(cpypkt.pts, pFormatCtx->streams[pkt->stream_index]->time_base,pVideoInfo->out_videos->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
				cpypkt.dts = av_rescale_q_rnd(cpypkt.dts, pFormatCtx->streams[pkt->stream_index]->time_base, pVideoInfo->out_videos->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
				cpypkt.duration = av_rescale_q(cpypkt.duration, pFormatCtx->streams[pkt->stream_index]->time_base, pVideoInfo->out_videos->time_base);
				//cpypkt.pos = -1;
				if (av_interleaved_write_frame(pVideoInfo->ofmt_ctx, &cpypkt) < 0)
				{  
					assert(0);
					break;
				}
			}
		}//字幕
		else if (pkt->stream_index == pVideoInfo->subtitle_stream && pkt_in_play_range) 
		{
			//printf("subtitleq\n");
			packet_queue_put(&pVideoInfo->subtitleq, pkt,pVideoInfo->pflush_pkt);
		} else
		{
			av_free_packet(pkt);
		}
		if(pVideoInfo->IsOutFile)//Write 
		{	
		    av_free_packet(&cpypkt);
		}
	}
}
void KKPlayer::PacketQueuefree()
{
   if(pVideoInfo!=NULL)
   {
	    packet_queue_flush(&pVideoInfo->videoq);
		packet_queue_flush(&pVideoInfo->audioq);
		packet_queue_flush(&pVideoInfo->subtitleq);


        frame_queue_destory(&pVideoInfo->pictq);
		frame_queue_destory(&pVideoInfo->subpq);
		frame_queue_destory(&pVideoInfo->sampq);
   }
}
void KKPlayer::SetVolume(long value)
{
   m_pSound->SetVolume(value);
}
long KKPlayer::GetVolume()
{
	return m_pSound->GetVolume();
}
void KKPlayer::Pause()
{
    if(pVideoInfo->paused==1)
		pVideoInfo->paused=0;
	else
		pVideoInfo->paused=1;
}
void KKPlayer::KKSeek( SeekEnum en,int value)
{
   double incr, pos, frac;
   incr=value;
   pos = get_master_clock(pVideoInfo);
   if (isNAN(pos))
	   pos = (double)pVideoInfo->seek_pos / AV_TIME_BASE;
   pos += incr;
   if (pVideoInfo->pFormatCtx->start_time != AV_NOPTS_VALUE && pos < pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE)
	   pos = pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE;
   stream_seek(pVideoInfo, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
}

unsigned WINAPI KKPlayer::PushStream(LPVOID lpParameter)
{
	//return 1;
     KKPlayer* pPlayer=(KKPlayer*)lpParameter;
	pPlayer->VideoPushStream();
	return 0;
}

//推流
void KKPlayer::VideoPushStream()
{
	int ret=0;
	int64_t xstart_time=av_gettime();  
	while(1)
	{
		    if(pVideoInfo->abort_request)
			          break;
			while(m_PushPktQue.size()>0)
			{

				m_PushStreamLock.Lock();
				AVPacket *pkt=m_PushPktQue.front();
				m_PushPktQue.pop();
				m_PushStreamLock.Unlock();
				
				//Important:Delay
				{
					AVRational time_base=pVideoInfo->PushOfmt_ctx->streams[pkt->stream_index]->time_base;
					AVRational time_base_q={1,AV_TIME_BASE};
					int64_t pts_time = av_rescale_q(pkt->dts, time_base, time_base_q);
					int64_t now_time = av_gettime() - xstart_time;
					if (pts_time > now_time)
					{
						av_usleep(pts_time - now_time);
						if(pVideoInfo->abort_request)
							break;
					}
				}
			

				ret = av_interleaved_write_frame(pVideoInfo->PushOfmt_ctx, pkt);  
				if (ret < 0)
				{  
					printf( "Error muxing packet\n");  
					break;  
				}  
				av_free_packet(pkt);
				av_free(pkt);
			}
	}

}