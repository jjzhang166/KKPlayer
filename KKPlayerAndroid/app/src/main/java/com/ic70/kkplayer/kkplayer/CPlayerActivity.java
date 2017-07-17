package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
/**
 * Created by saint on 2016/4/27.
 * Video UI
 */
public class CPlayerActivity extends Activity {
    //private GLSurfaceView glView;
    private  GLES2_0_SurfaceView glView;
    private CKKPlayerGlRender m_KKPlayer=null;
    private boolean m_bSeekPlayer=false;
    Activity m_PlayerActivity=null;
    boolean m_bNecState=true;
    int m_MinRealtimeDelay=3;
    int m_RaType=0;
    String CurTimeStr = new String();
    Timer Activitytimer = new Timer();

    private Lock m_Avtivitylock = new ReentrantLock();
    int WhereInTime=0;
    TimerTask task = new TimerTask() {

        @Override
        public void run() {
            Message message = new Message();
            message.what = 1;
            Activityhandler.sendMessage(message);
        }
    }; /* */
   public  enum  EnumPlayerStata
   {
       OpenFailure,
       Stop,
       Play,
       PAUSE
   }
    EnumPlayerStata PlayerStata=EnumPlayerStata.Stop;
    int m_CurTime=0;
    String MoviePathStr;
    int m_OpenCouner=0;
    int m_LastDisConnect=0;
    int m_nLastSerial=0;
    Handler Activityhandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            m_Avtivitylock.lock();
            WhereInTime=1;
            m_Avtivitylock.unlock();
            if (msg.what == 1&&PlayerStata!=EnumPlayerStata.Stop)
            {

                int llx = m_KKPlayer.GetReady();

                if(true)
                {
                    int ll=m_KKPlayer.GetRealtimeDelay() ;
                    TextView xxll=( TextView) findViewById(R.id.TxtDelay);
                    String lllxx="延迟:";
                    lllxx=lllxx+ll+"s";
                    xxll.setText(lllxx);
                }
             //  Log.v("xxxxx:", m_KKPlayer.GetPlayerState() + "==="+m_OpenCouner);
                /*********文件打开失败***********/
                if(   m_KKPlayer.GetPlayerState()== CKKPlayerGlRender.EKKPlayerErr.KKOpenUrlOkFailure &&m_OpenCouner==0&&  PlayerStata==EnumPlayerStata.Play) {

                        PlayerStata = EnumPlayerStata.OpenFailure;
                        Button NetButton = (Button) findViewById(R.id.NetButton);
                        NetButton.setText("文件打开失败");
                        NetButton.setVisibility(View.VISIBLE);
                        ImageView ImageV = (ImageView) findViewById(R.id.WaitRImageView);
                        ImageV.setVisibility(View.GONE);
                        ImageV.setAnimation(null);
                        PlayerStata=EnumPlayerStata.OpenFailure;
                }

                if(m_KKPlayer.GetPlayerState() ==CKKPlayerGlRender.EKKPlayerErr.KKAVOver)
                {
                    Button NetButton = (Button) findViewById(R.id.NetButton);
                    NetButton.setText("视频播放结束");
                    NetButton.setVisibility(View.VISIBLE);
                } else if (((llx == 0 && m_KKPlayer.GetPlayerState()== CKKPlayerGlRender.EKKPlayerErr.KKAVWait) || m_OpenCouner >= 2)|| (PlayerStata==EnumPlayerStata.Play&& m_KKPlayer.GetRealtimeDelay()>m_MinRealtimeDelay))
                {
                    ///需要缓冲
                    ImageView ImageV = (ImageView) findViewById(R.id.WaitRImageView);
                    if (ImageV.getVisibility() != View.VISIBLE)
                         WaitGif();
                } else if ((llx ==1 && (m_KKPlayer.GetPlayerState()==CKKPlayerGlRender.EKKPlayerErr.KKAVReady||m_KKPlayer.GetPlayerState()==CKKPlayerGlRender.EKKPlayerErr.KKOpenUrlOk))&& m_OpenCouner == 0) {
                    ImageView ImageV = (ImageView) findViewById(R.id.WaitRImageView);
                    ImageV.setVisibility(View.GONE);
                    ImageV.setAnimation(null);

                    Button NetButton = (Button) findViewById(R.id.NetButton);
                    NetButton.setVisibility(View.GONE);
                }else if (m_OpenCouner == 0&& m_KKPlayer.GetPlayerState() !=CKKPlayerGlRender.EKKPlayerErr.KKOpenUrlOk && m_KKPlayer.GetRealtimeDelay() < 5 && llx ==1)
                {
                    ImageView ImageV = (ImageView) findViewById(R.id.WaitRImageView);
                    ImageV.setVisibility(View.GONE);
                    ImageV.setAnimation(null);

                    Button NetButton = (Button) findViewById(R.id.NetButton);
                    NetButton.setVisibility(View.GONE);
                }

                    //检查是否需要重连,只有流媒体才需要
                if (m_OpenCouner == 0&& m_KKPlayer.GetRealtime()==1)
                {
                    m_bNecState = isNetworkAvailable(m_PlayerActivity);
                    if (!m_bNecState||m_KKPlayer.GetNeedReConnect()==1)
                          m_LastDisConnect = 1;
                    /***********网络断开了显示*********/
                   if (!m_bNecState) {
                               Button NetButton = (Button) findViewById(R.id.NetButton);
                               NetButton.setText("网络已断");
                               NetButton.setVisibility(View.VISIBLE);
                               TextView TipTxtView = (TextView) findViewById(R.id.TipTxtView);
                               TipTxtView.setVisibility(View.GONE);
                    } else {
                          //网络断开后重连，及流断了重连打开
                          if ( (PlayerStata == EnumPlayerStata.Play) && m_bNecState && m_OpenCouner == 0 && m_LastDisConnect == 1)
                          {
                                 System.out.println(m_OpenCouner + "===状态2===");
                                 Button NetButton = (Button) findViewById(R.id.NetButton);
                                 NetButton.setVisibility(View.GONE);
                                 m_OpenCouner = 1;
                                m_LastDisConnect = 0;
                             }
                   }
               }
                if(m_OpenCouner>=1) {
                    System.out.println(m_OpenCouner + "===状态===");
                    m_OpenCouner++;
                }

                if(m_OpenCouner>20)
                {
                   // m_KKPlayer.clo
                    m_KKPlayer.OpenMedia(MoviePathStr);
                    m_OpenCouner=0;
                    ImageView ImageV = (ImageView) findViewById(R.id.WaitRImageView);
                    ImageV.setVisibility(View.GONE);
                    ImageV.setAnimation(null);
                }


                 if( m_KKPlayer!=null&&!m_bSeekPlayer) {
                     CkkMediaInfo info = m_KKPlayer.GetCMediaInfo();

                     SeekBar MovieSeekBar = (SeekBar) findViewById(R.id.MovieSeekbar);
                     if (info.Serial1 == info.Serial&&info.Serial1!=-1) {
                             MovieSeekBar.setProgress(info.CurTime);
                             MovieSeekBar.setMax(info.TotalTime);
                             TextView CurTimetextView = (TextView) findViewById(R.id.CurTimetextView);
                             m_CurTime = info.CurTime;

                             int h = (info.CurTime / (60 * 60));
                             int m = (info.CurTime % (60 * 60)) / (60);
                             int s = ((info.CurTime % (60 * 60)) % (60));
                             if (h < 10)
                                 CurTimeStr = "0" + h + ":";
                             else
                                 CurTimeStr = h + ":";
                             if (m < 10)
                                 CurTimeStr += "0" + m + ":";
                             else
                                 CurTimeStr += m + ":";
                             if (s < 10)
                                 CurTimeStr += "0" + s + "";
                             else
                                 CurTimeStr += s + "";
                             CurTimeStr += "/";

                             h = info.TotalTime / (60 * 60);
                             m = (info.TotalTime % (60 * 60)) / 60;
                             s = ((info.TotalTime % (60 * 60)) % 60);
                             if (h < 10)
                                 CurTimeStr += "0" + h + ":";
                             else
                                 CurTimeStr += h + ":";
                             if (m < 10)
                                 CurTimeStr += "0" + m + ":";
                             else
                                 CurTimeStr += m + ":";
                             if (s < 10)
                                 CurTimeStr += "0" + s + "";
                             else
                                 CurTimeStr += s + "";

                             CurTimetextView.setText(CurTimeStr);
                     }
                 }
                m_Avtivitylock.lock();
                WhereInTime=0;
                m_Avtivitylock.unlock();
            }
            super.handleMessage(msg);
        };
    };


    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.movielayout);
        m_PlayerActivity=this;
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Activitytimer.schedule(task, 500, 500);
        //glView = new GLES2_0_SurfaceView(this);// GLSurfaceView(this);
        glView =  (GLES2_0_SurfaceView) findViewById(R.id.GlsurfaceView);
        glView.setRenderer(m_KKPlayer); // Use a custom renderer
        ///不保持长宽比
        m_KKPlayer.SetKeepRatio( 0);
        glView.getAlpha();
        FrameLayout MovieFrameLayout = (FrameLayout) findViewById(R.id.MovieFrameLayout);
        //MovieFrameLayout.addView(glView, 0);

        Button NetButton= ( Button) findViewById(R.id.NetButton);

        NetButton.setOnClickListener(new Button.OnClickListener(){//创建监听
            public void onClick(View v) {
                /*Button NetButton2=(Button)v;
                NetButton2.setVisibility(View.GONE);
                m_KKPlayer.OpenMedia(MoviePathStr);*/
            }

        });
        Bundle bundle = getIntent().getExtras();
        CharSequence MoviePath = bundle.getCharSequence("MoviePath");
        MoviePathStr= MoviePath.toString(); /**/

        CharSequence  CRaType= bundle.getCharSequence("RaType");
        m_RaType= Integer.parseInt(CRaType.toString());
        CharSequence kkMediacodec= bundle.getCharSequence("kkMediacodec");
        int nMediacodec=Integer.parseInt(kkMediacodec.toString());
       // String path="";
        ImageButton Btn=(ImageButton)findViewById(R.id.StartButton);
        Btn.setOnClickListener(new MediaClassBtnClick(this));
        AdJustControl();

        SeekBar SeekBtn=(SeekBar)findViewById(R.id.MovieSeekbar);
        SeekBtn.setOnSeekBarChangeListener(new MediaSeekBarChangeListener(this)); // onStopTrackingTouch
        m_KKPlayer.SetKeepRatio(m_RaType);

        m_KKPlayer.SetDecoderMethod(nMediacodec);
        ///这里有点问题
        m_KKPlayer.OpenMedia(MoviePathStr,true);
        m_KKPlayer.SetMinRealtimeDelay(m_MinRealtimeDelay);
        m_CurTime=0;
        PlayerStata = EnumPlayerStata.Play;
        WaitGif();
    }
    public void WaitGif()
    {

       ImageView infoOperatingIV = (ImageView)findViewById(R.id.WaitRImageView);
        Animation operatingAnim = AnimationUtils.loadAnimation(this, R.anim.dirtiprotate);
        LinearInterpolator lin = new LinearInterpolator();
        operatingAnim.setInterpolator(lin);
        if (operatingAnim != null) {
            infoOperatingIV.startAnimation(operatingAnim);
            infoOperatingIV.bringToFront();
            infoOperatingIV.setVisibility(View.VISIBLE);
        } /**/
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
                int ll=var1.getProgress();
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

        m_KKPlayer = new CKKPlayerGlRender();
    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            Log.v("KEYCODE_BACK", "KEYCODE_BACK");
            Activitytimer.cancel();
            PlayerStata=EnumPlayerStata.Stop;
            m_Avtivitylock.lock();
            while(WhereInTime==1)
            {
                ;
            }
            m_Avtivitylock.unlock();
            CKKPlayerGlRender KKPlayer= m_KKPlayer;
            m_KKPlayer=null;
            KKPlayer.KKDel();
            KKPlayer=null;
            Log.v("MoviePath", "Del");
            finish();
            Log.v("MoviePath", "退出 over mmm");
        }
        return false;
    }

    public boolean isNetworkAvailable(Activity activity)
    {
        Context context = activity.getApplicationContext();
        // 获取手机所有连接管理对象（包括对wi-fi,net等连接的管理）
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);

        if (connectivityManager == null)
        {
            return false;
        }
        else
        {
            // 获取NetworkInfo对象
            NetworkInfo[] networkInfo = connectivityManager.getAllNetworkInfo();

            if (networkInfo != null && networkInfo.length > 0)
            {
                for (int i = 0; i < networkInfo.length; i++)
                {
                  //  System.out.println(i + "===状态===" + networkInfo[i].getState());
                    //System.out.println(i + "===类型===" + networkInfo[i].getTypeName());
                    // 判断当前网络状态是否为连接状态
                    if (networkInfo[i].getState() == NetworkInfo.State.CONNECTED)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
