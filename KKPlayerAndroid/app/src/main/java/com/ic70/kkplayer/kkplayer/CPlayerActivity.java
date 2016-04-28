package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.widget.FrameLayout;

/**
 * Created by saint on 2016/4/27.
 * Video UI
 */
public class CPlayerActivity extends Activity {
    private GLSurfaceView glView;
    private CKKPlayerReader m_KKPlayer;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.movielayout);


        glView = new GLSurfaceView(this);
        m_KKPlayer = new CKKPlayerReader();
        glView.setRenderer(m_KKPlayer); // Use a custom renderer
        glView.getAlpha();
        FrameLayout MovieFrameLayout = (FrameLayout) findViewById(R.id.MovieFrameLayout);
        MovieFrameLayout.addView(glView, 0);

        Bundle bundle = getIntent().getExtras();
        CharSequence MoviePath = bundle.getCharSequence("MoviePath");
        String path = MoviePath.toString();
        m_KKPlayer.OpenMedia(path);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            m_KKPlayer.KKDel();;
            finish();
        }
        return false;
    }
}
