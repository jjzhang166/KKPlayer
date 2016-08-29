#include "../KKPlayer.h"

extern "C"{
	    
		int Srs_read_packet(void *opaque, uint8_t *buf, int buf_size);
		KKPlugin* CreateSrsRtmpPlugin()
		{
			KKPlugin* srs_rtmp_plugin =(KKPlugin* ) ::malloc(sizeof(KKPlugin));
			memset(srs_rtmp_plugin,0,sizeof(KKPlugin));
            srs_rtmp_plugin->RealTime=1;
			srs_rtmp_plugin->kkread=Srs_read_packet;
			return  srs_rtmp_plugin;

		}
		void DelSrsRtmpPlugin(KKPlugin*p);

};

void AddSrsRtmpPluginInfo()
{
	KKPluginInfo PluInfo;
	memset(&PluInfo,0,sizeof(KKPluginInfo));

	PluInfo.CreKKP=CreateSrsRtmpPlugin;
	PluInfo.DelKKp=DelSrsRtmpPlugin;
	strcpy(PluInfo.ptl,"srs");
	KKPlayer::AddKKPluginInfo(PluInfo);
}