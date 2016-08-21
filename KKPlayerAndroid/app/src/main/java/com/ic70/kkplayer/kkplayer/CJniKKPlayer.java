package com.ic70.kkplayer.kkplayer;

/**
 * Created by saint on 2016/2/26.
 */
public class CJniKKPlayer {
    public class CapImgInfo
    {
        public CapImgInfo()
        {

        }
        public byte[] ImgData=null;
        public String ImgPixfmt=null;
        public int imgHeight=0;
        public  int imgWidth=0;
        public int dataSize=0;
    }
    static{
        System.loadLibrary("z");
       // System.loadLibrary("dvbcsa");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");/**/
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

    public native int KKIsNeedReConnect(int obj);
    //get about of movie info
    public native String GetMediaInfo(int obj);
    public native  int KKCloseMedia(int obj);
    public native  int KKGetPlayerState(int obj);
    public native  int KKIsReady(int obj);
    public native  int KKGetRealtime(int obj);

    //得到延迟
    public native  int KKGetRealtimeDelay(int obj);
    //强制刷新缓存队列
    public native  void KKForceFlushQue(int obj);
    /***********UI调用***********/
    //Movie Pause
    public native void Pause(int obj);

    public native void Seek(int obj,int value);

    public native int GetCapImg(CapImgInfo obj);
}
//javah -classpath ..\..\..\build\intermediates\classes\debug com.ic70.kkplayer.kkplayer.CJniKKPlayer