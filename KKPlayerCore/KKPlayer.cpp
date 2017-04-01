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
#include "rtmp/SrsRtmpPlugin.h"
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

//如果定义安卓平台。
#ifdef Android_Plat
void* kk_jni_attach_env();
int kk_jni_detach_env();
#endif
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
	pVideoInfo=NULL;
	m_bOpen=false;
	static bool registerFF=true;
	if(registerFF)
	{
		 
		LOGE("len:%d \n",strlen("我们"));
		/*std::string strPort="";
		const char *pp=strPort.c_str();
		char* const aaa=(char* const)pp;*/

		avdevice_register_all();
		av_register_all();
		avfilter_register_all();
		avformat_network_init();
       // register_Kkv();
		
		registerFF=false;
        AddSrsRtmpPluginInfo();
		
	}
	
#ifdef Android_Plat
	m_pVideoRefreshJNIEnv=NULL;
#endif
	AVInputFormat *ff=av_iformat_next(NULL);

	AVCodec *codec=av_codec_next(NULL);
	AVHWAccel *hwaccel=av_hwaccel_next(NULL);
	//LOGE("AVInputFormatList \n");
#ifdef WIN32
	int i=0;
	while(hwaccel!=NULL)
	{
		const char *aa=hwaccel->name;
		::OutputDebugStringA("\n");
		::OutputDebugStringA(aa);
		::OutputDebugStringA("---");
		// aa=hwaccel->
		//::OutputDebugStringA(aa);
		//LOGE("%d,%s \n",i++,aa);
		hwaccel=av_hwaccel_next(hwaccel);
	}
#endif

	char buf[1024]="";
	//MD5File("F://ttxx.mp4", buf);
	
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
		Sleep(50);
		m_PreFileLock.Lock();

		LOGE(" xx\n");
	}
	
	//强制中断
	if(pVideoInfo!=NULL)
	{
		pVideoInfo->abort_request=1;
	}

	while(m_nPreFile==2)
	{
		m_PreFileLock.Unlock();
		Sleep(100);
		m_PreFileLock.Lock();
		LOGE(" xx2\n");
       
	}
	m_nPreFile=0;
	m_PreFileLock.Unlock();



    m_PlayerLock.Lock();
	if(!m_bOpen)
	{
		m_PlayerLock.Unlock();
		return;
	}
    m_pSound->Stop();
	
	if(pVideoInfo==NULL) 
	{
		m_bOpen=false;
		m_PlayerLock.Unlock();
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
		LOGE("thread Over1 m_ReadThreadInfo%d,m_VideoRefreshthreadInfo%d \n",m_ReadThreadInfo.ThOver
			,m_VideoRefreshthreadInfo.ThOver
			);
		 av_usleep(10000);
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
			LOGE("thread Over2 viddec%d,auddec%d,subdec%d \n",pVideoInfo->viddec.decoder_tid.ThOver
				,pVideoInfo->auddec.decoder_tid.ThOver
				,pVideoInfo->subdec.decoder_tid.ThOver
				);
			 av_usleep(10000);
		}
	}

	LOGE("thread Over 2 \n");
	
#ifdef WIN32_KK
	//SDL_CloseAudio();
	//关闭读取线程
	//::TerminateThread(m_ReadThreadInfo.ThreadHandel,0);
	::CloseHandle(m_ReadThreadInfo.ThreadHandel);
    
	//::TerminateThread(m_VideoRefreshthreadInfo.ThreadHandel,0);
	::CloseHandle(m_VideoRefreshthreadInfo.ThreadHandel);
	
	//关闭相关解码线程
	//::TerminateThread(pVideoInfo->viddec.decoder_tid.ThreadHandel,0);
	::CloseHandle(pVideoInfo->viddec.decoder_tid.ThreadHandel);
	
	//::TerminateThread(pVideoInfo->auddec.decoder_tid.ThreadHandel,0);
	::CloseHandle(pVideoInfo->auddec.decoder_tid.ThreadHandel);
	
	//::TerminateThread(pVideoInfo->subdec.decoder_tid.ThreadHandel,0);
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

	LOGE("PacketQueuefree1 \n");
	PacketQueuefree();
	LOGE("PacketQueuefree OK \n");

	delete pVideoInfo->videoq.pLock;
	pVideoInfo->videoq.pLock=NULL;
	delete pVideoInfo->subtitleq.pLock;
	pVideoInfo->subtitleq.pLock=NULL;
	delete pVideoInfo->audioq.pLock;
	pVideoInfo->audioq.pLock=NULL;
	
	
	LOGE("pVideoInfo->pictq.mutex OK \n");
	delete pVideoInfo->pictq.mutex;
	pVideoInfo->pictq.mutex=NULL;
	delete pVideoInfo->subpq.mutex;
	pVideoInfo->subpq.mutex=NULL;
	delete pVideoInfo->sampq.mutex;
	pVideoInfo->sampq.mutex=NULL;

	
	if(pVideoInfo->swr_ctx!=NULL)
	{
	   swr_free(&pVideoInfo->swr_ctx);
	   pVideoInfo->swr_ctx=NULL;
	   LOGE("swr_free OK \n");
	}

	if(pVideoInfo->img_convert_ctx!=NULL)
	{
		sws_freeContext(pVideoInfo->img_convert_ctx);
		pVideoInfo->img_convert_ctx=NULL;
		LOGE("pVideoInfo->img_convert_ctx \n");
	}
	


	if(pVideoInfo->sub_convert_ctx!=NULL)
	{
		sws_freeContext(pVideoInfo->sub_convert_ctx);
		pVideoInfo->sub_convert_ctx=NULL;
		LOGE("sub_convert_ctx \n");
	}
	


	if(pVideoInfo->InAudioSrc!=NULL)
	{
	    avfilter_free(pVideoInfo->InAudioSrc);
        LOGE("InAudioSrc \n");
	}
	if(pVideoInfo->OutAudioSink!=NULL)
	{
	    avfilter_free(pVideoInfo->OutAudioSink);
	    LOGE("OutAudioSink \n");
	}
	if(pVideoInfo->AudioGraph!=NULL)
	{
		avfilter_graph_free(&pVideoInfo->AudioGraph);
		LOGE("pVideoInfo->AudioGraph \n");
	}
	
	if(pVideoInfo->auddec.avctx!=NULL){
		 LOGE("avcodec_close(pVideoInfo->auddec.avctx); OK \n");
		 avcodec_close(pVideoInfo->auddec.avctx);
		 avcodec_free_context(&pVideoInfo->auddec.avctx);
		 av_freep(&pVideoInfo->audio_buf1);
	}
	
	if(pVideoInfo->subdec.avctx!=NULL)
	{
		 LOGE("avcodec_close(pVideoInfo->subdec.avctx); OK \n");
	   avcodec_close(pVideoInfo->subdec.avctx);
	   avcodec_free_context(&pVideoInfo->subdec.avctx);
	}

	if(pVideoInfo->viddec.avctx!=NULL)
	{
	   LOGE("avcodec_close(pVideoInfo->viddec.avctx); \n");

	   avcodec_flush_buffers(pVideoInfo->viddec.avctx);
	   avcodec_close(pVideoInfo->viddec.avctx);
	 //  av_packet_unref(&pVideoInfo->viddec->pkt);
	   avcodec_free_context(&pVideoInfo->viddec.avctx);
	   LOGE("avcodec_close(avcodec_free_context(&pVideoInfo->viddec.avctx) OK \n");
	}

	if(pVideoInfo->pFormatCtx!=NULL&&pVideoInfo->pFormatCtx->flags==AVFMT_FLAG_CUSTOM_IO)
	{
		FreeKKIo(pVideoInfo);
		pVideoInfo->pFormatCtx->pb=NULL;
	}

	LOGE("pVideoInfo->pFormatCtx\n");
	avformat_close_input(&pVideoInfo->pFormatCtx);


	LOGE("pVideoInfo->pKKPluginInfo\n");
	KK_Free_(pVideoInfo->pKKPluginInfo);

    av_packet_unref(pVideoInfo->pflush_pkt);
	KK_Free_(pVideoInfo->pflush_pkt);
	KK_Free_(pVideoInfo);
	pVideoInfo=NULL;

	start_time=AV_NOPTS_VALUE;
	m_CurTime=0;
	m_bOpen=false;
	m_PlayerLock.Unlock();

	#ifdef Android_Plat
	    m_pVideoRefreshJNIEnv=NULL;
    #endif
	LOGE("KKplay Over\n");
}

//获取媒体播放的信息
MEDIA_INFO KKPlayer::GetMediaInfo()
{
	
	MEDIA_INFO info;
	memset(&info,0,sizeof(info));

	if(m_nPreFile==3)
	{	
		m_PlayerLock.Lock();
		if(m_bOpen)
		{
			info.Open=m_bOpen;
			if(pVideoInfo!=NULL)
			{
				info.CurTime=m_CurTime;
				if(pVideoInfo->pFormatCtx!=NULL)
				{
					info.AvFile=(const char*)pVideoInfo->filename;
					info.TotalTime=(pVideoInfo->pFormatCtx->duration/1000/1000);
					info.serial=pVideoInfo->viddec.pkt_serial;
                    info.CacheInfo.AudioSize=m_AVCacheInfo.AudioSize;
					info.CacheInfo.VideoSize=m_AVCacheInfo.VideoSize;
					info.CacheInfo.MaxTime=m_AVCacheInfo.MaxTime;
				
                    info.FileSize=pVideoInfo->fileSize;
				
					snprintf(info.AVRes,32,"%dx%d",pVideoInfo->viddec_width,pVideoInfo->viddec_height);
					char infostr[1024]="";
					if(pVideoInfo->viddec.avctx!=NULL)
					{
						strcat(infostr,"视频流信息:");
						strcat(infostr,"\n+视频编码:");
						strcat(infostr, pVideoInfo->viddec.avctx->codec->name);
						strcat(infostr, "\n+平均码率:");   
						char abcd[32]="";
						
						snprintf(abcd,32,"%dkbps",pVideoInfo->viddec.avctx->bit_rate/1000);
						strcat(infostr, abcd);

						strcat(infostr, "\n+视频帧率:");   
						snprintf(abcd,32,"%d",pVideoInfo->viddec.avctx->framerate);
						strcat(infostr, abcd);
					}
					if(pVideoInfo->audio_st!=NULL&&pVideoInfo->auddec.avctx!=NULL)
					{

						strcat(infostr,"\n\n音频流信息:");
						strcat(infostr,"\n+音频编码:");
						strcat(infostr, pVideoInfo->auddec.avctx->codec->name);
						strcat(infostr, "\n+平均码率:");   
						char abcd[32]="";
						snprintf(abcd,32,"%dkbps",pVideoInfo->auddec.avctx->bit_rate/1000);
						strcat(infostr, abcd);

						strcat(infostr, "\n+采样帧率:");   
						snprintf(abcd,32,"%d Hz",pVideoInfo->auddec.avctx->sample_rate);
						strcat(infostr, abcd);

						strcat(infostr, "\n+声 道 数:");   
						snprintf(abcd,32,"%d channels",pVideoInfo->auddec.avctx->channels);
						strcat(infostr, abcd);
					}
					
					
					strcpy(info.AVinfo,infostr);
				}
	           
			}else{info.CurTime=0;}
			
		}else{
			info.Open=false;
		}
		m_PlayerLock.Unlock();/**/
	}
	
	//LOGE("MediaInfo:%f,%f \n",info.CurTime,info.TotalTime);
	return info;
}
//获取放播的历史信息
void KKPlayer::GetAVHistoryInfo(std::vector<AV_Hos_Info *> &slQue)
{
   if( m_pAVInfomanage!=NULL){
	    m_pAVInfomanage->GetAVHistoryInfo(slQue);
   }
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
      //rtmp://112.117.211.114/NewWorld/30
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
//http://120.25.236.44:9999/1/test.m3u8
//rtmp://117.135.131.98/771/003 live=1
void KKPlayer::video_image_refresh(SKK_VideoState *is)
{
	
	
    double time=0,duration=0;
	if(is->audio_st)
	    m_CurTime=is->audio_clock;
	else if(is->video_st)
		m_CurTime=is->vidclk.pts;
	else
		m_CurTime=get_master_clock(is);
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
				//is->remaining_time = 0.01;
			   //::OutputDebugStringA("frame_queue_nb_remaining(&is->pictq) <= 0 \n");
			   goto display;
			}
			
			
			

			SKK_Frame *vp;
			
		
			//获取上一次的读取位置
			SKK_Frame *lastvp = frame_queue_peek_last(&is->pictq);

			
			/**********获取包位置**********/
			vp = frame_queue_peek(&is->pictq);
			is->video_clock=vp->pts;
			if(is->realtime&&is->audio_st==NULL&&!is->abort_request){
				if(pVideoInfo->nRealtimeDelay>0)
				{
				
					pVideoInfo->nRealtimeDelay-=(vp->pts-lastvp->pts);
					//is->nMinRealtimeDelay=3;
					if(pVideoInfo->nRealtimeDelay>is->nMaxRealtimeDelay)
					{
						frame_queue_next(&is->pictq,true);
						is->nRealtimeDelayCount++;
						goto retry;
						//goto retry:
					}else
						is->nRealtimeDelayCount=0;
				}
			}
			

			if (vp->serial != is->videoq.serial) {
				frame_queue_next(&is->pictq,true);
				update_video_pts(is, vp->pts, vp->pos, vp->serial);
				is->redisplay=0;
				goto retry;
			}
			
			
				
				/*******时间**********/
				if (lastvp->serial != vp->serial && !is->redisplay)
				{
					is->frame_timer = av_gettime_relative() / 1000000.0;
				}

				if (is->paused)
					goto display;
				//is->frame_timer += delay;
				/******上一次更新和这一次时间的差值。图片之间差值******/
				is->last_duration = vp_duration(is, lastvp, vp);/******pts-pts********/

				if (is->redisplay)
					is->delay= 0.0;
				else/**/
				is->delay = compute_target_delay(is->last_duration, is);

#ifdef WIN32
				char abcdx[102]="";
				sprintf_s(abcdx,102,"is->delay:%f \n",is->delay);
				//OutputDebugStringA(abcdx);

#endif
				time= av_gettime_relative()/1000000.0;
				if (time < is->frame_timer + is->delay&&!is->redisplay) {
					double llxxxx=is->frame_timer + is->delay - time;
					is->remaining_time = FFMIN(llxxxx, is->remaining_time);
					goto display;
				}
	
				is->frame_timer += is->delay;
				if (is->delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
					is->frame_timer = time;


				if(!isNAN(vp->pts)&&!is->redisplay)
				{
					update_video_pts(is, vp->pts, vp->pos, vp->serial);
				}
				if (frame_queue_nb_remaining(&is->pictq) > 1)
				{
					SKK_Frame *nextvp = frame_queue_peek_next(&is->pictq);
					duration = vp_duration(is, vp, nextvp);
					if((is->redisplay ||(get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) && time > is->frame_timer + duration)
					{
						is->frame_drops_late++;
						frame_queue_next(&is->pictq,true);
						is->redisplay=0;
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
						/*if(vp->PktNumber%20==0)
						{
						  m_pAVInfomanage->UpDataAVinfo(is->filename,m_CurTime,total,(unsigned char *)vp->buffer,vp->buflen,is->viddec_width,is->viddec_height);
						}	*/	
				}
				frame_queue_next(&is->pictq,true);
				is->force_refresh=1;
	}
display:
	if(is->force_refresh&&m_pPlayUI!=NULL)
	{		
		m_pPlayUI->AVRender();
	}
	if(is->subtitle_st!=NULL)
	{

	}
	is->force_refresh=0;
}
int KKPlayer::GetPktSerial()
{
	return m_PktSerial;
}

void KKPlayer::VideoRefresh()
{
	
	if(pVideoInfo->IsReady==0||pVideoInfo->paused)
	{
		return;
	}
	
	video_image_refresh(pVideoInfo);
}
int KKPlayer::GetIsReady()
{
	if(pVideoInfo!=NULL)
	    return pVideoInfo->IsReady;
	return -1;
}

void KKPlayer::RenderImage(CRender *pRender,bool Force)
{
	SKK_Frame *vp;
	
	if(m_PlayerLock.TryLock())
	{
			if(pVideoInfo==NULL){
				
				int len=0;
				unsigned char* pBkImage=m_pPlayUI->GetCenterLogoImage(len);
				if(pBkImage!=NULL&&len>0){
					pRender->LoadCenterLogo(pBkImage,len);
					pBkImage=m_pPlayUI->GetBkImage(len);
					if(pBkImage!=NULL&&len>0){
					   pRender->renderBk(pBkImage,len);
					}
				}
			}else {
				if(pVideoInfo->IsReady==0)
				{
					int len=0;
					unsigned char* pWaitImage=m_pPlayUI->GetWaitImage(len,0);
					if(pWaitImage!=NULL)
					{
						 pRender->SetWaitPic(pWaitImage,len);
						 pRender->render(NULL,0,0,0);
					}
					
				}else{
						if(m_bOpen){
						   pVideoInfo->pictq.mutex->Lock();
						   vp =frame_queue_peek_last(&pVideoInfo->pictq);
						   if(vp->buffer!=NULL&&m_lstPts!=vp->pts||Force)
						   {
							   m_lstPts=vp->pts;
							   pRender->render((char*)vp->buffer,vp->width,vp->height,vp->pitch);
							
						   }
						   pVideoInfo->pictq.mutex->Unlock();
						 }
				   
				}
			}
			m_PlayerLock.Unlock();
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
					//strcpy(ProName,"");
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
int KKPlayer::GetRealtime()
{

	if(pVideoInfo!=NULL)
		return pVideoInfo->realtime;
	return -1;

}


//返回 1 流媒体
int is_realtime2(char *name)
{
	if(   !strcmp(name, "rtp")    || 
		!strcmp(name, "rtsp")   || 
		!strcmp(name, "sdp")
		)
		return 1;


	if(strncmp(name, "rtmp:",5)==0){
		return 1;
	}else if(!strncmp(name, "rtp:", 4)|| !strncmp(name, "udp:", 4)){
		  return 1;
	}else if(strncmp(name, "rtsp:",5)==0){
		return 1;
	}
	return 0;
}
int KKPlayer::OpenMedia(char* URL,char* Other)
{
	m_PreFileLock.Lock();
	if(m_nPreFile!=0)
	{
		m_PreFileLock.Unlock();
        return -1;
	}
	m_PreFileLock.Unlock();

	
	m_PlayerLock.Lock();
    if(m_bOpen)
	{
        m_PlayerLock.Unlock();
		return -1;
	}
	m_bOpen=true;
	
	m_PreFileLock.Lock();
	m_nPreFile=1;
	m_PreFileLock.Unlock();
	
	pVideoInfo = (SKK_VideoState*)KK_Malloc_(sizeof(SKK_VideoState));
	

	pVideoInfo->nMaxRealtimeDelay=3600;//单位s
	pVideoInfo->pKKPluginInfo=(KKPluginInfo *)KK_Malloc_(sizeof(KKPluginInfo));
	pVideoInfo->pflush_pkt =(AVPacket*)KK_Malloc_(sizeof(AVPacket));

    m_PktSerial=0;
	
	LOGE("Movie Path：\n");
	LOGE(URL);
	LOGE("\n");
	pVideoInfo->viddec.decoder_tid.ThOver=true;
	pVideoInfo->auddec.decoder_tid.ThOver=true;
	pVideoInfo->subdec.decoder_tid.ThOver=true;
	m_ReadThreadInfo.ThOver=true;
	m_VideoRefreshthreadInfo.ThOver=true;

	RECT rt;
	::GetClientRect(m_hwnd,&rt);
	
	
	av_init_packet(pVideoInfo->pflush_pkt);
	flush_pkt.data = (uint8_t *)pVideoInfo->pflush_pkt;

	memcpy(pVideoInfo->filename, URL, strlen(URL));

	pVideoInfo->realtime = is_realtime2(URL);

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
	LOGE("创建线程\n");
#ifdef WIN32_KK
	m_ReadThreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, ReadAV_thread, (LPVOID)this, 0,&m_ReadThreadInfo.Addr);

	if(m_ReadThreadInfo.ThreadHandel==0)
		assert(0);
	m_VideoRefreshthreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, VideoRefreshthread, (LPVOID)this, 0,&m_VideoRefreshthreadInfo.Addr);
    m_AudioCallthreadInfo.ThreadHandel=(HANDLE)_beginthreadex(NULL, NULL, Audio_Thread, (LPVOID)this, 0,&m_AudioCallthreadInfo.Addr);
#else
	m_ReadThreadInfo.Addr = pthread_create(&m_ReadThreadInfo.Tid_task, NULL, (void* (*)(void*))ReadAV_thread, (LPVOID)this);
	LOGE("m_ReadThreadInfo.Addr =%d\n",m_ReadThreadInfo.Addr);
	LOGE("VideoRefreshthread XX");
	m_VideoRefreshthreadInfo.Addr = pthread_create(&m_VideoRefreshthreadInfo.Tid_task, NULL, (void* (*)(void*))VideoRefreshthread, (LPVOID)this);
	m_AudioCallthreadInfo.Addr =pthread_create(&m_AudioCallthreadInfo.Tid_task, NULL, (void* (*)(void*))Audio_Thread, (LPVOID)this);
#endif
	m_lstPts=-1;

	m_AVCacheInfo.AudioSize=0;
	m_AVCacheInfo.MaxTime=0;
	m_AVCacheInfo.VideoSize=0;
	m_PlayerLock.Unlock();
		LOGE("创建线程结束\n");
	return 0;
}
void KKPlayer::OnDecelerate()
{
	int64_t seek_target =0;
	bool okk=false;
   // m_PlayerLock.Lock();
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
	//m_PlayerLock.Unlock();
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
	//m_PlayerLock.Lock();
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
	//m_PlayerLock.Unlock();

	if(okk)
	{
		KKSeek(Right,1);
		pVideoInfo->seek_req=2;
	}
}
int KKPlayer::GetAVRate()
{

	int Rate=100;
	//m_PlayerLock.Lock();
	if(pVideoInfo!=NULL)
	{
		Rate=pVideoInfo->AVRate;
	}
	//m_PlayerLock.Unlock();
	return Rate;
}
/*********视频刷新线程********/
 unsigned __stdcall KKPlayer::VideoRefreshthread(LPVOID lpParameter)
 {
	 LOGE("VideoRefreshthread strat \n");
     KKPlayer* pPlayer=(KKPlayer*)lpParameter;
	 pPlayer->m_VideoRefreshthreadInfo.ThOver=false;
	 int llxx=0;
#ifdef WIN32
	 ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#endif 
#ifdef Android_Plat
	pPlayer->m_pVideoRefreshJNIEnv=kk_jni_attach_env();
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
#ifdef Android_Plat
	kk_jni_detach_env();
	pPlayer->m_pVideoRefreshJNIEnv=NULL;
#endif
	 pPlayer->m_VideoRefreshthreadInfo.ThOver=true;
	 LOGE("VideoRefreshthread over \n");
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
		 //pVideoInfo->pKKAudio->Start();
		 while(!pVideoInfo->abort_request)
		 {
			 if(pVideoInfo->IsReady&&pVideoInfo->audio_st!=NULL)
			    pVideoInfo->pKKAudio->ReadAudio();
			 else
				 av_usleep(1000);
		 }
	 }
	 LOGE("KKPlayer Audio_Thread over \n");
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
int  KKPlayer::GetRealtimeDelay()
{
	if(pVideoInfo!=NULL)
          return pVideoInfo->nRealtimeDelay;
	return 0;
}
//强制刷新
void KKPlayer::ForceFlushQue()
{
	AvflushRealTime(1);  
	AvflushRealTime(2);  
	AvflushRealTime(3);  
}
 AVIOContext * CreateKKIo(SKK_VideoState *);
 int KKPlayer::ShowTraceAV(bool Show)
 {
		if(pVideoInfo!=NULL){
			pVideoInfo->bTraceAV=Show;
			return 1;
		}
		return 0;
 }
/*****读取视频信息******/
void KKPlayer::ReadAV()
{
	//rtmp://117.135.131.98/771/003 live=1
	m_PlayerLock.Lock();
	m_ReadThreadInfo.ThOver=false;
	LOGE("ReadAV thread start \n");
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
	
	//timeout
	if(KKProtocolAnalyze(pVideoInfo->filename,*pVideoInfo->pKKPluginInfo)==1)
	{	
	    pFormatCtx->pb=CreateKKIo(pVideoInfo);
        pFormatCtx->flags = AVFMT_FLAG_CUSTOM_IO;
		//pFormatCtx->max_delay
    }
	m_PreFileLock.Lock();
	m_nPreFile=2;
	m_PreFileLock.Unlock();
	
	if(!strncmp(pVideoInfo->filename, "rtmp:",5)){
        //rtmp 不支持 timeout
		// av_dict_set(&format_opts, "rtmp_listen", "1", AV_DICT_MATCH_CASE);
		 //av_dict_set(&format_opts, "timeout", "5", AV_DICT_MATCH_CASE);
		//av_dict_set(&format_opts, "rtmp_buffer", "0", AV_DICT_MATCH_CASE);
	}else if(!strncmp(pVideoInfo->filename, "rtsp:",5)){
      
		av_dict_set(&format_opts, "rtsp_transport", "tcp", AV_DICT_MATCH_CASE);
	}
	
	pVideoInfo->OpenTime= av_gettime ()/1000/1000;
	double Opex=0;
//	std::string urlxx=filename;
	//此函数是阻塞的
	err =avformat_open_input(
		&pVideoInfo->pFormatCtx,                    pVideoInfo->filename,
		pVideoInfo->iformat,    &format_opts);
    
	if(pVideoInfo->bTraceAV)
	   LOGE("avformat_open_input=%d,%s \n",err,pVideoInfo->filename);
	
	m_PreFileLock.Lock();
	m_nPreFile=3;
	m_PreFileLock.Unlock();




	 if(!m_bOpen)
	 {
		 m_PlayerLock.Unlock();
		 return;
	 }
    //文件打开失败
	if(err<0)
	{
		char urlx[256]="";
		strcpy(urlx,pVideoInfo->filename);
		pVideoInfo->abort_request=1;
		m_PlayerLock.Unlock();

        if(m_pPlayUI!=NULL)
		{
			m_pPlayUI->OpenMediaFailure(urlx,err);
		}else
		{
			 //if(m_bTrace)
		        LOGE("m_pPlayUI=NULL \n");
		}

		//if(m_bTrace)
		LOGE("avformat_open_input <0 \n");
		return;
		
	}
    m_PlayerLock.Unlock();

	if (scan_all_pmts_set)
		av_dict_set(&format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
	//pVideoInfo->realtime

	
	//if(m_bTrace)
	LOGE("av_format_inject_global_side_data \n");
	av_format_inject_global_side_data(pFormatCtx);
	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
		//if(m_bTrace)
		LOGE("avformat_find_stream_info<0 \n");
		char urlx[256]="";
		strcpy(urlx,pVideoInfo->filename);
		pVideoInfo->abort_request=1;
		//m_PlayerLock.Unlock();
		if(m_pPlayUI!=NULL)
		{
			m_pPlayUI->OpenMediaFailure(urlx,err);
		}else
		{
			//if(m_bTrace)
			LOGE("m_pPlayUI=NULL \n");
		}
		//if(m_bTrace)
		LOGE("avformat_find_stream_info<0 \n");
		return; // Couldn't find stream information
    }
	//if(m_bTrace)
	LOGE("avformat_find_stream_info Ok \n");
	
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
	if (start_time != AV_NOPTS_VALUE) 
	{
		int64_t timestamp;
		timestamp = start_time;
		
		/* add the stream start time */
		if (pFormatCtx->start_time != AV_NOPTS_VALUE)
			timestamp += pFormatCtx->start_time;
		ret = avformat_seek_file(pFormatCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0);
		if (ret < 0) {
			
		}
	}

	
	for (i = 0; i < pFormatCtx->nb_streams; i++) 
	{
		AVStream *st = pFormatCtx->streams[i];
		enum AVMediaType type = st->codec->codec_type;
		st_index[type] = i;
	}
	
	/* open the streams */
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) 
	{
		//if(m_bTrace)
		LOGE("AVMEDIA_TYPE_AUDIO \n");
		stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) 
	{
		//if(m_bTrace)
		LOGE("AVMEDIA_TYPE_VIDEO \n");
		ret = stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_VIDEO]);
	}

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) 
	{
		//if(m_bTrace)
		LOGE("AVMEDIA_TYPE_SUBTITLE \n");
		stream_component_open(pVideoInfo, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}

	if(pVideoInfo->iformat==NULL)
	{
		pVideoInfo->iformat=pVideoInfo->pFormatCtx->iformat;
	}
	pVideoInfo->max_frame_duration = (pVideoInfo->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;
	pVideoInfo->IsReady=1;	

	if(pVideoInfo->realtime&&pVideoInfo->audio_st==NULL)
	{
         pVideoInfo->max_frame_duration = 2.0;
	}
	
	bool fush=false;
	int avsize=0;
	

	INT64 GOP1=0;
	INT64 GOP2=0;

	
	while(m_bOpen) 
	{
		if(pVideoInfo->abort_request)
		{
			break;
		}
		if(pVideoInfo->realtime&&pVideoInfo->audio_st!=NULL)
		{

			if(pVideoInfo->audioq.size==0||pVideoInfo->videoq.size==0)
			{
                  avsize++; 
			}
			
			if(pVideoInfo->bTraceAV)
			   LOGE("de %.3fs,que audioq:%d,videoq:%d,subtitleq:%d \n",pVideoInfo->nRealtimeDelay,pVideoInfo->audioq.size,pVideoInfo->videoq.size,pVideoInfo->subtitleq.size);
			if(pVideoInfo->nRealtimeDelay<=2)//开始缓存
			{
				if(pVideoInfo->audioq.size<=1000&&pVideoInfo->videoq.size<=1000&&pVideoInfo->IsReady&&avsize>100)
				{
					pVideoInfo->IsReady=0;
					pVideoInfo->paused=1;
					avsize=0;
				}
				
			}
			/********刷新后缓存一会儿********/
			if(pVideoInfo->audioq.size>2000&&pVideoInfo->videoq.size>2000&&!pVideoInfo->IsReady)
			{
				pVideoInfo->IsReady=1;
				pVideoInfo->paused=0;
			}
		}else if(pVideoInfo->realtime&&pVideoInfo->audio_st==NULL&&pVideoInfo->video_st!=NULL)
		{
			if(pVideoInfo->videoq.size==0)
			{
				avsize++; 
			}   

			if(pVideoInfo->videoq.size<=1000&&pVideoInfo->IsReady&&avsize>100)
			{
				pVideoInfo->IsReady=0;
				pVideoInfo->paused=1;
				avsize=0;
			}
			

			if(pVideoInfo->bTraceAV)
				LOGE("de %.3fs,que audioq:%d,videoq:%d,subtitleq:%d \n",pVideoInfo->nRealtimeDelay,pVideoInfo->audioq.size,pVideoInfo->videoq.size,pVideoInfo->subtitleq.size);
			if(pVideoInfo->videoq.size>2000&& pVideoInfo->IsReady==0){
				   pVideoInfo->IsReady=1;
				   pVideoInfo->paused=0;
			}
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
				if(pVideoInfo->audio_st!=0)
                seek_max =seek_target + 1000000;
			}
			//int64_t seek_min    =pVideoInfo->seek_pos-10 * AV_TIME_BASE; //
			//int64_t seek_max    =pVideoInfo->seek_pos+10 * AV_TIME_BASE; //
			ret = avformat_seek_file(pVideoInfo->pFormatCtx, -1, seek_min, seek_target, seek_max, pVideoInfo->seek_flags);
			if (ret < 0) {
			     assert(0);
				//失败
			}else{
				Avflush(seek_target);
			}
			pVideoInfo->seek_req=0;
		}

		m_PktSerial=pVideoInfo->viddec.pkt_serial;

		

		/******缓存满了*******/
		int countxx=0;
		while(1)
		{
			if(pVideoInfo->abort_request)
			{
				break;
			}else if(pVideoInfo->audioq.size + pVideoInfo->videoq.size + pVideoInfo->subtitleq.size > MAX_QUEUE_SIZE){
				// LOGE("catch full");
				 av_usleep(5000);;//等待一会
				 countxx++;
				 if(pFormatCtx->flags == AVFMT_FLAG_CUSTOM_IO&&pVideoInfo->pKKPluginInfo!=NULL&&countxx%200==0){
					AVIOContext *KKIO=pFormatCtx->pb;
					if(KKIO!=NULL){
                         KKPlugin* kk= (KKPlugin*)KKIO->opaque;
						 if(kk->GetCacheTime!=NULL){
                                pVideoInfo->nCacheTime=kk->GetCacheTime(kk);
							    if(pVideoInfo->nCacheTime>0&&pVideoInfo->nCacheTime>m_AVCacheInfo.MaxTime)
								      m_AVCacheInfo.MaxTime=pVideoInfo->nCacheTime;
						 }
                    }
					countxx=0;
				 }
				 continue;
			}else{
				break;
			}
		}

			
        /********读取一个pkt**********/
		// LOGE("pVideoInfo->realtime %d \n",1);
		ret = av_read_frame(pFormatCtx, pkt);
		
		if(pVideoInfo->nCacheTime>0&&pVideoInfo->nCacheTime>m_AVCacheInfo.MaxTime)
			m_AVCacheInfo.MaxTime=pVideoInfo->nCacheTime;

		if(pVideoInfo->realtime&&pVideoInfo->nRealtimeDelayCount>1000){	
			 pVideoInfo->OpenTime+=pVideoInfo->nRealtimeDelay;
			 pVideoInfo->nRealtimeDelayCount=0;
		}

//		 LOGE("pVideoInfo->realtime %d \n",2);
		 //LOGE("pVideoInfo->realtime %d \n",pVideoInfo->realtime);
		if (ret < 0) {
			 if(pVideoInfo->bTraceAV)
			 LOGE("readAV ret=%d \n",ret);

			 if ((ret == AVERROR_EOF || avio_feof(pFormatCtx->pb)) && !pVideoInfo->eof)
			 {
			        if(pVideoInfo->bTraceAV) 
				    LOGE("ret == AVERROR_EOF || avio_feof(pFormatCtx->pb)) && !pVideoInfo->eof \n");
                    pVideoInfo->eof=1;
					if(pVideoInfo->realtime){ 
						
						pVideoInfo->nRealtimeDelay=0;
						pVideoInfo->abort_request=1;
						pVideoInfo->IsReady=0;
						m_pPlayUI->AutoMediaCose(-1);
						
					}
			 }else if (pFormatCtx->pb && pFormatCtx->pb->error&&ret != AVERROR_EOF){
				/* int xxx=AVERROR_EOF;
				 xxx=-541478725;
				*/
					 pVideoInfo->eof=1;
					 pVideoInfo->abort_request=1;
					 pVideoInfo->nRealtimeDelay=0;
					 if(pVideoInfo->realtime)
					 {
						 pVideoInfo->IsReady=0;
						 m_pPlayUI->AutoMediaCose(-2);
					 }else {
						 m_pPlayUI->AutoMediaCose(pFormatCtx->pb->error);
					 }
				 if(pVideoInfo->bTraceAV)
					 LOGE("pFormatCtx->pb && pFormatCtx->pb->error \n");
				 break;
				
			 }
			av_usleep(10000);
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
		if (pkt->stream_index == pVideoInfo->audio_stream && pkt_in_play_range&&pkt->data!=NULL) {
			packet_queue_put(&pVideoInfo->audioq, pkt,pVideoInfo->pflush_pkt);
			
			m_AVCacheInfo.AudioSize=pVideoInfo->audioq.PktMemSize;
			if(pVideoInfo->audio_st!=NULL)
			{
				pkt_ts=pkt_ts*av_q2d(pVideoInfo->audio_st->time_base);
			}
			if(m_AVCacheInfo.MaxTime<pkt_ts)
			{
				m_AVCacheInfo.MaxTime=pkt_ts;
			}
		} //视频
		else if (
			pkt->stream_index == pVideoInfo->video_stream && pkt_in_play_range
			&& !(pVideoInfo->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC&&pkt->data!=NULL)
			) {
			
			packet_queue_put(&pVideoInfo->videoq, pkt,pVideoInfo->pflush_pkt);
			m_AVCacheInfo.VideoSize=pVideoInfo->videoq.PktMemSize;
			if(pVideoInfo->video_st!=NULL){
				pkt_ts=pkt_ts*av_q2d(pVideoInfo->video_st->time_base);
			}
			if(m_AVCacheInfo.MaxTime<pkt_ts)
			{
                 m_AVCacheInfo.MaxTime=pkt_ts;
			}
		}//字幕
		else if (pkt->stream_index == pVideoInfo->subtitle_stream && pkt_in_play_range&&pkt->data!=NULL) 
		{
			//printf("subtitleq\n");
			packet_queue_put(&pVideoInfo->subtitleq, pkt,pVideoInfo->pflush_pkt);
		} else
		{
			av_packet_unref(pkt);
		}
		
	}
	
	
	LOGE("readAV Over \n");
	
}
//释放队列数据
void KKPlayer::PacketQueuefree()
{
   if(pVideoInfo!=NULL)
   {
	    LOGE("pVideoInfo->videoq \n");
		if(pVideoInfo->videoq.pLock->TryLock())
		{
              pVideoInfo->videoq.pLock->Unlock();
		}else{
               pVideoInfo->videoq.pLock->Unlock();
		}
	    packet_queue_flush(&pVideoInfo->videoq);

		LOGE("pVideoInfo->audioq \n");
		if(pVideoInfo->audioq.pLock->TryLock())
		{
			pVideoInfo->audioq.pLock->Unlock();
		}else{
			pVideoInfo->audioq.pLock->Unlock();
		}
		packet_queue_flush(&pVideoInfo->audioq);

		LOGE("pVideoInfo->subtitleq \n");
		if(pVideoInfo->subtitleq.pLock->TryLock())
		{
			pVideoInfo->subtitleq.pLock->Unlock();
		}else{
			pVideoInfo->subtitleq.pLock->Unlock();
		}
		packet_queue_flush(&pVideoInfo->subtitleq);


		LOGE("pVideoInfo->pictq \n");
        frame_queue_destory(&pVideoInfo->pictq);

		LOGE("pVideoInfo->subpq \n");
		frame_queue_destory(&pVideoInfo->subpq);

		LOGE("pVideoInfo->sampq \n");
		frame_queue_destory(&pVideoInfo->sampq);
   }
}
void KKPlayer::SetVolume(long value)
{
	if(pVideoInfo!=NULL&&pVideoInfo->audio_st!=NULL)
   m_pSound->SetVolume(value);
}
long KKPlayer::GetVolume()
{
	if(pVideoInfo!=NULL&&pVideoInfo->audio_st!=NULL)
	     return m_pSound->GetVolume();
	return 0;
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
 
   if(pVideoInfo!=NULL&&pVideoInfo->IsReady)
   {
	   double incr, pos, frac;
	   incr=value;
	   pos = get_master_clock(pVideoInfo);
	  /* if(pVideoInfo->AVRate!=100)
	   {
		   float aa=(float)pVideoInfo->AVRate/100;
		   pos=pos*aa;
	   }*/
	   if (isNAN(pos))
		   pos = (double)pVideoInfo->seek_pos / AV_TIME_BASE;
	   incr+=3;
	   if (pVideoInfo->pFormatCtx->start_time != AV_NOPTS_VALUE && pos < pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE)
		   pos = pVideoInfo->pFormatCtx->start_time / (double)AV_TIME_BASE;
	   stream_seek(pVideoInfo, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
	   
   }
   //m_PlayerLock.Lock();
   //m_PlayerLock.Unlock();
}

void KKPlayer::AVSeek(int value)
{
	
	if(pVideoInfo!=NULL&&pVideoInfo->IsReady)
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
}

void KKPlayer::AvflushRealTime(int Avtype)
{
	if(pVideoInfo==NULL)
		return;
	if (pVideoInfo->video_stream >= 0&&Avtype==1) 
	{
		packet_queue_put(&pVideoInfo->videoq, pVideoInfo->pflush_pkt,pVideoInfo->pflush_pkt);
	}

	if (pVideoInfo->audio_st!=NULL&&pVideoInfo->audio_stream >= 0&&Avtype==2) 
	{
		packet_queue_put(&pVideoInfo->audioq,pVideoInfo->pflush_pkt, pVideoInfo->pflush_pkt);
	}
	if (pVideoInfo->subtitle_stream >= 0&&Avtype==3) 
	{
		packet_queue_put(&pVideoInfo->subtitleq, pVideoInfo->pflush_pkt,pVideoInfo->pflush_pkt);
	}
}

/******设置实时流媒体最小延迟**********/
int KKPlayer::SetMaxRealtimeDelay(int Delay)
{
     if(pVideoInfo!=NULL&&Delay>=1)
	 {
		 pVideoInfo->nMaxRealtimeDelay=Delay;
		 return 1;
	 }
	 return 0;
}
void KKPlayer::Avflush(int64_t seek_target)
{
	if (pVideoInfo->video_stream >= 0) 
	{
		packet_queue_put(&pVideoInfo->videoq, pVideoInfo->pflush_pkt,pVideoInfo->pflush_pkt);
		pVideoInfo->pictq.mutex->Lock();
		pVideoInfo->pictq.size=0;
		pVideoInfo->pictq.rindex=0;
		pVideoInfo->pictq.windex=0;
		pVideoInfo->pictq.rindex_shown=1;

		pVideoInfo->pictq.m_pWaitCond->SetCond();
		pVideoInfo->pictq.mutex->Unlock();
	}

	if (pVideoInfo->audio_st!=NULL&&pVideoInfo->audio_stream >= 0) 
	{
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

#ifdef WIN32
#include <emmintrin.h>
#include <smmintrin.h>
void* gpu_memcpy(void* d, const void* s, size_t size)
{
	static const size_t regsInLoop = sizeof(size_t) * 2; // 8 or 16

	if (d == NULL|| s == NULL) 
		return NULL;

	// If memory is not aligned, use memcpy
	bool isAligned = (((size_t)(s) | (size_t)(d)) & 0xF) == 0;
	if (!isAligned)
	{
		return memcpy(d, s, size);
	}

	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
#ifdef _M_X64
	__m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
#endif

	size_t reminder = size & (regsInLoop * sizeof(xmm0) - 1); // Copy 128 or 256 bytes every loop
	size_t end = 0;

	__m128i* pTrg = (__m128i*)d;
	__m128i* pTrgEnd = pTrg + ((size - reminder) >> 4);
	__m128i* pSrc = (__m128i*)s;

	// Make sure source is synced - doesn't hurt if not needed.
	_mm_sfence();

	while (pTrg < pTrgEnd)
	{
		// _mm_stream_load_si128 emits the Streaming SIMD Extensions 4 (SSE4.1) instruction MOVNTDQA
		// Fastest method for copying GPU RAM. Available since Penryn (45nm Core 2 Duo/Quad)
		xmm0  = _mm_stream_load_si128(pSrc);
		xmm1  = _mm_stream_load_si128(pSrc + 1);
		xmm2  = _mm_stream_load_si128(pSrc + 2);
		xmm3  = _mm_stream_load_si128(pSrc + 3);
		xmm4  = _mm_stream_load_si128(pSrc + 4);
		xmm5  = _mm_stream_load_si128(pSrc + 5);
		xmm6  = _mm_stream_load_si128(pSrc + 6);
		xmm7  = _mm_stream_load_si128(pSrc + 7);
#ifdef _M_X64 // Use all 16 xmm registers
		xmm8  = _mm_stream_load_si128(pSrc + 8);
		xmm9  = _mm_stream_load_si128(pSrc + 9);
		xmm10 = _mm_stream_load_si128(pSrc + 10);
		xmm11 = _mm_stream_load_si128(pSrc + 11);
		xmm12 = _mm_stream_load_si128(pSrc + 12);
		xmm13 = _mm_stream_load_si128(pSrc + 13);
		xmm14 = _mm_stream_load_si128(pSrc + 14);
		xmm15 = _mm_stream_load_si128(pSrc + 15);
#endif
		pSrc += regsInLoop;
		// _mm_store_si128 emit the SSE2 intruction MOVDQA (aligned store)
		_mm_store_si128(pTrg     , xmm0);
		_mm_store_si128(pTrg +  1, xmm1);
		_mm_store_si128(pTrg +  2, xmm2);
		_mm_store_si128(pTrg +  3, xmm3);
		_mm_store_si128(pTrg +  4, xmm4);
		_mm_store_si128(pTrg +  5, xmm5);
		_mm_store_si128(pTrg +  6, xmm6);
		_mm_store_si128(pTrg +  7, xmm7);
#ifdef _M_X64 // Use all 16 xmm registers
		_mm_store_si128(pTrg +  8, xmm8);
		_mm_store_si128(pTrg +  9, xmm9);
		_mm_store_si128(pTrg + 10, xmm10);
		_mm_store_si128(pTrg + 11, xmm11);
		_mm_store_si128(pTrg + 12, xmm12);
		_mm_store_si128(pTrg + 13, xmm13);
		_mm_store_si128(pTrg + 14, xmm14);
		_mm_store_si128(pTrg + 15, xmm15);
#endif
		pTrg += regsInLoop;
	}

	// Copy in 16 byte steps
	if (reminder >= 16)
	{
		size = reminder;
		reminder = size & 15;
		end = size >> 4;
		for (size_t i = 0; i < end; ++i)
		{
			pTrg[i] = _mm_stream_load_si128(pSrc + i);
		}
	}

	// Copy last bytes - shouldn't happen as strides are modulu 16
	if (reminder)
	{
		__m128i temp = _mm_stream_load_si128(pSrc + end);

		char* ps = (char*)(&temp);
		char* pt = (char*)(pTrg + end);

		for (size_t i = 0; i < reminder; ++i)
		{
			pt[i] = ps[i];
		}
	}

	return d;
}
#endif