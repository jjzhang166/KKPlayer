package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by saint on 2016/4/27.
 * Video UI
 */
public class CPlayerActivity extends Activity {
    private GLSurfaceView glView;
    private CKKPlayerReader m_KKPlayer=null;

    String CurTimeStr = new String();
    Timer timer = new Timer();
    TimerTask task = new TimerTask() {

        @Override
        public void run() {
            Message message = new Message();
            message.what = 1;
            handler.sendMessage(message);
        }
    }; /* */

    Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            if (msg.what == 1)
            {
                 if( m_KKPlayer!=null)
                 {
                     CKKPlayerReader.CMediaInfo info = m_KKPlayer.GetCMediaInfo();
                     SeekBar MovieSeekBar = ( SeekBar) findViewById(R.id.MovieSeekbar);
                     MovieSeekBar.setProgress(info.CurTime);
                     MovieSeekBar.setMax(info.TotalTime);
                     TextView CurTimetextView=(TextView) findViewById(R.id.CurTimetextView);


                     int h=(info.CurTime/(60*60));
                     int m=(info.CurTime%(60*60))/(60);
                     int s=((info.CurTime%(60*60))%(60));
                     if(h<10)
                        CurTimeStr="0"+h+":";
                     else
                         CurTimeStr=h+":";
                     if(m<10)
                         CurTimeStr+="0"+m+":";
                     else
                         CurTimeStr+=m+":";
                     if(s<10)
                         CurTimeStr+="0"+s+"";
                     else
                         CurTimeStr+=s+"";
                     CurTimeStr+="/";

                     h=info.TotalTime/(60*60);
                     m=(info.TotalTime%(60*60))/60;
                     s=((info.TotalTime%(60*60))%60);
                     if(h<10)
                         CurTimeStr+="0"+h+":";
                     else
                         CurTimeStr+=h+":";
                     if(m<10)
                         CurTimeStr+="0"+m+":";
                     else
                         CurTimeStr+=m+":";
                     if(s<10)
                         CurTimeStr+="0"+s+"";
                     else
                         CurTimeStr+=s+"";

                     CurTimetextView.setText(CurTimeStr);
                 }
            }
            super.handleMessage(msg);
        };
    };
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.movielayout);

        timer.schedule(task, 500, 500);
        glView = new GLSurfaceView(this);

        glView.setRenderer(m_KKPlayer); // Use a custom renderer
        glView.getAlpha();
        FrameLayout MovieFrameLayout = (FrameLayout) findViewById(R.id.MovieFrameLayout);
        MovieFrameLayout.addView(glView, 0);

        Bundle bundle = getIntent().getExtras();
        CharSequence MoviePath = bundle.getCharSequence("MoviePath");
        String path = MoviePath.toString();
        m_KKPlayer.OpenMedia(path);
    }
    public CPlayerActivity()
    {
         m_KKPlayer = new CKKPlayerReader();
    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            m_KKPlayer.KKDel();
            m_KKPlayer=null;
            Log.v("MoviePath", "退出 over mmm");
            finish();
        }
        return false;
    }
}
