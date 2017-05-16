include Android_config.mak
objects=platforms.o KKLock.o KKMutex.o KKCond_t.o KKInternal.o kkio.o \
	KKPlayer.o md5.o srs_librtmp.o SrsRtmpPlugin.o FlvEncode.o \
	GlEs2Render.o AndKKAudio.o AndKKPlayerUI.o JniKKPlayer.o
#ln -fs $(BASELib)/libc.so libc.so.1;
#SHARE_LIB   :=KKPayerCore.so  
#-l$(STLLib)libstlport_static.a \
#-l$(STLLib)libstlport_static.a \
#cd f:/ProgramTool/OpenPro/KKPlayer/KKPlayerCore
# f:/android/android-sdk-windows/ndk-bundle/toolchains/aarch64-linux-android-4.9/prebuilt/windows/bin/aarch64-linux-android-readelf.exe -d libKKPlayerCore.so
LDFLAGS =  -shared -std=c++11 -fPIC
#-Wl,--gc-sections -O2
ObjTARGET = libKKPlayerCore.so
ObjLib = libKKPlayerCore.a
#-L $(BASELib) \
#-l$(STLLib)/libstlport_static.a \
#-static 静态编译 -shared 动态编译
KKPlayer_DLL=1
#$(AR) rcs $(ObjTARGET) $(objects) 
#-l$(BASELib)/libdl.so;
$(ObjTARGET):$(objects)
	cp -f $(BASELib)/crtbegin_dynamic.o ./crtbegin_dynamic.o;
	cp -f $(BASELib)/crtbegin_static.o ./crtbegin_static.o;
	cp -f $(BASELib)/crtend_android.o ./crtend_android.o;
	cp -f $(BASELib)/crtbegin_so.o ./crtbegin_so.o;
	cp -f $(BASELib)/crtend_so.o ./crtend_so.o;
	$(CXX) $(LDFLAGS) -o $(ObjTARGET) $(objects) \
	$(LDFLAGS)  \
	-L$(BASELib) \
	-L$(FFMPEGLib) \
	-L$(STLLib) \
	-llog \
	-lGLESv2 \
	-landroid \
	-lOpenSLES \
	-lstlport_static \
	-lavcodec-57 \
	-lavfilter-6 \
	-lavformat-57 \
	-lavutil-55 \
	-lswresample-2 \
	-lswscale-4;
	$(AR) rcs $(ObjLib) $(objects) 

md5.o: MD5/md5.c MD5/md5.h
	$(CC) -c  $(CFLAGS) MD5/md5.c

platforms.o:platforms.cpp stdafx.h
	$(CXX) -c $(CFLAGS) platforms.cpp
KKLock.o: KKLock.cpp KKLock.h platforms.h stdafx.h 
	$(CXX) -c $(CFLAGS) KKLock.cpp
KKMutex.o: KKMutex.cpp KKMutex.h platforms.h stdafx.h 
	$(CXX) -c $(CFLAGS) KKMutex.cpp
KKCond_t.o: KKCond_t.cpp KKCond_t.h	KKMutex.h platforms.h stdafx.h 
	$(CXX) -c $(CFLAGS) KKCond_t.cpp
KKInternal.o: KKInternal.cpp KKInternal.h  KKLock.h Includeffmpeg.h KKVideoInfo.h platforms.h stdafx.h 
	$(CXX) -c $(CFLAGS) KKInternal.cpp
kkio.o: KKVideoInfo.h kkptl/kkio.cpp
	$(CXX) -c $(CFLAGS)  kkptl/kkio.cpp

srs_librtmp.o: srs_librtmp.h srs_librtmp.cpp
	$(CXX) -c $(CFLAGS) srs_librtmp.cpp
SrsRtmpPlugin.o: srs_librtmp.h rtmp/SrsRtmpPlugin.h rtmp/SrsRtmpPlugin.cpp
	$(CXX) -c $(CFLAGS) rtmp/SrsRtmpPlugin.cpp
FlvEncode.o: rtmp/FlvEncode.h rtmp/FlvEncode.cpp
	$(CXX) -c $(CFLAGS) rtmp/FlvEncode.cpp
KKPlayer.o: KKPlayer.cpp KKPlayer.h IKKAudio.h render/render.h KKLock.h KKVideoInfo.h KKInternal.h rtmp/SrsRtmpPlugin.h
	$(CXX) -c $(CFLAGS) KKPlayer.cpp

AndKKAudio.o: Android/AndKKAudio.cpp Android/AndKKAudio.h KKCond_t.h KKLock.h IKKAudio.h
	$(CXX) -c $(CFLAGS) Android/AndKKAudio.cpp
GlEs2Render.o: Android/GlEs2Render.cpp Android/GlEs2Render.h KKCond_t.h KKLock.h IKKAudio.h
	$(CXX) -c $(CFLAGS) Android/GlEs2Render.cpp
AndKKPlayerUI.o: Android/AndKKPlayerUI.cpp Android/AndKKPlayerUI.h
	$(CXX) -c $(CFLAGS) Android/AndKKPlayerUI.cpp
JniKKPlayer.o: Android/JniKKPlayer.cpp Android/JniKKPlayer.h
	$(CXX) -c $(CFLAGS) Android/JniKKPlayer.cpp
install:
	cp -f $(ObjTARGET) ../KKPlayerAndroid/app/libs/armeabi-v7a
clean:
	rm $(ObjTARGET) $(objects) $(ObjLib) crtbegin_dynamic.o crtend_android.o crtend_so.o crtbegin_so.o crtbegin_static.o