package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by saint on 2016/3/9.
 */

public class CKKPlayerGlRender implements GLSurfaceView.Renderer
{

    public class CMediaInfo
    {
        public  int CurTime;
        public  int TotalTime;
        public CMediaInfo()
        {
            CurTime=0;
            TotalTime=0;
        }
    }
    private CJniKKPlayer m_JniKKPlayer;
    private int          m_nKKPlayer=0;
    private int          m_nGlHandle=0;
    private boolean      m_ReOpen=false;
    private CMediaInfo info = new  CMediaInfo();
    private String m_url;
    public CKKPlayerGlRender()
    {
        m_JniKKPlayer = new CJniKKPlayer();
        m_nKKPlayer   = m_JniKKPlayer.IniKK(0);
    }
    //暂停
    public void Pause()
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.Pause(m_nKKPlayer);
        }
    }
    //
    public void Seek(int time)
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.Seek(m_nKKPlayer, time);
        }
    }
    public CMediaInfo GetCMediaInfo()
    {
        if(m_nKKPlayer!=0) {
            String infostr= m_JniKKPlayer.GetMediaInfo(m_nKKPlayer);
            String[]  ll= infostr.split(";");
            info.CurTime=Integer.parseInt(ll[0]);
            info.TotalTime=Integer.parseInt(ll[1]);
        }
        return  info;
    }
   

    public int OpenMedia(String str)
    {
        m_url=str;
        String ll;
        ll=m_nKKPlayer+";";
        Log.v("MoviePath",str);
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.KKCloseMedia(m_nKKPlayer);
            return m_JniKKPlayer.KKOpenMedia(str,m_nKKPlayer);
        }
        return 2;
    }
    public void SetKeepRatio( int KeepRatio)
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.SetKeepRatio(m_nKKPlayer, KeepRatio);
        }
    }

    public void KKDel()
    {
        if(m_nKKPlayer!=0)
        {
            int ll=m_nKKPlayer;
            m_nKKPlayer=0;
            m_JniKKPlayer.DelKK(ll);

        }
    }

    /***
     *播放器状态
     * @return -1 播放器关闭状态 -2，媒体打开失败。如果是实时媒体-2是流终断 。 -3，媒体播放关闭,如果是流媒体这表示需要重连
     */
    public int GetPlayerState()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKGetPlayerState(m_nKKPlayer);
        }
        return -1;
    }
    public int GetReady()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKIsReady(m_nKKPlayer);
        }
        return 0;
    }
    public int GetRealtime() {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKGetRealtime(m_nKKPlayer);
        }
        return 0;
    }
    public int GetRealtimeDelay()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKGetRealtimeDelay(m_nKKPlayer);
        }
        return 0;
    }
    public int SetMinRealtimeDelay(int value)
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKSetMinRealtimeDelay(m_nKKPlayer, value);
        }
        return 0;
    }
    public int GetNeedReConnect()
    {
        if(m_nKKPlayer!=0) {
            return  m_JniKKPlayer.KKIsNeedReConnect(m_nKKPlayer);
        }
        return 0;
    }
    @Override
    public void onDrawFrame(GL10 gl)
    {
       // Log.v("m_nKKPlayer", "="+m_nKKPlayer);
        if(m_nKKPlayer!=0){
            m_JniKKPlayer.GlRender(m_nKKPlayer);
        }
    }
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        if(m_nKKPlayer!=0)
            m_JniKKPlayer.OnSize(m_nKKPlayer,width,height);
    }
    //当窗口被创建时需要调用 onSurfaceCreate ，我们可以在这里对 OpenGL 做一些初始化工作，例如：
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        if(m_nKKPlayer!=0) {
            String glv = gl.glGetString(GL10.GL_VERSION);
            ///
            Log.v("Gl", "Gl Init");
            m_nGlHandle = m_JniKKPlayer.IniGl(m_nKKPlayer);

        }
    }
}
