package com.ic70.kkplayer.kkplayer;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by saint on 2016/3/9.
 */
public class CKKPlayerView implements GLSurfaceView.Renderer
{
    @Override
    public void onDrawFrame(GL10 gl)
    {

    }
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {

    }
    //当窗口被创建时需要调用 onSurfaceCreate ，我们可以在这里对 OpenGL 做一些初始化工作，例如：
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {

    }
}
