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
        System.loadLibrary("JniKKPlayer");
    }
    public native int IniKK();
    public native int IniGl();
    public native int Resizeint(int gl,int w, int h);
    public native int GlRender();
}
//javah -classpath ..\..\..\build\intermediates\classes\debug com.ic70.kkplayer.kkplayer.CJniKKPlayer