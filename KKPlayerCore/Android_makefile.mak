include Android_config.mak
objects=platforms.o KKinfo.o KKLock.o KKInternal.o KKPlayer.o
#ln -fs $(BASELib)/libc.so libc.so.1;
#SHARE_LIB   :=KKPayerCore.so  
ObjTARGET = libKKPayerCore.so
$(ObjTARGET): $(objects)
	cp -f $(BASELib)/crtbegin_dynamic.o ./crtbegin_dynamic.o;
	cp -f $(BASELib)/crtend_android.o ./crtend_android.o;
	cp -f $(BASELib)/crtbegin_so.o ./crtbegin_so.o
	cp -f $(BASELib)/crtend_so.o ./crtend_so.o
	$(CXX) -shared -fPIC -o $(ObjTARGET) $(objects) \
	-L $(BASELib) \
	-l$(STLLib)libstlport_static.a \
	-l$(FFMPEGLib)libavcodec-56.so \
	-l$(FFMPEGLib)libavdevice-56.so -l$(FFMPEGLib)libavfilter-5.so \
	-l$(FFMPEGLib)libavformat-56.so \
	-l$(FFMPEGLib)libavutil-54.so \
	-l$(FFMPEGLib)libswresample-1.so \
	-l$(FFMPEGLib)libswscale-3.so \
	
	
	
platforms.o:platforms.cpp stdafx.h
	$(CXX) -c platforms.cpp
KKinfo.o: KKinfo.cpp KKinfo.h platforms.h stdafx.h
	$(CXX) -c KKinfo.cpp $(CFLAGS)
KKLock.o: KKLock.cpp KKLock.h platforms.h stdafx.h 
	$(CXX) -c $(CFLAGS) KKLock.cpp 
KKInternal.o: KKInternal.cpp KKInternal.h  KKLock.h Includeffmpeg.h KKVideoInfo.h KKinfo.h platforms.h stdafx.h 
	$(CXX) -c $(CFLAGS) KKInternal.cpp  
KKPlayer.o: KKPlayer.cpp KKPlayer.h IKKAudio.h render/render.h KKLock.h KKVideoInfo.h KKInternal.h
	$(CXX) -c $(CFLAGS) KKPlayer.cpp  
clean:
	rm $(ObjTARGET) $(objects) crtbegin_dynamic.o crtend_android.o crtend_so.o crtbegin_so.o