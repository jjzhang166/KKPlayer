#include "SDLSound.h"

#include "../KKPlayerCore/Includeffmpeg.h"

#pragma comment (lib,"../libx86/SDL1.2.15/lib/x86/SDL.lib")
CSDLSound::CSDLSound()
{
    m_UserData=NULL;
	m_pFun=NULL;
}
CSDLSound::~CSDLSound()
{

}
void CSDLSound::SetWindowHAND(int m_hwnd){}
void CSDLSound::SetUserData(void* UserData){

	m_UserData=UserData;

}
	 /********设置音频回到函数*********/
void CSDLSound::SetAudioCallBack(pfun fun)
{
	m_pFun=fun;
}
	 /***********初始化音频设备*********/

void sdl_audio_callback(void *userdata, Uint8 *stream, int len)
{
	 CSDLSound* m_pFun=(CSDLSound* )userdata;
	
	m_pFun->KKSDLCall( stream,len);
}

void  CSDLSound::KKSDLCall(Uint8 *stream, int len)
{
	if(m_pFun!=NULL&&m_UserData!=NULL)
	{
		//memset(buf,0,buf_len);
		m_pFun(m_UserData,(char*)stream,len);
		//RaiseVolume((char*)buf, buf_len, 1, 5);
	}
}
void CSDLSound::InitAudio()
{

	int flags = SDL_INIT_AUDIO | SDL_INIT_TIMER;

	if (SDL_Init (flags)) 
	{
	     return;	
	}

	int wanted_sample_rate=44100;
	int wanted_nb_channels =2;
	int wanted_channel_layout =0;
	SDL_AudioSpec wanted_spec, spec;
	const char *env;
	static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
	static const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env) 
	{
		wanted_nb_channels = atoi(env);
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
	}
	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}
	
	
	wanted_spec.channels = wanted_nb_channels;
	wanted_spec.freq = wanted_sample_rate;
	if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) 
	{
		
		return;
	}
	while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
		next_sample_rate_idx--;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	int ll=SDL_AUDIO_MIN_BUFFER_SIZE;
	int xx= 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC);
	wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
	wanted_spec.samples /=4;
	wanted_spec.callback =sdl_audio_callback;
	wanted_spec.userdata = this;
	
	while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
	
		wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
		if (!wanted_spec.channels) {
			wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
			wanted_spec.channels = wanted_nb_channels;
			if (!wanted_spec.freq) {
				
				return;
			}
		}
		wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
	}
	if (spec.format != AUDIO_S16SYS) 
	{
		
		return;
	}
	int ii=0;
	ii++;
	SDL_PauseAudio(1);
}
	 /*******读取音频数据********/
	 void CSDLSound::ReadAudio()
	 {
		 Sleep(10);
	 }
	 void CSDLSound::Start(){
		 SDL_PauseAudio(0);
	 } 
	 void CSDLSound::Stop(){
		 SDL_PauseAudio(1);
	 }   
	 /*********关闭**********/
	 void CSDLSound::CloseAudio()
	 {
         SDL_CloseAudio();
	 }	
	 /*********设置音量************/
	 void CSDLSound::SetVolume(long value){}
	 long CSDLSound::GetVolume(){
		 return 1;
	 }