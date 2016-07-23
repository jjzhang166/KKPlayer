/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
//http://www.2cto.com/kf/201504/390386.html mediacodec aac 解码。


#include "KKPlayer.h"
#include "KKInternal.h"
#include "MD5/md5.h"
static AVPacket flush_pkt;
static int decoder_reorder_pts = -1;
static int framedrop = -1;
static int fast = 0;
static int lowres = 0;
static int64_t sws_flags = SWS_BICUBIC;
static int av_sync_type =AV_SYNC_AUDIO_MASTER;//AV_SYNC_EXTERNAL_CLOCK;//AV_SYNC_AUDIO_MASTER;//AV_SYNC_VIDEO_MASTER;// AV_SYNC_AUDIO_MASTER;
double rdftspeed = 0.02;

extern AVPixelFormat DstAVff;//=AV_PIX_FMT_YUV420P;//AV_PIX_FMT_BGRA;
//解码成BGRA格式
void KKPlayer::SetBGRA()
{
	DstAVff=AV_PIX_FMT_BGRA;
}

std::list<KKPluginInfo>  KKPlayer::KKPluginInfoList;
void KKPlayer::AddKKPluginInfo(KKPluginInfo& info)
{
     KKPluginInfoList.push_back(info);
}
void register_Kkv();
KKPlayer::KKPlayer(IKKPlayUI* pPlayUI,IKKAudio* pSound):m_pSound(pSound),m_pPlayUI(pPlayUI),m_nPreFile(false)
,m_PicBuf(NULL)
,m_PicBufLen(0)
{
	m_pAVInfomanage=CAVInfoManage::GetInance();
//	assert(m_pPlayUI!=NULL);
	m_DisplayVp=NULL;
	pVideoInfo=NULL;
	m_bOpen=false;
	static bool registerFF=true;
	if(registerFF)
	{
		 
		LOGE("len:%d",strlen("我们"));
		/*std::string strPort="";
		const char *pp=strPort.c_str();
		char* const aaa=(char* const)pp;*/

		avdevice_register_all();
		av_register_all();
		avfilter_register_all();
		avformat_network_init();
       // register_Kkv();
		
		registerFF=false;

		
	}
	
	AVInputFormat *ff=av_iformat_next(NULL);

	//AVCodec *codec=av_codec_next(NULL);
	//AVHWAccel *hwaccel=av_hwaccel_next(NULL);
	////LOGE("AVInputFormatList \n");
	//int i=0;
	//while(hwaccel!=NULL)
	//{
	//	const char *aa=hwaccel->name;
	//	::OutputDebugStringA("\n");
	//	::OutputDebugStringA(aa);
	//	::OutputDebugStringA("---");
	//	// aa=hwaccel->
	//	//::OutputDebugStringA(aa);
	//	//LOGE("%d,%s \n",i++,aa);
	//	hwaccel=av_hwaccel_next(hwaccel);
	//}

	char buf[1024]="";
	//MD5File("F://ttxx.mp4", buf);
	


	
	LOGE("\n");
	WindowWidth=0;
	WindowHeight=0;
	//avio_alloc_context
	start_time=AV_NOPTS_VALUE;
	m_CurTime=0;

	
}

void FreeKKIo(SKK_VideoState *kkAV);

void KKPlayer::CloseMedia()
{

	m_PreFileLock.Lock();
	while(m_nPreFile==1)
	{
		m_PreFileLock.Unlock();
		Sleep(10);
		m_PreFileLock.Lock();
	}
	if(m_nPreFile==2&&pVideoInfo->pFormatCtx!=NULL&& pVideoInfo->pFormatCtx!=NULL&&pVideoInfo->pFormatCtx->flags==AVFMT_FLAG_CUSTOM_IO)
	{
		AVIOContext *io=pVideoInfo->pFormatCtx->pb;
		KKPlugin* pKKP=(KKPlugin*)io->opaque; 
		pKKP->kkirq(pKKP);
	}
	while(m_nPreFile==2)
	{
		m_PreFileLock.Unlock();
		Sleep(100);
		m_PreFileLock.Lock();
       
	}
	m_nPreFile=0;
	m_PreFileLock.Unlock();

    m_CloseLock.Lock();
	if(!m_bOpen)
	{
		m_CloseLock.Unlock();
		return;
	}
    m_pSound->Stop();
	
	if(pVideoInfo==NULL) 
	{
		m_bOpen=false;
		m_CloseLock.Unlock();
		return;
	}
    pVideoInfo->abort_request=1;
	if(pVideoInfo->pFormatCtx!=NULL&&pVideoInfo->pFormatCtx->flags==AVFMT_FLAG_CUSTOM_IO)
	{
        FreeKKIo(pVideoInfo);
		pVideoInfo->pFormatCtx->pb=NULL;
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
		 av_usleep(50000);;
	}


	LOGE("thread Over 1");
	
	
	#ifndef WIN32_KK
			pthread_join(m_ReadThreadInfo.Tid_task,0);
			pthread_join(m_VideoRefreshthreadInfo.Tid_task,0);
	#endif	

	pVideoInfo->videoq.m_pWaitCond->SetCond();
	pVideoInfo->audioq.m_pWaitCond->SetCond();
	pVideoInfo->subtitleq.m_pWaitCond->SetCond();

	pVideoInfo->pictq.m_pWaitCond->SetCond();
	pVideoInfo->sampq.m_pWaitCond->SetCond();
	pVideoInfo->subpq.m_pWaitCond->SetCond();/**/

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
			 av_usleep(50000);
			 //pVideoInfo->videoq.m_pWaitCond->SetCond();
			 //pVideoInfo->audioq.m_pWaitCond->SetCond();
			 //pVideoInfo->subtitleq.m_pWaitCond->SetCond();

			 //pVideoInfo->pictq.m_pWaitCond->SetCond();
			 //pVideoInfo->sampq.m_pWaitCond->SetCond();
			 //pVideoInfo->subpq.m_pWaitCond->SetCond();/**/

		}
	}

	LOGE("thread Over 2");
	
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
	//视频包
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
	
	//IsWindow()
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


	avfilter_free(pVideoInfo->InAudioSrc);
	avfilter_free(pVideoInfo->OutAudioSink);
	avfilter_graph_free(&pVideoInfo->AudioGraph);

	/*if(pVideoInfo->pFormatCtx)
	   
	else*/
	{
         avcodec_close(pVideoInfo->auddec.avctx);
		avcodec_close(pVideoInfo->subdec.avctx);
		avcodec_close(pVideoInfo->viddec.avctx);
	}
	
	

	 avformat_close_input(&pVideoInfo->pFormatCtx);
//	avformat_free_context(pVideoInfo->pFormatCtx);
	
	



	av_free(pVideoInfo);
	pVideoInfo=NULL;

	start_time=AV_NOPTS_VALUE;
	m_CurTime=0;
	m_bOpen=false;
	m_CloseLock.Unlock();

	LOGE("thread Over Ok");
}

//获取媒体播放的信息
MEDIA_INFO KKPlayer::GetMediaInfo()
{
	
	MEDIA_INFO info;
	memset(&info,0,sizeof(info));

	if(m_nPreFile==3)
	{	
		m_CloseLock.Lock();
		if(m_bOpen)
		{
			info.Open=m_bOpen;
			if(pVideoInfo!=NULL)
			{
				info.CurTime=m_CurTime;
				if(pVideoInfo->pFormatCtx!=NULL){
					info.TotalTime=(pVideoInfo->pFormatCtx->duration/1000/1000);
					info.serial=pVideoInfo->viddec.pkt_serial;
				}
	           
			}else{info.CurTime=0;}
			
		}else{info.Open=false;}
		m_CloseLock.Unlock();/**/
	}
	
	//LOGE("MediaInfo:%f,%f \n",info.CurTime,info.TotalTime);
	return info;
}
//获取放播的历史信息
void KKPlayer::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
   if( m_pAVInfomanage!=NULL)
   {
	    m_pAVInfomanage->GetAVHistoryInfo(slQue);
   }
}
KKPlayer::~KKPlayer(void)
{
    delete m_pAVInfomanage;
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


void KKPlayer::SetDbPath(char *strPath)
{
	m_pAVInfomanage->SetPath(strPath);
	m_pAVInfomanage->InitDb();
	//m_pAVInfomanage->GetAVHistoryInfo();
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
//void KKPlayer::video_image_refresh(SKK_VideoState *is)
//{
//	if(is->video_st)
//	{
//retry:		
//				//没有数据
//				if (frame_queue_nb_remaining(&is->pictq) <= 0)
//				{
//				   return;
//				}
//				m_CurTime=is->audio_clock;
//				SKK_Frame *vp;
//				int redisplay=0;
//				is->pictq.mutex->Lock();
//				double time= av_gettime_relative()/1000000.0;
//				//获取上一次的读取位置
//			    SKK_Frame *lastvp = frame_queue_peek_last(&is->pictq);
//				/**********获取包位置**********/
//				vp = frame_queue_peek(&is->pictq);
//
//				if (vp->serial != is->videoq.serial) {
//					frame_queue_next(&is->pictq,false);
//					is->pictq.mutex->Unlock();
//					goto retry;
//				}
//				
//				if(vp!=NULL)
//				{
//					/*******时间**********/
//					if (lastvp->serial != vp->serial)
//					{
//						is->frame_timer = av_gettime_relative() / 1000000.0;
//					}
//					if (is->paused)
//					{
//						   is->pictq.mutex->Unlock();
//						   return;
//					}
//					//is->frame_timer += delay;
//					
//					
//					if (is->delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
//						is->frame_timer = time+is->delay;
//
//					/******上一次更新和这一次时间的差值。图片之间差值******/
//					is->last_duration = vp_duration(is, lastvp, vp);/******pts-pts********/
//					is->delay = compute_target_delay(is->last_duration, is);
//					/******上一次更新和这一次时间的差值。图片之间差值******/
//					double DiffCurrent=(is->frame_timer -is->vidclk.last_updated);
//					time= av_gettime_relative()/1000000.0;
//
//					if (time < is->frame_timer + is->delay) {
//						is->pictq.mutex->Unlock();
//						return;
//					}/**/
//
//					is->frame_timer += is->delay ;
//					if (is->delay  > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
//						is->frame_timer = time;
//
//					if(!isNAN(vp->pts))
//					{
//						update_video_pts(is, vp->pts, vp->pos, vp->serial);
//						if(start_time==AV_NOPTS_VALUE)
//						{
//							start_time=vp->pts;
//						}
//					}
//
//					if (frame_queue_nb_remaining(&is->pictq) >1)
//					{
//						SKK_Frame *nextvp = frame_queue_peek_next(&is->pictq);
//						double duration = vp_duration(is, vp, nextvp);
//						if(get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER && time > is->frame_timer + duration)
//						{
//							is->frame_drops_late++;
//							frame_queue_next(&is->pictq,false);
//							is->pictq.mutex->Unlock();
//							return;
//						}
//					}
//
//					if (vp->buffer)
//					{
//						 int total=(pVideoInfo->pFormatCtx->duration/1000/1000);
//                         if(vp->PktNumber%20==0)
//						 {
//							 m_pAVInfomanage->UpDataAVinfo(is->filename,m_CurTime,total,(unsigned char *)vp->buffer,vp->buflen,vp->width,vp->height);
//						 }
//					}
//					frame_queue_next(&is->pictq,false);
//				}
//				is->pictq.mutex->Unlock();
//	}
//}

void KKPlayer::video_image_refresh(SKK_VideoState *is)
{
	m_CurTime=is->audio_clock;
   double time=0,duration=0;
	if (0&&is->audio_st) {
		time = av_gettime_relative() / 1000000.0;
		if (is->force_refresh || is->last_vis_time + rdftspeed < time)
		{
			
			
			is->last_vis_time = time;
		}

		
		is->remaining_time = FFMIN(is->remaining_time, is->last_vis_time + rdftspeed - time);
	}

	if(is->video_st)
    {
retry:
			//没有数据
			if (frame_queue_nb_remaining(&is->pictq) <= 0)
			{
			   goto display;
			}
			
				
			SKK_Frame *vp;
			int redisplay=0;
		
			/**********获取包位置**********/
			vp = frame_queue_peek(&is->pictq);
			//获取上一次的读取位置
			SKK_Frame *lastvp = frame_queue_peek_last(&is->pictq);
			if (vp->serial != is->videoq.serial) {
				frame_queue_next(&is->pictq,true);
				goto retry;
			}
			
			if(vp!=NULL)
			{
				
				
				/*******时间**********/
				if (lastvp->serial != vp->serial && !redisplay)
				{
					is->frame_timer = av_gettime_relative() / 1000000.0;
				}

				if (is->paused)
					goto display;
				//is->frame_timer += delay;
				/******上一次更新和这一次时间的差值。图片之间差值******/
				is->last_duration = vp_duration(is, lastvp, vp);/******pts-pts********/
				is->delay = compute_target_delay(is->last_duration, is);

				time= av_gettime_relative()/1000000.0;
				if (time < is->frame_timer + is->delay) {
					double llxxxx=is->frame_timer + is->delay - time;
					is->remaining_time = FFMIN(llxxxx, is->remaining_time);
					goto display;
				}

				if (is->delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
					is->frame_timer = time+is->delay;


				if(!isNAN(vp->pts))
				{
					update_video_pts(is, vp->pts, vp->pos, vp->serial);
				}
				if (frame_queue_nb_remaining(&is->pictq) > 1)
				{
					SKK_Frame *nextvp = frame_queue_peek_next(&is->pictq);
					duration = vp_duration(is, vp, nextvp);
					if((get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) && time > is->frame_timer + duration)
					{
						is->frame_drops_late++;
						frame_queue_next(&is->pictq,true);
						//is->pictq.mutex->Unlock();
						goto retry;
					}
				}
				
				if (vp->buffer)
				{
					    int total=(pVideoInfo->pFormatCtx->duration/1000/1000);
						if(start_time==AV_NOPTS_VALUE)
						{
						  start_time=vp->pts;
						}
						if(vp->PktNumber%20==0)
						{
						  m_pAVInfomanage->UpDataAVinfo(is->filename,m_CurTime,total,(unsigned char *)vp->buffer,vp->buflen,is->viddec_width,is->viddec_height);
						}		
				}
				frame_queue_next(&is->pictq,true);
			}
			//is->pictq.mutex->Unlock();
	}
display:
	if(m_pPlayUI!=NULL)
	{		
		m_pPlayUI->AVRender();
	}
	if(is->subtitle_st!=NULL)
	{

	}
}
int KKPlayer::PktSerial()
{
	return m_PktSerial;
}

void KKPlayer::VideoRefresh()
{
	if(pVideoInfo==NULL)
		return;
	if(pVideoInfo->IsReady==0||pVideoInfo->paused)
		return;
	
	video_image_refresh(pVideoInfo);
}


void KKPlayer::RenderImage(CRender *pRender,bool Force)
{
	SKK_Frame *vp;
	
	
	if(pVideoInfo==NULL)
	{
		
		int len=0;
		unsigned char* pBkImage=m_pPlayUI->GetCenterLogoImage(len);
		pRender->LoadCenterLogo(pBkImage,len);

		pBkImage=m_pPlayUI->GetBkImage(len);
		pRender->renderBk(pBkImage,len);
		return;
	}else 
	{
		//LOGE("pVideoInfo->IsReady=%d m_bOpen=%d\n",pVideoInfo->IsReady,m_bOpen);
		if(pVideoInfo->IsReady==0)
		{
			int len=0;
			unsigned char* pWaitImage=m_pPlayUI->GetWaitImage(len,0);
			if(pWaitImage!=NULL)
			{
                 pRender->SetWaitPic(pWaitImage,len);
				 pRender->render(NULL,0,0);
			}
			
		}else
		{
			if(!m_bOpen)
			{
			   return;
			}
		      

		//	pVideoInfo->pictq.mutex->Lock();
			
		   vp =frame_queue_peek_last(&pVideoInfo->pictq);//frame_queue_peek(&pVideoInfo->pictq);// 
		   if(vp->buffer!=NULL)
		   {
			vp->BmpLock->Lock();
			pRender->render((char*)vp->buffer,pVideoInfo->viddec_width,pVideoInfo->viddec_height);
			vp->BmpLock->Unlock();
		   }
			/*if(m_PicBuf!=NULL)
			{
				pRender->render((char*)m_PicBuf,width,height);
			}*/

		
		    
		}
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
	//VideoDisplay(vp->buffer,pVideoInfo->width,vp->height,&memdc,pVideoInfo->last_duration,vp->pts,vp->duration,vp->pos,pVideoInfo->delay);
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

	RECT rt;
	::GetClientRect(m_hwnd,&rt);
	int www=rt.right-rt.left;
	int hhh=rt.bottom-rt.top;

	//拷贝图像
	StretchDIBits(dc, 0,   0, 
		www,   hhh, 
		0,   0, 
		w,   h, 
		buf, (BITMAPINFO*)&header,
		DIB_RGB_COLORS, SRCCOPY);/**/

	{			
		//速率
		char t[256]="";
		sprintf(t, "VPdur:%f",duration);
		int w2=0;
		RECT rt2={w2,10,w2+150,30};
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

char * c_left(char *dst,char *src, int n)
{
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if(n>len) n = len;
	/*p += (len-n);*/   /*从右边第n个字符开始*/
	while(n--) *(q++) = *(p++);
	*(q++)='\0'; /*有必要吗？很有必要*/
	return dst;
}
int KKPlayer::KKProtocolAnalyze(char* StrfileName,KKPluginInfo &KKPl)
{
	char* pos=strstr(StrfileName,":") ;
	if(pos!=NULL)
	{
		char ProName[256];
		
		int lll=pos-StrfileName;
		c_left(ProName,StrfileName,lll);

		std::list<KKPluginInfo>::iterator It=KKPlayer::KKPluginInfoList.begin();
		for(;It!=KKPluginInfoList.end();++It)
		{
			if(strcmp(ProName,It->ptl)==0)
			{
				int len=strlen(StrfileName)-lll;
				if(len>0)
				{
					strcpy(ProName,pos+1);
					memset(StrfileName,0,1024);
					strcpy(StrfileName,ProName);
					KKPl=*It;/**/
					return 1;
				}
			}
		}
        return -1;
		
	}
	return 0;
}
int KKPlayer::OpenMedia(char* fileName,char* FilePath)
{
	m_PreFileLock.Lock();
	if(m_nPreFile!=0)
	{
		m_PreFileLock.Unlock();
        return -1;
	}
	m_PreFileLock.Unlock();

	//fileName="KKV:687281b0bee195d415caaf20bd50a8de|test.mp4";
	//fileName="KKV:e8a486a4e28480ad18bd5041c2ad34fa|test.mp4";
	//fileName="c:/xx.mp4";
	m_CloseLock.Lock();
    if(m_bOpen)
	{
        m_CloseLock.Unlock();
		return -1;
	}
	m_bOpen=true;
	
	m_PreFileLock.Lock();
	m_nPreFile=1;
	m_PreFileLock.Unlock();
	
	pVideoInfo = (SKK_VideoState*)av_mallocz(sizeof(SKK_VideoState));
	memset(pVideoInfo,0,sizeof(SKK_VideoState));

	pVideoInfo->pKKPluginInfo=(KKPluginInfo *)av_mallocz(sizeof(KKPluginInfo));
	pVideoInfo->pflush_pkt =(AVPacket*)av_mallocz(sizeof(AVPacket));

    m_PktSerial=0;
	
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
	
	memset(pVideoInfo->pflush_pkt,0,sizeof(AVPacket));
	av_init_packet(pVideoInfo->pflush_pkt);
	flush_pkt.data = (uint8_t *)pVideoInfo->pflush_pkt;

	av_strlcpy(pVideoInfo->filename, fileName, strlen(fileName)+1);

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
	if (frame_queue_init(&pVideoInfo->subpq, &pVideoInfo->subtitleq, SUBPICTURE_QUEUE_SIZE, 1) < 0)
	{

	}
	pVideoInfo->subpq.m_pWaitCond=new CKKCond_t();
	pVideoInfo->subpq.m_pWaitCond->SetCond();
	if (frame_queue_init(&pVideoInfo->sampq, &pVideoInfo->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
	{

	}
	pVideoInfo->sampq.m_pWaitCond=new CKKCond_t();
    pVideoInfo->sampq.m_pWaitCond->SetCond();

	m_pSound->SetAudioCallBack(audio_callback);
	m_pSound->SetUserData(pVideoInfo);
	pVideoInfo->pKKAudio=m_pSound;
    pVideoInfo->AVRate=100;
	m_ReadThreadInfo.ThOver=false;
	m_VideoRefreshthreadInfo.ThOver=false;
	m_AudioCallthreadInfo.ThOver=false;

	float aa=(float)pVideoInfo->AVRate/100;
	snprintf(pVideoInfo->Atempo,sizeof(pVideoInfo->Atempo),"atempo=%f",aa);


	pVideoInfo->InAudioSrc=NULL;
	pVideoInfo->OutAudioSink=NULL;
	pVideoInfo->AudioGraph=NULL;
#ifdef WIN32_KK
	m_ReadThreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, ReadAV_thread, (LPVOID)this, 0,&m_ReadThreadInfo.Addr);
	m_VideoRefreshthreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, VideoRefreshthread, (LPVOID)this, 0,&m_VideoRefreshthreadInfo.Addr);
    m_AudioCallthreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, Audio_Thread, (LPVOID)this, 0,&m_AudioCallthreadInfo.Addr);
#else

	

	m_ReadThreadInfo.Addr = pthread_create(&m_ReadThreadInfo.Tid_task, NULL, (void* (*)(void*))ReadAV_thread, (LPVOID)this);
	LOGE("m_ReadThreadInfo.Addr =%d\n",m_ReadThreadInfo.Addr);
	LOGE("VideoRefreshthread XX");
	m_VideoRefreshthreadInfo.Addr = pthread_create(&m_VideoRefreshthreadInfo.Tid_task, NULL, (void* (*)(void*))VideoRefreshthread, (LPVOID)this);
	
	m_AudioCallthreadInfo.Addr =pthread_create(&m_AudioCallthreadInfo.Tid_task, NULL, (void* (*)(void*))Audio_Thread, (LPVOID)this);
#endif

	m_CloseLock.Unlock();
	return 0;
}
void KKPlayer::OnDecelerate()
{
	int64_t seek_target =0;
	bool okk=false;
   // m_CloseLock.Lock();
	if(pVideoInfo!=NULL&&pVideoInfo->AVRate>50)
	{
		pVideoInfo->AVRate-=10;
		float aa=(float)pVideoInfo->AVRate/100;
		snprintf(pVideoInfo->Atempo,sizeof(pVideoInfo->Atempo),"atempo=%f",aa); 
		seek_target= m_CurTime;/**/
		//packet_queue_put(&pVideoInfo->audioq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
		//packet_queue_put(&pVideoInfo->videoq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
        okk=true;
	}
	//m_CloseLock.Unlock();
	if(okk)
	{
		KKSeek(Right,1);
		pVideoInfo->seek_req=2;
	}
}
void KKPlayer::OnAccelerate()
{
	int64_t seek_target=0;
	bool okk=false;
	//m_CloseLock.Lock();
	if(pVideoInfo!=NULL&&pVideoInfo->AVRate<200)
	{
		pVideoInfo->AVRate+=10;
		float aa=(float)pVideoInfo->AVRate/100;
		  snprintf(pVideoInfo->Atempo,sizeof(pVideoInfo->Atempo),"atempo=%.2f",aa);
        //packet_queue_put(&pVideoInfo->audioq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
		//packet_queue_put(&pVideoInfo->videoq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
		seek_target = m_CurTime;
		okk=true;
	}
	//m_CloseLock.Unlock();

	if(okk)
	{
		KKSeek(Right,1);
		pVideoInfo->seek_req=2;
	}
}
int KKPlayer::GetAVRate()
{

	int Rate=100;
	//m_CloseLock.Lock();
	if(pVideoInfo!=NULL)
	{
		Rate=pVideoInfo->AVRate;
	}
	//m_CloseLock.Unlock();
	return Rate;
}
/*********视频刷新线程********/
 unsigned __stdcall KKPlayer::VideoRefreshthread(LPVOID lpParameter)
 {
	 LOGE("VideoRefreshthread strat");
     KKPlayer* pPlayer=(KKPlayer*)lpParameter;
	 pPlayer->m_VideoRefreshthreadInfo.ThOver=false;
	 int llxx=0;
#ifdef WIN32
	 ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#endif
	 pPlayer->pVideoInfo->remaining_time = 0.01;
	 while(pPlayer->m_bOpen)
	 {
		
		if(pPlayer->pVideoInfo!=NULL)
		{
			if(pPlayer->pVideoInfo->abort_request==1)
				break;

			if (pPlayer->pVideoInfo->remaining_time > 0.0)
			{
				int64_t ll=(int64_t)(pPlayer->pVideoInfo->remaining_time* 1000000.0);
				av_usleep(ll);
			}
			 pPlayer->pVideoInfo->remaining_time = 0.01;
			
            pPlayer->VideoRefresh();
		}else{
            av_usleep(5000);
		}
		
		
	 }
	 pPlayer->m_VideoRefreshthreadInfo.ThOver=true;
	 LOGE("VideoRefreshthread over");
	 return 1;
 }

 //音频数据回调线程
 unsigned __stdcall  KKPlayer::Audio_Thread(LPVOID lpParameter)
 {
	#ifdef WIN32
		 ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	#endif
	 KKPlayer* pPlayer=(KKPlayer*)lpParameter;
	 pPlayer->ReadAudioCall();
	 return 1;
 }
 void KKPlayer::ReadAudioCall()
 {
	 m_AudioCallthreadInfo.ThOver=false;
	 if(pVideoInfo->pKKAudio!=NULL)
	 {
		 pVideoInfo->pKKAudio->Start();
		 while(!pVideoInfo->abort_request && pVideoInfo->pKKAudio!=NULL)
		 {
			 if(pVideoInfo->IsReady)
			   pVideoInfo->pKKAudio->ReadAudio();
			 else
				 av_usleep(1000);
		 }
	 }
	 LOGE("KKPlayer Audio_Thread over");
	 m_AudioCallthreadInfo.ThOver=true;
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


 AVIOContext * CreateKKIo(SKK_VideoState *);
/*****读取视频信息******/
void KKPlayer::ReadAV()
{
	m_CloseLock.Lock();
	m_ReadThreadInfo.ThOver=false;
	LOGE("ReadAV thread start");
	AVFormatContext *pFormatCtx= avformat_alloc_context();
	pVideoInfo->pFormatCtx = pFormatCtx;
	AVDictionary *format_opts=NULL;
	int err=-1;
	int scan_all_pmts_set = 0;
	pVideoInfo->iformat=NULL;
	

	pFormatCtx->interrupt_callback.callback = decode_interrupt_cb;
	pFormatCtx->interrupt_callback.opaque = pVideoInfo;

	if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
		av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		scan_all_pmts_set = 1;
	}
	
	

	if(KKProtocolAnalyze(pVideoInfo->filename,*pVideoInfo->pKKPluginInfo)==1)
	{	
	    pFormatCtx->pb=CreateKKIo(pVideoInfo);
        pFormatCtx->flags = AVFMT_FLAG_CUSTOM_IO;
    }



	m_PreFileLock.Lock();
	m_nPreFile=2;
	m_PreFileLock.Unlock();
	
	//此函数是阻塞的
	err = avformat_open_input(
		&pVideoInfo->pFormatCtx,                    pVideoInfo->filename,
		pVideoInfo->iformat,    &format_opts);
	LOGE("avformat_open_input=%d,%s \n",err,pVideoInfo->filename);
	
	m_PreFileLock.Lock();
	m_nPreFile=3;
	m_PreFileLock.Unlock();




	 if(!m_bOpen)
	 {
		 m_CloseLock.Unlock();
		 return;
	 }
    //文件打开失败
	if(err<0)
	{
		char urlx[256]="";
		strcpy(urlx,pVideoInfo->filename);
		pVideoInfo->abort_request=1;
		m_CloseLock.Unlock();

        if(m_pPlayUI!=NULL)
		{
			m_pPlayUI->OpenMediaFailure(urlx);
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
		m_CloseLock.Unlock();
		return; // Couldn't find stream information
    }
	
	
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
	m_CloseLock.Unlock();

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
				av_usleep(1000);
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
			int64_t seek_min    =pVideoInfo->seek_rel > 0 ? seek_target - pVideoInfo->seek_rel + 2: INT64_MIN;//pVideoInfo->seek_pos-10 * AV_TIME_BASE; //
			int64_t seek_max    =pVideoInfo->seek_rel < 0 ? seek_target - pVideoInfo->seek_rel - 2: INT64_MAX;//=pVideoInfo->seek_pos+10 * AV_TIME_BASE; //


			if(pVideoInfo->seek_req==2)
			{
				seek_min=seek_target;
                seek_max =seek_target +10000;
			}
			//int64_t seek_min    =pVideoInfo->seek_pos-10 * AV_TIME_BASE; //
			//int64_t seek_max    =pVideoInfo->seek_pos+10 * AV_TIME_BASE; //
			ret = avformat_seek_file(pVideoInfo->pFormatCtx, -1, seek_min, seek_target, seek_max, pVideoInfo->seek_flags);
			if (ret < 0) 
			{
			     assert(0);
				//失败
			}else
			{
				//if(pVideoInfo->seek_req==2)
			    //{
				  // packet_queue_put(&pVideoInfo->audioq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
				   //packet_queue_put(&pVideoInfo->videoq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
				//}else{
				Avflush(seek_target);
				//}
			}
			pVideoInfo->seek_req=0;
		}

		m_PktSerial=pVideoInfo->viddec.pkt_serial;

		/******缓存满了*******/
		while(1)
		{
			if(pVideoInfo->abort_request)
			{
				break;
			}else if(pVideoInfo->audioq.size + pVideoInfo->videoq.size + pVideoInfo->subtitleq.size > MAX_QUEUE_SIZE)
			{
				// LOGE("catch full");
				 av_usleep(5000);;//等待一会
			}else
			{
				break;
			}
		}
        /********读取一个pkt**********/
		ret = av_read_frame(pFormatCtx, pkt);
		if (ret < 0) 
		{
			
			  //LOGE("readAV ret=%d \n",ret);
			 if ((ret == AVERROR_EOF || avio_feof(pFormatCtx->pb)) && !pVideoInfo->eof) 
			// if ((ret == AVERROR_EOF ) && !pVideoInfo->eof) 
			 {
				    
                    pVideoInfo->eof=1;
			 }

			 if (pFormatCtx->pb && pFormatCtx->pb->error)
				    break;
			av_usleep(1000);
			continue;
		} else 
		{
			pVideoInfo->eof = 0;
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


		
		//音频
		if (pkt->stream_index == pVideoInfo->audio_stream && pkt_in_play_range) 
		{
			//LOGE("audio_stream");
			packet_queue_put(&pVideoInfo->audioq, pkt,pVideoInfo->pflush_pkt);
		} //视频
		else if (
			pkt->stream_index == pVideoInfo->video_stream && pkt_in_play_range
			&& !(pVideoInfo->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC)
			) 
		{
			//LOGE("video pkt");
			/********无内存泄露*******/
			packet_queue_put(&pVideoInfo->videoq, pkt,pVideoInfo->pflush_pkt);
		}//字幕
		else if (pkt->stream_index == pVideoInfo->subtitle_stream && pkt_in_play_range) 
		{
			//printf("subtitleq\n");
			packet_queue_put(&pVideoInfo->subtitleq, pkt,pVideoInfo->pflush_pkt);
		} else
		{
			av_free_packet(pkt);
		}
		
	}

	
	LOGE("readAV Over \n");
	
}
//释放队列数据
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
		if(pVideoInfo!=NULL)
		{
			if(pVideoInfo->paused==1)
				pVideoInfo->paused=0;
			else
				pVideoInfo->paused=1;
		}
}
//快进
void KKPlayer::KKSeek( SeekEnum en,int value)
{
   m_CloseLock.Lock();
   if(pVideoInfo!=NULL)
   {
	   m_CurTime=value;
	   double incr, pos, frac;
	   incr=value;
	   pos = get_master_clock(pVideoInfo);
	   if(pVideoInfo->AVRate!=100)
	   {
		   float aa=(float)pVideoInfo->AVRate/100;
		   pos=pos*aa;
	   }
	   if (isNAN(pos))
		   pos = (double)pVideoInfo->seek_pos / AV_TIME_BASE;
	   pos += incr;
	   if (pVideoInfo->pFormatCtx->start_time != AV_NOPTS_VALUE && pos < pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE)
		   pos = pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE;
	   stream_seek(pVideoInfo, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
	   
   }
   m_CloseLock.Unlock();
}

void KKPlayer::AVSeek(int value)
{
	m_CloseLock.Lock();
	
	if(pVideoInfo!=NULL)
	{
		m_CurTime=value;
		double incr, pos, frac;
		incr=value;
		pos = get_master_clock(pVideoInfo);
		if (isNAN(pos))
			pos = (double)pVideoInfo->seek_pos / AV_TIME_BASE;
		incr=value-pos;
		pos += incr;
		incr=60;
		if (pVideoInfo->pFormatCtx->start_time != AV_NOPTS_VALUE && pos < pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE)
			pos = pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE;
		stream_seek(pVideoInfo, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
	}
	m_CloseLock.Unlock();
}


void KKPlayer::Avflush(int64_t seek_target)
{
	if (pVideoInfo->video_stream >= 0) 
	{
		//packet_queue_flush(&pVideoInfo->videoq);
		packet_queue_put(&pVideoInfo->videoq, pVideoInfo->pflush_pkt,pVideoInfo->pflush_pkt);
		pVideoInfo->pictq.mutex->Lock();
		//for(int i=0;i<pVideoInfo->pictq.max_size;i++)
		//{
		//	SKK_Frame *p=& pVideoInfo->pictq.queue[i];
		//	/*******这里不释放内存是为了保证图像连续********/						 
		//	if(p->buffer!=NULL)
		//	{
		//		av_free(p->buffer);
		//		p->buffer=NULL;
		//	}
		//}
		pVideoInfo->pictq.size=0;
		pVideoInfo->pictq.rindex=0;
		pVideoInfo->pictq.windex=0;
		pVideoInfo->pictq.rindex_shown=1;

		pVideoInfo->pictq.m_pWaitCond->SetCond();
		pVideoInfo->pictq.mutex->Unlock();
	}

	if (pVideoInfo->audio_stream >= 0) 
	{
		//packet_queue_flush(&pVideoInfo->audioq);
		packet_queue_put(&pVideoInfo->audioq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);

		pVideoInfo->sampq.mutex->Lock();

		pVideoInfo->sampq.size=0;
		pVideoInfo->sampq.rindex=0;
		pVideoInfo->sampq.windex=0;
		pVideoInfo->sampq.rindex_shown=1;
		pVideoInfo->sampq.m_pWaitCond->SetCond();
		pVideoInfo->sampq.mutex->Unlock();/**/
	}
	if (pVideoInfo->subtitle_stream >= 0) 
	{
		//packet_queue_flush(&pVideoInfo->subtitleq);
		packet_queue_put(&pVideoInfo->subtitleq, pVideoInfo->pflush_pkt,pVideoInfo->pflush_pkt);
	}

	if (pVideoInfo->seek_flags & AVSEEK_FLAG_BYTE) 
	{
		set_clock(&pVideoInfo->extclk, NAN, 0);
	} else 
	{
		set_clock(&pVideoInfo->extclk, seek_target / (double)AV_TIME_BASE, 0);
	}
}