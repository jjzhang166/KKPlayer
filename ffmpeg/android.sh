export TMPDIR=D:/ffmpeg/ffmpeg-3.3
NDK=D:/ProgramFiles/Android/Androidsdk/ndk-bundle
SYSROOT=$NDK/platforms/android-9/arch-arm/ 
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64
function build_one { 
./AndroidConfigure \
--prefix=./ff3.3Jni \
--enable-shared \
--disable-static \
--disable-doc \
--enable-cross-compile \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--target-os=linux \
--arch=arm \
--disable-encoders \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic $ADDI_CFLAGS" \
--extra-ldflags="$ADDI_LDFLAGS" \
--disable-avdevice \
--disable-programs
} 
CPU=arm PREFIX=$(pwd)/android/$CPU 
ADDI_CFLAGS="-marm" 
build_one