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
    private boolean m_bSeekPlayer=false;

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
    int m_CurTime=0;
    Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            if (msg.what == 1)
            {
                 if( m_KKPlayer!=null&&!m_bSeekPlayer)
                 {
                     CKKPlayerReader.CMediaInfo info = m_KKPlayer.GetCMediaInfo();
                     SeekBar MovieSeekBar = ( SeekBar) findViewById(R.id.MovieSeekbar);
                     if(info.CurTime>m_CurTime&&info.CurTime-m_CurTime<20) {
                         MovieSeekBar.setProgress(info.CurTime);
                     }
                     MovieSeekBar.setMax(info.TotalTime);
                     TextView CurTimetextView=(TextView) findViewById(R.id.CurTimetextView);
                     m_CurTime=info.CurTime;

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

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        timer.schedule(task, 500, 500);
        glView = new GLSurfaceView(this);

        glView.setRenderer(m_KKPlayer); // Use a custom renderer
        glView.getAlpha();
        FrameLayout MovieFrameLayout = (FrameLayout) findViewById(R.id.MovieFrameLayout);
        MovieFrameLayout.addView(glView, 0);

       Bundle bundle = getIntent().getExtras();
        CharSequence MoviePath = bundle.getCharSequence("MoviePath");
        String path = MoviePath.toString(); /**/
       // String path="";
        ImageButton Btn=(ImageButton)findViewById(R.id.StartButton);
        Btn.setOnClickListener(new MediaClassBtnClick(this));
        AdJustControl();

        SeekBar SeekBtn=(SeekBar)findViewById(R.id.MovieSeekbar);
        SeekBtn.setOnSeekBarChangeListener(new MediaSeekBarChangeListener(this)); // onStopTrackingTouch
        m_KKPlayer.OpenMedia(path);
        m_CurTime=0;
        PlayerStata=EnumPlayerStata.Play;
    }
    public void onConfigurationChanged (Configuration newConfig){
        super.onConfigurationChanged(newConfig);
        AdJustControl();
    }
    public void onStart()
    {
        super.onStart();
        if(PlayerStata==EnumPlayerStata.PAUSE)
        {
            FunPalyState();
        }
    }
    public void onStop()
    {
        super.onStop();
        if(PlayerStata==EnumPlayerStata.Play)
        {
            FunPalyState();
        }
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
    class MediaSeekBarChangeListener implements SeekBar.OnSeekBarChangeListener
    {
        CPlayerActivity m_PlayerActivity;
        public  MediaSeekBarChangeListener(CPlayerActivity PlayerActivity)
        {
            m_PlayerActivity=PlayerActivity;
        }
        public void onProgressChanged(SeekBar var1, int var2, boolean var3)
        {

        }
        public void onStartTrackingTouch(SeekBar var1)
        {
            m_bSeekPlayer=true;
        }
        public void onStopTrackingTouch(SeekBar var1)
        {
            if(PlayerStata!=EnumPlayerStata.Stop)
            {
                if(PlayerStata==EnumPlayerStata.PAUSE)
                {
                    FunPalyState();
                }
                int ll=var1.getProgress()-m_CurTime;
                m_CurTime+=ll;
                m_KKPlayer.Seek(ll);
                m_bSeekPlayer=false;
            }

        }
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
           FunPalyState();
       }
   }
    protected void FunPalyState()
    {
        ImageButton btn=(ImageButton)findViewById(R.id.StartButton);
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
    public CPlayerActivity()
    {
        m_KKPlayer = new CKKPlayerReader();
    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            PlayerStata=EnumPlayerStata.Stop;
            timer.cancel();
            CKKPlayerReader KKPlayer= m_KKPlayer;
            m_KKPlayer=null;
            KKPlayer.KKDel();
            KKPlayer=null;
            Log.v("MoviePath", "Del");
            finish();
            Log.v("MoviePath", "退出 over mmm");
        }
        return false;
    }
}
