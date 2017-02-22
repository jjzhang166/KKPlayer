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

		

ffmpeg3.0 以上版本Android编译
如果直接按照未修改的配置进行编译，结果编译出来的so文件类似libavcodec.so.55.39.101，版本号位于so之后，Android上似乎无法加载。
因此需要按如下修改：
将configure文件中的如下四行：
SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)'
LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'
SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)'
SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR)$(SLIBNAME)'
替换为：
SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(SLIBSUF)'
LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'
SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)'
SLIB_INSTALL_LINKS='$(SLIBNAME)'

sh：
#!/bin/bash
export TMPDIR=/home/ubuntu64/FFmpeg/Tmep
NDK=/home/ubuntu64/FFmpeg/android-ndk-r10
SYSROOT=$NDK/platforms/android-9/arch-arm/
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64
function build_one
{
./configure \
--prefix=/home/ubuntu64/FFmpeg/ff3.1.2Jni \
--enable-shared \
--disable-static \
--disable-doc \
--disable-ffserver \
--enable-cross-compile \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--target-os=linux \
--arch=arm \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic $ADDI_CFLAGS" \
--extra-ldflags="$ADDI_LDFLAGS" \
--enable-protocol=rtmp
$ADDITIONAL_CONFIGURE_FLAG
}
CPU=arm
PREFIX=$(pwd)/android/$CPU
ADDI_CFLAGS="-marm"
build_one





windowVc(Vs2010)编译ffmpeg(https://ffmpeg.org/platform.html#Microsoft-Visual-C_002b_002b-or-Intel-C_002b_002b-Compiler-for-Windows)
./configure的3580行修改为：
        if [ -z "$cl_major_ver" ] || [ $cl_major_ver -ge 18 ]; then
            cc_default="cl"
        else
            cc_default="c99wrap cl"
        fi
		cc_default="c99wrap cl"
例如:
./configure --toolchain=msvc --extra-cflags='-IC:/msinttypes' --enable-shared