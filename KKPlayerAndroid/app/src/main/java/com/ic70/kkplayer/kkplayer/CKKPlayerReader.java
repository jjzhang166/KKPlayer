package com.ic70.kkplayer.kkplayer;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by saint on 2016/3/9.
 */

public class CKKPlayerReader implements GLSurfaceView.Renderer
{
    CMediaInfo info = new  CMediaInfo();
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
    public CKKPlayerReader()
    {
        m_JniKKPlayer = new CJniKKPlayer();
        m_nKKPlayer=m_JniKKPlayer.IniKK();
        int ll=0;
        ll++;
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
        String ll;
        ll=m_nKKPlayer+";";
        Log.v("MoviePath",str);
        if(m_nKKPlayer!=0) {
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
    @Override
    public void onDrawFrame(GL10 gl)
    {
        if(m_nKKPlayer!=0)
        {
            m_JniKKPlayer.GlRender(m_nKKPlayer);
        }
    }
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        if(m_nKKPlayer!=0)
            m_JniKKPlayer.Resizeint(m_nKKPlayer,width,height);
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
