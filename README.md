KKPlayer一个基于ffmpeg的视频播放器，目前移植到windows，android
希望大家多多交流 QQ群：342276504
windows：vs2008编译通过。
Android:android studio1.5 编译通过。
项目组成 
         KKPlayerCore  播放器核心代码
		 KKplayer项目是基于KKPlayerCore写的Windows例子
		 KKPlayerAndroid项目是基于KKPlayerCore写的android例子
         Windows版本：生成的正式文件在release下，调试的在debug下。界面库采用SOUI2
		 IOS版本正在筹划中.
windows 编译需要定义环境变量
KKWinSdkPath和KKWinDx9Path
例如
KKWinDx9Path=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)
KKWinSdkPath=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A

rtmpt延迟格式控制请使用以下格式srs:rtmp:xxxxxxxxxxxxxxxxxxxxxx
		
