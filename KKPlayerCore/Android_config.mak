ifndef KKPlayer_CONFIG_MAK
FFMPEG_CONFIG_MAK=1
#此处需要修改
NDK_PATH=D:/ProgramFiles/Android/Androidsdk/ndk-bundle
#此处需要修改
FFMPEGLib=D:/work/Projects/KKPlayer/libarm/ffmpeg/lib/
#此处需要修改
FFMPEGINCLUDE=D:/work/Projects/KKPlayer/ffmpeg/include
#此处需要修改 windows/bin
COMPILERDIR=windows-x86_64/bin
ARM_LINUX_ANDROIDE_API=arm-linux-androideabi-4.9
PLATFORMS=android-9
BASELib=$(NDK_PATH)/platforms/android-9/arch-arm/usr/lib
STLLib=$(NDK_PATH)/sources/cxx-stl/stlport/libs/armeabi/
CFLAGS = -I$(NDK_PATH)/platforms/$(PLATFORMS)/arch-arm/usr/include -I$(NDK_PATH)/sources/cxx-stl/stlport/stlport -I$(FFMPEGINCLUDE)
#LDFLAGS = -L$(FFMPEGLib) -nostdlib
#LIBS = 	-llibavcodec-56.so -llibavdevice-56.so -llibavfilter-5.so -llibavformat-56.so -llibavutil-54.so -llibswresample-1.so -llibswscale-3.so 
#-std=c99
endif
#f:/ProgramTool/OpenPro/KKPlayer/KKPlayerCore
#f:/android/android-sdk-windows/ndk-bundle/sources/cxx-stl/stlport/stlport/include
#F:\android\android-sdk-windows\ndk-bundle\sources\android\support\include
#F:\android\android-sdk-windows\ndk-bundle\sources\cxx-stl\stlport\stlport
AR=$(NDK_PATH)/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-ar
CC=$(NDK_PATH)/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-gcc
CXX=$(NDK_PATH)/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-g++.exe
AS=$(NDK_PATH)/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-gcc
OBJCC=$(NDK_PATH)/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-gcc
LD=$(NDK_PATH)/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-gcc
DEPCC=$(NDK_PATH)/ndk-bundle/toolchains/$(ARM_LINUX_ANDROIDE_API)/prebuilt/$(COMPILERDIR)/arm-linux-androideabi-gcc