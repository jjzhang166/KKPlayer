//
// Created by saint on 2016/3/10.
//

#include <malloc.h>
#include <string.h>
#include "AndKKAudio.h"
#include "ffmpeg2.8.6/include/libavutil/common.h"
#include <time.h>
#define  LOG_TAG    "XlibOpenLSjni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
static const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
CAndKKAudio::CAndKKAudio():m_engineObject(NULL),m_engineEngine(NULL),m_outputMixObject(NULL)
{
    m_pFun=NULL;
    m_nBufLength=1024*3*4;
    //4096
    m_pBuf=::malloc( m_nBufLength);
    if(m_pBuf!=NULL)
    memset(m_pBuf,0,m_nBufLength);

    m_pNext_buffer=m_pBuf;
    m_nnext_buffer_index=0;
    bytes_per_buffer=1024*3;

    m_bqPlayerPlay=NULL;
    m_bqPlayerObject=NULL;

    m_AudioClose=0;
}
CAndKKAudio::~CAndKKAudio()
{

    CloseAudio();
    if(m_outputMixObject!=NULL)
    {
        (*m_outputMixObject)->Destroy(m_outputMixObject);
        m_outputMixObject=NULL;
        LOGI("m_outputMixObject Destroy\n");

    }
    if(m_engineObject!=NULL)
    {
        (*m_engineObject)->Destroy(m_engineObject);
        m_engineObject=NULL;
        m_engineEngine=NULL;
        LOGI("m_engineObject Destroy\n");
    }
    ::free(m_pBuf);
    m_pBuf=NULL;
    LOGE("~CAndKKAudio");
}


// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    CAndKKAudio *pAudio=(CAndKKAudio*)context;
    pAudio->SetCond();
}
void CAndKKAudio::SetCond()
{
    m_ReadLock.Lock();
   // LOGE("SetCond,%d",m_nnext_buffer_index);
    m_nReadWait= false;
    //m_ReadLockCond.SetCond();
    m_ReadLock.Unlock();
}
unsigned long GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
void CAndKKAudio::SetWindowHAND(int hwnd)
{

    m_hwnd=hwnd;
    SLresult result;
    m_ReadLock.Lock();
    m_AudioClose=0;
    m_ReadLock.Unlock();

    // create engine创建一个引擎对象
    result = slCreateEngine(&m_engineObject, 0, NULL, 0, NULL, NULL);

    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("m_engineObject error");
    }
    (void)result;

    // realize the engine，实现一个引擎
    result = (*m_engineObject)->Realize(m_engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE(" Realize m_engineObject error");
    }
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*m_engineObject)->GetInterface(m_engineObject, SL_IID_ENGINE, &m_engineEngine);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("GetInterface m_engineObject error,%d",result);
    }
    (void)result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids1[] = {SL_IID_VOLUME};
    const SLboolean req1[] = {SL_BOOLEAN_FALSE};
    LOGE("create output mix, with environmental reverb specified as a non-required interface");
    result = (*m_engineEngine)->CreateOutputMix(m_engineEngine, &m_outputMixObject, 1, ids1, req1);

    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("CreateOutputMix m_engineEngine error,%d", result);
    }
    (void)result;

    // realize the output mix
    LOGE("realize the output mix");
    result = (*m_outputMixObject)->Realize(m_outputMixObject, SL_BOOLEAN_FALSE);

    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("Realize m_outputMixObject eror");
    }

}
void CAndKKAudio::SetUserData(void* UserData)
{
   m_UserData=UserData;
}
/********设置音频回到函数*********/
void CAndKKAudio::SetAudioCallBack(pfun fun)
{
    m_ReadLock.Lock();
    m_pFun=fun;
    m_ReadLock.Unlock();
}
/***********初始化音频设备*********/
int CAndKKAudio::OpenAudio( int &wanted_channel_layout, int &wanted_nb_channels,int &wanted_sample_rate)
{
    SLresult result;
    int lll=1024*4;
	SetCond();
    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,4};
    SLDataFormat_PCM format_pcm;
    wanted_channel_layout=3;
    wanted_nb_channels=2;
    format_pcm.formatType=SL_DATAFORMAT_PCM;
    format_pcm.numChannels=2;
    format_pcm.samplesPerSec=wanted_sample_rate*1000;//


    SLuint32 lxs[]={ SL_SAMPLINGRATE_8,SL_SAMPLINGRATE_11_025	,SL_SAMPLINGRATE_12	,
                     SL_SAMPLINGRATE_16	,SL_SAMPLINGRATE_22_05, SL_SAMPLINGRATE_24,
                     SL_SAMPLINGRATE_32,SL_SAMPLINGRATE_44_1	,SL_SAMPLINGRATE_48	,
                     SL_SAMPLINGRATE_64	, SL_SAMPLINGRATE_88_2,SL_SAMPLINGRATE_96,
                     SL_SAMPLINGRATE_192};
    int next_sample_rate_idx = FF_ARRAY_ELEMS(lxs) - 1;
    while (next_sample_rate_idx >0)
    {

        if(lxs[next_sample_rate_idx]==format_pcm.samplesPerSec)
        {
            break;
        }
        next_sample_rate_idx--;
    }
    if(next_sample_rate_idx<0)
        format_pcm.samplesPerSec=SL_SAMPLINGRATE_44_1;//

    format_pcm.bitsPerSample=SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.containerSize= SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.channelMask=SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    format_pcm.endianness=SL_BYTEORDER_LITTLEENDIAN;

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, m_outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};



    // create audio playerSL_IID_ANDROIDSIMPLEBUFFERQUEUE
    // //SL_IID_BUFFERQUEUE
    const SLInterfaceID ids2[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE , SL_IID_VOLUME, SL_IID_PLAY };
    static const SLboolean req2[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	
	 if(m_bqPlayerObject!=NULL){
        (*m_bqPlayerObject)->Destroy(m_bqPlayerObject);
        m_bqPlayerObject=NULL;
        LOGI("m_bqPlayerObject Destroy\n");
    }
	
    result = (*m_engineEngine)->CreateAudioPlayer(m_engineEngine, &m_bqPlayerObject, &audioSrc, &audioSnk,
                                                  sizeof(ids2) / sizeof(*ids2),ids2, req2);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE(" CreateAudioPlayer m_engineObject m_bqPlayerObject error");
        return -1;
    }
    (void)result;

    // realize the player
    result = (*m_bqPlayerObject)->Realize(m_bqPlayerObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("Realize m_bqPlayerObject error");
    }
    (void)result;

    // get the play interface
    result = (*m_bqPlayerObject)->GetInterface(m_bqPlayerObject, SL_IID_PLAY, &m_bqPlayerPlay);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("GetInterface m_bqPlayerObject m_bqPlayerPlay error");
    }
    (void)result;
    // get the volume interface
    result = (*m_bqPlayerObject)->GetInterface(m_bqPlayerObject, SL_IID_VOLUME, &m_bqPlayerVolume);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("GetInterface m_bqPlayerObject m_bqPlayerVolume error，%d",result);
    }

    (void)result;
    // get the buffer queue interface SL_IID_BUFFERQUEUE
    result = (*m_bqPlayerObject)->GetInterface(m_bqPlayerObject,SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                               &m_bqPlayerBufferQueue);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("GetInterface m_bqPlayerObject m_bqPlayerBufferQueue error，%d",result);
    }
    (void)result;

    // register callback on the buffer queue
    result = (*m_bqPlayerBufferQueue)->RegisterCallback(m_bqPlayerBufferQueue, bqPlayerCallback, this);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("RegisterCallback m_bqPlayerBufferQueue m_bqPlayerBufferQueue error，%d",result);
    }

    (void)result;
    // set the player's state to playing
    result = (*m_bqPlayerPlay)->SetPlayState(m_bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("SetPlayState m_bqPlayerPlay SL_PLAYSTATE_PLAYING error，%d",result);
    }
    (void)result;
    return 0;
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example
}
/*******读取音频数据********/
void CAndKKAudio::ReadAudio()
{
    m_ReadLock.Lock();
    if(m_AudioClose==1)
    {
        m_ReadLock.Unlock();
        return;
    }
    //LOGI("ReadAudio\n");
    m_ReadLock.Unlock();
    if(m_pFun!=NULL&&m_bqPlayerPlay!=NULL)
    {
        long t_start, t_end;
        t_start =GetTickCount();
      //  LOGE("ReadAudio");
        m_pFun(m_UserData,(char*)m_pNext_buffer,bytes_per_buffer);
     //   LOGE("ReadAudioEnd");
        //t=m_nBufLength/(采样频率（Hz）*采样位数（bit）*??声道数)
        SLresult result;
        SLuint32 lx=0;
        result = (*m_bqPlayerPlay)->GetPlayState(m_bqPlayerPlay,&lx);
        if(SL_RESULT_SUCCESS == result&&lx!=SL_PLAYSTATE_PLAYING)
        {
           result = (*m_bqPlayerPlay)->SetPlayState(m_bqPlayerPlay, SL_PLAYSTATE_PLAYING);
           if(SL_RESULT_SUCCESS != result)
           {
                LOGE("SetPlayState m_bqPlayerPlay SL_PLAYSTATE_PLAYING error，%d",result);
           }
        }
        if(m_bqPlayerBufferQueue!=NULL)
        {
            m_ReadLock.Lock();
            m_nReadWait=true;
            m_ReadLock.Unlock();
            result = (* m_bqPlayerBufferQueue)->Enqueue(m_bqPlayerBufferQueue, m_pNext_buffer,bytes_per_buffer);
            //int t=m_nBufLength*10000/1764;
            if(SL_RESULT_SUCCESS != result)
            {
                   LOGE(" m_bqPlayerBufferQueue)->Enqueue(m_bqPlayerBufferQueue error");
            }
            //LOGE("WaitCond");
            //m_ReadLockCond.WaitCond(1);
            m_ReadLock.Lock();
            while (m_nReadWait&&!m_AudioClose)
            {
                     m_ReadLock.Unlock();
                     // LOGE("WaitCond");
                     usleep(400);
                     m_ReadLock.Lock();
            }
            m_ReadLock.Unlock();
        }
        t_end = GetTickCount();
        //LOGE("audio:%d,%d,%d,%d",t_end-t_start,m_nnext_buffer_index,lx,SL_PLAYSTATE_PLAYING);
        m_pNext_buffer = m_pBuf + m_nnext_buffer_index * bytes_per_buffer;
        m_nnext_buffer_index = (m_nnext_buffer_index + 1) % 4;
    }
}
void CAndKKAudio::Start()
{
   if(m_bqPlayerPlay!=NULL)
   {
       m_ReadLock.Lock();
       m_AudioClose=0;
       m_ReadLock.Unlock();
       SLresult result= (*m_bqPlayerPlay)->SetPlayState(m_bqPlayerPlay, SL_PLAYSTATE_PLAYING);
   }
}
void CAndKKAudio::Stop()
{
    m_ReadLock.Lock();
    m_AudioClose=1;
    m_ReadLock.Unlock();
    SetCond();
    if(m_bqPlayerPlay!=NULL)
    {
        LOGI("m_bqPlayerPlay stop\n");
        SLresult result= (*m_bqPlayerPlay)->SetPlayState(m_bqPlayerPlay,  SL_PLAYSTATE_STOPPED);
    }
}
/*********关闭**********/
void CAndKKAudio::CloseAudio()
{
    Stop();
    LOGI("CloseAudio Stop m_bqPlayerObject=%d\n",m_bqPlayerObject);

    if(m_bqPlayerObject!=NULL){
        (*m_bqPlayerObject)->Destroy(m_bqPlayerObject);
        m_bqPlayerObject=NULL;
        m_bqPlayerPlay=NULL;
        m_bqPlayerBufferQueue=NULL;
        m_bqPlayerVolume=NULL;
        LOGI("m_bqPlayerObject Destroy\n");
    }

    LOGI("CloseAudio Over\n");
}
/*********设置音量************/
void CAndKKAudio::SetVolume(long value)
{

}
long CAndKKAudio::GetVolume()
{
    return 0;
}