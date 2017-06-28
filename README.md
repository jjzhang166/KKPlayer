KKPlayer一个基于ffmpeg的视频播放器，目前移植到windows，android <br/>
希望大家多多交流 QQ群：342276504 <br/>
windows：vs2008编译通过。<br/>
Android:android studio1.5 编译通过。<br/>
项目组成 <br/>
         KKPlayerCore  播放器核心代码<br/>
		 KKplayer项目是基于KKPlayerCore写的Windows例子<br/>
		 KKPlayerAndroid项目是基于KKPlayerCore写的android例子<br/>
         Windows版本：生成的正式文件在release下，调试的在debug下。界面库采用SOUI2<br/>
		 IOS版本正在筹划中.<br/>
windows 编译需要定义环境变量<br/>
KKWinSdkPath,KKWinDx9Path和KKSoui2Path<br/>
dxsdk10(d3d9)下载地址：http://pan.baidu.com/s/1kV0vmVP<br/>
soui2下载地址：https://git.oschina.net/Qyerp/soui<br/>
例如<br/>
KKWinDx9Path=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)<br/>
KKWinSdkPath=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A<br/>
KKSoui2Path=D:\work\Projects\soui<br/>
项目生成顺序:<br/>
 libmfx(inter QSV编码)->zlibstat->png->skia->sdl->render->kkplayercore->kkui->jsoncpp->kkplayer<br/>
rtmpt延迟格式控制请使用以下格式librtmp:rtmp:xxxxxxxxxxxxxxxxxxxxxx<br/>

		

ffmpeg3.0 以上版本Android编译<br/>
如果直接按照未修改的配置进行编译，结果编译出来的so文件类似libavcodec.so.55.39.101，版本号位于so之后，Android上似乎无法加载。<br/>
因此需要按如下修改：<br/>
将configure文件中的如下四行：<br/>
SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)'<br/>
LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'<br/>
SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)'<br/>
SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR)$(SLIBNAME)'<br/>
替换为：<br/>
SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(SLIBSUF)'<br/>
LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'<br/>
SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)'<br/>
SLIB_INSTALL_LINKS='$(SLIBNAME)'<br/>

sh：<br/>
#!/bin/bash<br/>
export TMPDIR=/home/ubuntu64/FFmpeg/Tmep<br/>
NDK=/home/ubuntu64/FFmpeg/android-ndk-r10<br/>
SYSROOT=$NDK/platforms/android-9/arch-arm/<br/>
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64<br/>
function build_one<br/>
{<br/>
./configure \<br/>
--prefix=/home/ubuntu64/FFmpeg/ff3.1.2Jni \<br/>
--enable-shared \<br/>
--disable-static \<br/>
--disable-doc \<br/>
--disable-ffserver \<br/>
--enable-cross-compile \<br/>
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \<br/>
--target-os=linux \<br/>
--arch=arm \<br/>
--disable-encoders \<br/>
--sysroot=$SYSROOT \<br/>
--extra-cflags="-Os -fpic $ADDI_CFLAGS" \<br/>
--extra-ldflags="$ADDI_LDFLAGS" \<br/>
--enable-protocol=rtmp<br/>
$ADDITIONAL_CONFIGURE_FLAG<br/>
}<br/>
CPU=arm<br/>
PREFIX=$(pwd)/android/$CPU<br/>
ADDI_CFLAGS="-marm"<br/>
build_one<br/>





windowVc(Vs2010)编译ffmpeg(https://ffmpeg.org/platform.html#Microsoft-Visual-C_002b_002b-or-Intel-C_002b_002b-Compiler-for-Windows) <br/>
./configure的3580行修改为：<br/>
        if [ -z "$cl_major_ver" ] || [ $cl_major_ver -ge 18 ]; then <br/>
            cc_default="cl"<br/>
        else<br/>
            cc_default="c99wrap cl"<br/>
        fi<br/>
		cc_default="c99wrap cl"<br/>
例如(启用共享库，关闭所有编码器):<br/>
./configure --toolchain=msvc --extra-cflags='-IC:/msinttypes' --prefix='./ffmpegBin'   --incdir='./ffmpegBin/incdir' --enable-shared --disable-encoders<br/>
更新日志：<br/>
2017-5-23	<br>
&nbsp;&nbsp;&nbsp;&nbsp; 移除srslibrtmpplugin. 添加librtmpplugin<br/>
&nbsp;&nbsp;&nbsp;&nbsp; 支持视频分片(像优酷那种视频分片)<br/>
&nbsp;&nbsp;&nbsp;&nbsp; 整合FFmpeg的mediacode解码<br/>
&nbsp;&nbsp;&nbsp;&nbsp; intermedia h264解码<br/>
&nbsp;&nbsp;&nbsp;&nbsp; ffmpge更新到3.3.2<br/>
2017-5-30   <br/>
&nbsp;&nbsp;&nbsp;&nbsp; windows 添加libkkplayer库。支持无窗口dui渲染，或者以窗口进行渲染 <br/>
&nbsp;&nbsp;&nbsp;&nbsp; 修改sdl1支持同时创建多个音频渲染器 <br/>
&nbsp;&nbsp;&nbsp;&nbsp; 修正渲染器一些bug <br/>