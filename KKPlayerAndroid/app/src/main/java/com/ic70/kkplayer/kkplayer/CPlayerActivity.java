package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
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
    private boolean m_bPlayer=false;
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
   public  enum  EnumPlayerStata
   {
       Stop,
       Play,
       PAUSE
   }
    EnumPlayerStata PlayerStata=EnumPlayerStata.Stop;
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
        ImageButton Btn=(ImageButton)findViewById(R.id.StartButton);
        Btn.setOnClickListener(new MediaClassBtnClick(this));
        AdJustControl();

        m_KKPlayer.OpenMedia(path);
        PlayerStata=EnumPlayerStata.Play;
    }
    public void onConfigurationChanged (Configuration newConfig){
        super.onConfigurationChanged(newConfig);
        AdJustControl();
    }
    void AdJustControl()
    {
        Configuration mConfiguration = this.getResources().getConfiguration(); //获取设置的配置信息
        int ori = mConfiguration.orientation ; //获取屏幕方向
        boolean Sp=false;
        if(ori == Configuration.ORIENTATION_LANDSCAPE){
            //横屏
            Log.v("Screen", "横屏");
            Sp=true;
        }
        WindowManager wm = (WindowManager) this.getSystemService(Context.WINDOW_SERVICE);
        int width = wm.getDefaultDisplay().getWidth()/2-220;
        int height = wm.getDefaultDisplay().getHeight()/2-220;
        LinearLayout MediaControlLLayOut=(LinearLayout)findViewById(R.id.MediaControlLLayOut);

        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(MediaControlLLayOut.getLayoutParams());
        if(Sp)
        {
            Log.v("Screen",height+";" +width);
            lp.setMargins(width, 0, 0, 0);
        }else
        {
            lp.setMargins(width, 0, 0, 0);
        }

        MediaControlLLayOut.setLayoutParams(lp);
    }
   class MediaClassBtnClick implements   View.OnClickListener
   {
       CPlayerActivity m_PlayerActivity;
       public MediaClassBtnClick(CPlayerActivity PlayerActivity)
       {
          m_PlayerActivity=PlayerActivity;
       }
       public void onClick(View var1)
       {
          ImageButton btn=(ImageButton)var1;
           if(PlayerStata==EnumPlayerStata.Play) {
               btn.setImageResource(R.drawable.play1);
               PlayerStata=EnumPlayerStata.PAUSE;
               m_KKPlayer.Pause();
           }else if(PlayerStata==EnumPlayerStata.PAUSE)
           {
               btn.setImageResource(R.drawable.pause1);
               PlayerStata=EnumPlayerStata.Play;
               m_KKPlayer.Pause();
           }
       }
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
            Log.v("MoviePath", "Del");
            finish();
            Log.v("MoviePath", "退出 over mmm");
        }
        return false;
    }
}
