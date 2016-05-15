include Android_config.mak
objects=platforms.o KKLock.o KKMutex.o KKCond_t.o KKInternal.o SqliteOp.o AVInfomanage.o KKPlayer.o sqlite3.o
#ln -fs $(BASELib)/libc.so libc.so.1;
#SHARE_LIB   :=KKPayerCore.so  
#-l$(STLLib)libstlport_static.a \
#-l$(STLLib)libstlport_static.a \
#cd f:/ProgramTool/OpenPro/KKPlayer/KKPlayerCore
# f:/android/android-sdk-windows/ndk-bundle/toolchains/aarch64-linux-android-4.9/prebuilt/windows/bin/aarch64-linux-android-readelf.exe -d libKKPlayerCore.so
LDFLAGS =  -shared -std=c++11 -fPIC
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
	$(LDFLAGS) \
	-L$(BASELib) \
	-l$(BASELib)/liblog.so \
	-l$(STLLib)/libstlport_static.a \
	-l$(FFMPEGLib)libavcodec-56.so \
	-l$(FFMPEGLib)libavdevice-56.so -l$(FFMPEGLib)libavfilter-5.so \
	-l$(FFMPEGLib)libavformat-56.so \
	-l$(FFMPEGLib)libavutil-54.so \
	-l$(FFMPEGLib)libswresample-1.so \
	-l$(FFMPEGLib)libswscale-3.so;
	$(AR) rcs $(ObjLib) $(objects) 

sqlite3.o: sqlite/sqlite3ext.h sqlite/sqlite3.h sqlite/sqlite3.c
	$(CC) -c $(CFLAGS) sqlite/sqlite3.c
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
SqliteOp.o: sqlite/sqlite3.h SqlOp/SqliteOp.h SqlOp/SqliteOp.cpp
	$(CXX) -c $(CFLAGS) SqlOp/SqliteOp.cpp
AVInfomanage.o:	SqlOp/SqliteOp.h SqlOp/AVInfomanage.h SqlOp/AVInfomanage.cpp
	$(CXX) -c $(CFLAGS) SqlOp/AVInfomanage.cpp
KKPlayer.o: KKPlayer.cpp KKPlayer.h IKKAudio.h render/render.h KKLock.h KKVideoInfo.h KKInternal.h SqlOp/AVInfomanage.h
	$(CXX) -c $(CFLAGS) KKPlayer.cpp

install:
	cp -f $(ObjTARGET) ../KKPlayerAndroid/app/libs/armeabi-v7a
clean:
	rm $(ObjTARGET) $(objects) $(ObjLib) crtbegin_dynamic.o crtend_android.o crtend_so.o crtbegin_so.o crtbegin_static.o