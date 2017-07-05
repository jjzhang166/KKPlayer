package com.ic70.kkplayer.kkplayer;
import android.view.Surface;
import android.view.SurfaceView;

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
       // System.loadLibrary("android");
       // System.loadLibrary("dvbcsa");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("avfilter-6");
       // System.loadLibrary("avdevice-57");/**/
        System.loadLibrary("KKPlayerCore");
    }
    /**
     * 初始化一个KKplayer实例 目前只支持glview。surfaceview后续支持
     * @param RenderType 0 glview 1 surfaceview
     * @return 返回一个播放器对象
     */
    public native int IniKK(int RenderType);

    //检查Gl环境，初始化glview环境
    public native int IniGl(int obj);

    /***
     * 尺寸调整
     * @param obj 播放器实例
     * @param w
     * @param h
     * @return
     */
    public native int OnSize(int obj,int w, int h);

    /***
     * 是否保持视频比例
     * @param KeepRatio 0拉伸， 1保持比例，2 4:3 ，3 16:9
     */
    public native void SetKeepRatio(int obj,int KeepRatio);
    /**
     * glview 显示调用
     * @param obj 播放器实例
     * @return
     */
    public native int GlRender(int obj);

    /***
     * 删除播放器
     * @param obj 播放器实例
     */
    public native void DelKK(int obj);

    public native void SurfaceRender(int obj,Surface surface);
    public native int  KKOpenMedia(String str,int obj);

    public native int  KKIsNeedReConnect(int obj);
    //get about of movie info
    public native  String GetMediaInfo(int obj);
    public native  int  KKCloseMedia(int obj);

    //KKOpenUrlOk=0,          /***播发器打开成功**/
    //KKOpenUrlOkFailure=1,   /**播发器打开失败***/
    //KKAVNotStream=2,
    //KKAVReady=3,            ///缓冲已经准备就绪
    //KKAVWait=4,             ///需要缓冲
    //KKRealTimeOver=5,
    //KKEOF=6,                ///文件结束了。
    //KKAVOver=7              ///视频播放结束
    //KKSeekOk=8,              ///Seek成功
    //KKSeekErr=9,             ///seek失败
    public native  int  KKGetPlayerState (int obj);
    public native  int  KKIsReady(int obj);
    public native  int  KKGetRealtime(int obj);

    //得到延迟
    public native  int KKGetRealtimeDelay(int obj);
    //设置延迟
    public native  int KKSetMinRealtimeDelay(int obj,int value);
    //强制刷新缓存队列
    public native  void KKForceFlushQue(int obj);
    /***********UI调用***********/
    //Movie Pause
    public native void Pause(int obj);

    public native void Seek(int obj,int value);

    public native int GetCapImg(CapImgInfo obj);
}
//javah -classpath ..\..\..\build\intermediates\classes\debug com.ic70.kkplayer.kkplayer.CJniKKPlayer