package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.SurfaceView;
import android.widget.FrameLayout;

/**
 * Created by saint on 2016/4/27.
 */
public class CPlayerActivity extends Activity
{
    private GLSurfaceView glView;
    private CKKPlayerReader m_KKPlayer;
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.movielayout);

        //SurfaceView MovieView=(SurfaceView)findViewById(R.id.surfaceView);
        glView = new GLSurfaceView(this);
        m_KKPlayer = new CKKPlayerReader();
        glView.setRenderer(m_KKPlayer); // Use a custom renderer
        glView.getAlpha();
   //    glView.setLayoutDirection(mx.core.LayoutDirection.LTR);

        // MovieView.setv
        FrameLayout MovieFrameLayout=(FrameLayout)findViewById(R.id.MovieFrameLayout);
        MovieFrameLayout.addView(glView,0);
    }
}
