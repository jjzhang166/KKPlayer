package com.ic70.kkplayer.kkplayer;

/**
 * Created by saint on 2016/2/26.
 */
public class CJniKKPlayer {
    static{
        System.loadLibrary("z");
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");/**/
        System.loadLibrary("KKPlayerCore");
        System.loadLibrary("JniKKPlayer");
    }
    //初始化一个KKplayer实例
    public native int IniKK();
    //检查Gl环境
    public native int IniGl(int obj);
    //尺寸调整
    public native int Resizeint(int obj,int w, int h);
    public native int GlRender(int obj);
    public native void DelKK(int obj);
    public native int KKOpenMedia(String str,int obj);

    //get about of movie info
    public native String GetMediaInfo(int obj);
    //Movie Pause
    public native void Pause(int obj);

    public native void Seek(int obj,int value);
}
//javah -classpath ..\..\..\build\intermediates\classes\debug com.ic70.kkplayer.kkplayer.CJniKKPlayer