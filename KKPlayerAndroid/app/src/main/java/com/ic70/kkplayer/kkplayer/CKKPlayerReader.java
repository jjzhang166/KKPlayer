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

public class CKKPlayerReader implements GLSurfaceView.Renderer
{
    CMediaInfo info = new  CMediaInfo();
    String m_url;
    public class CMediaInfo
    {
        public int CurTime;
        public  int TotalTime;
        public CMediaInfo()
        {
            CurTime=0;
            TotalTime=0;
        }
    }
    private CJniKKPlayer m_JniKKPlayer;
    private int m_nKKPlayer=0;
    private int m_nGlHandle=0;
    private Activity m_PlayerAc=null;
    public CKKPlayerReader(Activity ac)
    {
        m_PlayerAc=ac;
        m_JniKKPlayer = new CJniKKPlayer();
        m_nKKPlayer=m_JniKKPlayer.IniKK(0);
        int ll=0;
        ll++;
    }
    //暂停
    public void Pause()
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.Pause(m_nKKPlayer);
        }
    }
    //
    public void Seek(int l)
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.Seek(m_nKKPlayer, l);
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
    boolean m_ReOpen=false;

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
    public void KKDel()
    {
        if(m_nKKPlayer!=0)
        {
            int ll=m_nKKPlayer;
            m_nKKPlayer=0;
            m_JniKKPlayer.DelKK(ll);

        }
    }
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
    public int GetRealtime()
    {
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
        if(m_nKKPlayer!=0)
        {
            /*if(m_JniKKPlayer.KKIsNeedReConnect(m_nKKPlayer)==1&& !m_ReOpen&&m_JniKKPlayer.KKIsReady(m_nKKPlayer)==1)
            {
                OpenMedia(m_url);
            }else
            if(m_ReOpen)
            {
                m_ReOpen=false;
                OpenMedia(m_url);
            }*/
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
            m_nGlHandle = m_JniKKPlayer.IniGl(m_nKKPlayer);
        }
    }
}
