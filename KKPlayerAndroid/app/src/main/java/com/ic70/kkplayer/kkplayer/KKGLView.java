package com.ic70.kkplayer.kkplayer;
//import android.opengl.EGLConfig;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLU;
import android.support.v7.app.AppCompatActivity;

import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by saint on 2016/3/8.
 * android gl测试
 */
public class KKGLView implements Renderer
{
    private  AppCompatActivity AndroidHwnd;
    Square square = new Square();
    public KKGLView(AppCompatActivity whd)
    {

        AndroidHwnd=whd;
    }
	@Override
    public void onDrawFrame(GL10 gl)
    {
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
       gl.glLoadIdentity();
       		gl.glTranslatef(0, 0, -4);  square.draw(gl);
    }
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {

        gl.glViewport(0, 0, width, height);
        gl.glMatrixMode(GL10.GL_PROJECTION);// OpenGL docs.
        // Reset the projection matrix
        gl.glLoadIdentity();// OpenGL docs.
        // Calculate the aspect ratio of the window
        GLU.gluPerspective(gl, 45.0f, (float) width / (float) height,
                0.1f, 100.0f);
        // Select the modelview matrix
        gl.glMatrixMode(GL10.GL_MODELVIEW);// OpenGL docs.
        // Reset the modelview matrix
        gl.glLoadIdentity();

    }
    //当窗口被创建时需要调用 onSurfaceCreate ，我们可以在这里对 OpenGL 做一些初始化工作，例如：
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        gl.glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
        gl.glShadeModel(GL10.GL_SMOOTH);
        gl.glClearDepthf(1.0f);
        gl.glEnable(GL10.GL_DEPTH_TEST);// OpenGL docs.
        gl.glDepthFunc(GL10.GL_LEQUAL);// OpenGL docs.
        gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT,GL10.GL_NICEST);

    }
}
