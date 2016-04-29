/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/

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
KKPlayer::KKPlayer(IKKPlayUI* pPlayUI,IKKAudio* pSound):m_pSound(pSound),m_pPlayUI(pPlayUI), m_OpenMediaEnum(No)
{
//	assert(m_pPlayUI!=NULL);
	m_DisplayVp=NULL;
	pVideoInfo=NULL;
	m_bOpen=false;
	static bool registerFF=true;
	if(registerFF)
	{
		
		avdevice_register_all();
		av_register_all();
		avfilter_register_all();
		avformat_network_init();
		registerFF=false;
	}
	
	AVInputFormat *ff=av_iformat_next(NULL);
	//LOGE("AVInputFormatList \n");
	int i=0;
	while(ff!=NULL)
	{
		const char *aa=ff->name;
		//LOGE("%d,%s \n",i++,aa);
		ff=av_iformat_next(ff);
	}
	//LOGE("\n");
	WindowWidth=0;
	WindowHeight=0;
	//avio_alloc_context
	start_time=AV_NOPTS_VALUE;
	m_CurTime=0;
}
void KKPlayer::CloseMedia()
{
    m_CloseLock.Lock();
	if(!m_bOpen)
	{
		m_CloseLock.Unlock();
		return;
	}
    m_pSound->Stop();
	m_bOpen=false;
	if(pVideoInfo==NULL) 
	{
		m_CloseLock.Unlock();
		return;
	}

	while(1)
	{
		if(
			m_ReadThreadInfo.ThOver==true&&
			m_VideoRefreshthreadInfo.ThOver==true
			)
		{
			break;
		}
		LOGE("thread Over1 m_ReadThreadInfo%d,m_VideoRefreshthreadInfo%d",m_ReadThreadInfo.ThOver
			,m_VideoRefreshthreadInfo.ThOver
			);
		Sleep(100);
	}


	LOGE("thread Over 1");
	pVideoInfo->abort_request=1;
	pVideoInfo->pictq.m_pWaitCond->SetCond();
	pVideoInfo->audioq.m_pWaitCond->SetCond();
	pVideoInfo->subpq.m_pWaitCond->SetCond();/**/

	#ifndef WIN32_KK
			pthread_join(m_ReadThreadInfo.Tid_task,0);
			pthread_join(m_VideoRefreshthreadInfo.Tid_task,0);
	#endif	



	if(pVideoInfo->IsReady!=0)
	{
		while(1)
		{
			if(
				pVideoInfo->viddec.decoder_tid.ThOver==true&&
				pVideoInfo->auddec.decoder_tid.ThOver==true&&
				pVideoInfo->subdec.decoder_tid.ThOver==true
				)
			{
				break;
			}
			LOGE("thread Over2 viddec%d,auddec%d,subdec%d",pVideoInfo->viddec.decoder_tid.ThOver
				,pVideoInfo->auddec.decoder_tid.ThOver
				,pVideoInfo->subdec.decoder_tid.ThOver
				);
			Sleep(100);
		}
	}

	LOGE("thread Over 2");
	
Sleep(100);
#ifdef WIN32_KK
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
#endif	
	
    /*******事件*********/
	delete pVideoInfo->videoq.m_pWaitCond;
	//音频包
	delete pVideoInfo->audioq.m_pWaitCond;
	//字幕包
	delete pVideoInfo->subtitleq.m_pWaitCond;
	delete pVideoInfo->pictq.m_pWaitCond;
	delete pVideoInfo->subpq.m_pWaitCond;
	delete pVideoInfo->sampq.m_pWaitCond;
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

	LOGE("thread Over Ok");
}

MEDIA_INFO KKPlayer::GetMediaInfo()
{
	MEDIA_INFO info;
	memset(&info,0,sizeof(info));
	m_CloseLock.Lock();
	if(m_bOpen)
	{
		info.Open=m_bOpen;
		if(pVideoInfo!=NULL)
		{

			{
				
				info.CurTime=m_CurTime;

			}
			if(pVideoInfo->pFormatCtx!=NULL)
			{
				info.TotalTime=(pVideoInfo->pFormatCtx->duration/1000/1000);
				
			}
           
		}else{
           info.CurTime=0;
		}
		
	}else
	{
          info.Open=false;
	}
	m_CloseLock.Unlock();
	LOGE("MediaInfo:%f,%f \n",info.CurTime,info.TotalTime);
	return info;
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
unsigned __stdcall  KKPlayer::ReadAV_thread(LPVOID lpParameter)
{
	KKPlayer *pPlayer=(KKPlayer *  )lpParameter;
	pPlayer->m_ReadThreadInfo.ThOver=false;
	pPlayer->ReadAV();
	pPlayer->m_ReadThreadInfo.ThOver=true;
	return 1;
}



#ifdef WIN32_KK
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
#endif
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
					frame_queue_next(&is->pictq,false);
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
	
	}else
	{

	}
	if (pVideoInfo->video_st) 
	{
		int redisplay = 0;
		video_image_refresh(pVideoInfo);
	}
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
		//LOGE("pVideoInfo->IsReady=%d m_bOpen=%d\n",pVideoInfo->IsReady,m_bOpen);
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
					//LOGE("WindowWidth=vp->width:%d,WindowHeight=vp->height:%d \n",vp->width,vp->height);
			}else{
					//LOGE("no vp");
			}
			//LOGE("WindowWidth=vp->width:%d,WindowHeight=vp->height:%d \n",vp->width,vp->height);
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
#ifdef WIN32_KK
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
void KKPlayer::VideoDisplay(void *buf,int w,int h,void *usadata,double last_duration,double pts,double duration,int64_t pos,double diff)
{
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

#endif
//unsigned __stdcall VideoRefreshthread(LPVOID lpParameter);  
int KKPlayer::OpenMedia(char* fileName,OpenMediaEnum en,char* FilePath)
{
	m_CloseLock.Lock();
    if(m_bOpen)
	{
        m_CloseLock.Unlock();
		return -1;
	}
	m_bOpen=true;
	m_CloseLock.Unlock();

	int lenstr=strlen(FilePath);
	m_pStrFilePath=(char*)::malloc(lenstr+1);
	memset(m_pStrFilePath,0,lenstr+1);
	strcpy(m_pStrFilePath,FilePath);
	
	this->m_OpenMediaEnum=en;
	
	pVideoInfo = (SKK_VideoState*)av_mallocz(sizeof(SKK_VideoState));
	memset(pVideoInfo,0,sizeof(SKK_VideoState));
	pVideoInfo->pflush_pkt =(AVPacket*)av_mallocz(sizeof(AVPacket));


	LOGE("Movie Path：\n");
	LOGE(fileName);
	LOGE("\n");
	pVideoInfo->viddec.decoder_tid.ThOver=true;
	pVideoInfo->auddec.decoder_tid.ThOver=true;
	pVideoInfo->subdec.decoder_tid.ThOver=true;
	m_ReadThreadInfo.ThOver=true;
	m_VideoRefreshthreadInfo.ThOver=true;

	RECT rt;
	::GetClientRect(m_hwnd,&rt);
	pVideoInfo->DisplayWidth=100;//rt.right-rt.left;
	pVideoInfo->DisplayHeight=100;//rt.bottom-rt.top;
	


	memset(pVideoInfo->pflush_pkt,0,sizeof(AVPacket));
	av_init_packet(pVideoInfo->pflush_pkt);
	flush_pkt.data = (uint8_t *)pVideoInfo->pflush_pkt;

	av_strlcpy(pVideoInfo->filename, fileName, strlen(fileName)+1);

	//pVideoInfo->pFile=fopen("E:\\output.pcm", "wb");  
	//初始化队列
	packet_queue_init(&pVideoInfo->videoq);
	pVideoInfo->videoq.m_pWaitCond=new CKKCond_t();
	//pVideoInfo->videoq.m_pWaitCond->SetCond();
    //音频包
	packet_queue_init(&pVideoInfo->audioq);
	pVideoInfo->audioq.m_pWaitCond=new CKKCond_t();
	//pVideoInfo->audioq.m_pWaitCond->SetCond();
	//字幕包
	packet_queue_init(&pVideoInfo->subtitleq);
	pVideoInfo->subtitleq.m_pWaitCond=new CKKCond_t();
	//pVideoInfo->subtitleq.m_pWaitCond->SetCond();


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
	pVideoInfo->pictq.m_pWaitCond=new CKKCond_t();
    pVideoInfo->pictq.m_pWaitCond->SetCond();
	if (frame_queue_init(&pVideoInfo->subpq, &pVideoInfo->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)
	{

	}
	pVideoInfo->subpq.m_pWaitCond=new CKKCond_t();
	pVideoInfo->pictq.m_pWaitCond->SetCond();
	if (frame_queue_init(&pVideoInfo->sampq, &pVideoInfo->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
	{

	}
	pVideoInfo->sampq.m_pWaitCond=new CKKCond_t();
    pVideoInfo->pictq.m_pWaitCond->SetCond();
	m_pSound->SetAudioCallBack(audio_callback2);
	m_pSound->SetUserData(pVideoInfo);
	pVideoInfo->pKKAudio=m_pSound;

	m_ReadThreadInfo.ThOver=false;
	m_VideoRefreshthreadInfo.ThOver=false;
#ifdef WIN32_KK
	m_ReadThreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, ReadAV_thread, (LPVOID)this, 0,&m_ReadThreadInfo.Addr);
	m_VideoRefreshthreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, VideoRefreshthread, (LPVOID)this, 0,&m_VideoRefreshthreadInfo.Addr);
#else

	

	m_ReadThreadInfo.Addr = pthread_create(&m_ReadThreadInfo.Tid_task, NULL, (void* (*)(void*))ReadAV_thread, (LPVOID)this);
	LOGE("m_ReadThreadInfo.Addr =%d\n",m_ReadThreadInfo.Addr);
	 LOGE("VideoRefreshthread XX");
	m_VideoRefreshthreadInfo.Addr = pthread_create(&m_VideoRefreshthreadInfo.Tid_task, NULL, (void* (*)(void*))VideoRefreshthread, (LPVOID)this);
#endif
	return 0;
}
/*********视频刷新线程********/
 unsigned __stdcall KKPlayer::VideoRefreshthread(LPVOID lpParameter)
 {
	 LOGE("VideoRefreshthread strat");
     KKPlayer* pPlayer=(KKPlayer*)lpParameter;
	 pPlayer->m_VideoRefreshthreadInfo.ThOver=false;
	 while(pPlayer->m_bOpen)
	 {
		
		//av_usleep(2000);
		Sleep(10);
		if(pPlayer->pVideoInfo!=NULL)
		{
			if(pPlayer->pVideoInfo->abort_request==1)
				break;

			//time_t t_start, t_end;
			//t_start = time(NULL) ;
            pPlayer->VideoRefresh();
			//t_end = time(NULL) ;
			//char abcd[100]="";
			//sprintf(abcd,"\n 刷新时间：%f",difftime(t_end,t_start));
			//::OutputDebugStringA(abcd);
		}
	 }
	 pPlayer->m_VideoRefreshthreadInfo.ThOver=true;
	 LOGE("VideoRefreshthread over");
	 return 1;
 }
 int KKPlayer::GetCurTime()
 {

	 return m_CurTime;
 }

 static int decode_interrupt_cb(void *ctx)
 {
	 SKK_VideoState *is =(SKK_VideoState *) ctx;
	 return is->abort_request;
 }
/*****读取视频信息******/
void KKPlayer::ReadAV()
{
	m_ReadThreadInfo.ThOver=false;

	LOGE("ReadAV thread start");
	AVFormatContext *pFormatCtx= avformat_alloc_context();
	AVDictionary *format_opts=NULL;
	int err=-1;
	int scan_all_pmts_set = 0;
	pVideoInfo->iformat=NULL;
	
	//av_dict_set(&format_opts, "timeout", "6", 0);


	pFormatCtx->interrupt_callback.callback = decode_interrupt_cb;
	pFormatCtx->interrupt_callback.opaque = pVideoInfo;

	if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
		av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		scan_all_pmts_set = 1;
	}
	//此函数是阻塞的
	err = avformat_open_input(
		&pFormatCtx,                    pVideoInfo->filename,
		pVideoInfo->iformat,    &format_opts);
    AVPacket cpypkt;
	
	LOGE("avformat_open_input=%d,%s \n",err,pVideoInfo->filename);
    //文件打开失败
	if(err<0)
	{
		
		pVideoInfo->abort_request=1;
        if(m_pPlayUI!=NULL)
		{
			Sleep(1000);
			m_pPlayUI->OpenMediaFailure(pVideoInfo->filename);
		}else
		{
		     LOGE("m_pPlayUI=NULL \n");
		}
		return;
		
	}
    
	if (scan_all_pmts_set)
		av_dict_set(&format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
		LOGE("avformat_find_stream_info<0 \n");
		return; // Couldn't find stream information
    }
	if(this->m_OpenMediaEnum==Dump)
	{
			    
	}


	pVideoInfo->pFormatCtx = pFormatCtx;
	
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
		LOGE("AVMEDIA_TYPE_AUDIO");
		stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) 
	{
		LOGE("AVMEDIA_TYPE_VIDEO");
		ret = stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_VIDEO]);
	}

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) 
	{
		LOGE("AVMEDIA_TYPE_SUBTITLE");
		stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}

	if(pVideoInfo->iformat==NULL)
	{
		pVideoInfo->iformat=pVideoInfo->pFormatCtx->iformat;
	}
	pVideoInfo->max_frame_duration = (pVideoInfo->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;
	pVideoInfo->IsReady=1;	
	
	pVideoInfo->m_nLiveType=0;
	AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb"); 


	AVAES bKey;
	uint8_t rkey[16] ={ 0x10, 0xa5, 0x88, 0x69, 0xd7, 0x4b, 0xe5, 0xa3,0x74, 0xcf, 0x86, 0x7c, 0xfb, 0x47, 0x38, 0x59 };
	av_aes_init(&bKey, rkey, 128, 1);
	while(m_bOpen) 
	{
		if(pVideoInfo->abort_request)
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
							 av_free(p->buffer);
							 p->buffer=NULL;
						  }
					}
                    pVideoInfo->pictq.size=0;
					pVideoInfo->pictq.rindex=0;
					pVideoInfo->pictq.windex=0;
					pVideoInfo->pictq.rindex_shown=1;
                    
					pVideoInfo->pictq.m_pWaitCond->SetCond();
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
				 LOGE("catch full");
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
			  LOGE("readAV ret=%d \n",ret);
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
			//LOGE("audio_stream");
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
			//LOGE("video pkt");
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

	LOGE("readAV Over \n");
	int ii=0;
	ii++;
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
//快进
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