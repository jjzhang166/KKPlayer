package com.ic70.kkplayer.kkplayer;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by saint on 2016/3/9.
 */
public class CKKPlayerView implements GLSurfaceView.Renderer
{

    private CJniKKPlayer m_JniKKPlayer;
    private int m_nKKPlayer=0;
    private int m_nGlHandle=0;
    public CKKPlayerView()
    {
        m_JniKKPlayer = new CJniKKPlayer();
        m_nKKPlayer=m_JniKKPlayer.IniKK();
        int ll=0;
        ll++;
    }
    @Override
    public void onDrawFrame(GL10 gl)
    {
        m_JniKKPlayer.GlRender(m_nKKPlayer);
    }
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
            m_JniKKPlayer.Resizeint(m_nKKPlayer,width,height);
    }
    //当窗口被创建时需要调用 onSurfaceCreate ，我们可以在这里对 OpenGL 做一些初始化工作，例如：
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        m_nGlHandle= m_JniKKPlayer.IniGl(m_nKKPlayer);
    }
}
