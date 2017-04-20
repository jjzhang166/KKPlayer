function build_one { 
./configure \
--toolchain=msvc \
--extra-cflags='-IC:/msinttypes' \
--prefix='./vs2010' \
--enable-shared \
--disable-encoders \
--disable-avdevice \
--disable-doc \
--disable-programs
}
build_one