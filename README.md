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
		 对于rtmp有实时性要求请使用例如这样的格式srs:rtmp://182.92.80.26:1935/live/livestream。
		 在openmedia调用后，调用SetMaxRealtimeDelay(int Delay)方法设置最大延迟。
