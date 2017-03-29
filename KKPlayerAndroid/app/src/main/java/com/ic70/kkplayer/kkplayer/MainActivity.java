package com.ic70.kkplayer.kkplayer;

import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.ic70.kkplayer.kkplayer.CBtnClick;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements IKKMessageHandler
{
    private  Handler m_Handler;
    //视频文件管理
    private CFileManage m_FileManage;
    boolean m_HandleIni=false;
    private List<CKKMoviePath> m_lstFile;
    private EditText txtAvUrl;
    CKKPlayerSurfaceRender SurfaceRender;
    public MainActivity()
    {

        Log.v("MainActivity","初始化" );
        //扫描文件系统
       // m_FileManage = new CFileManage();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
       // IniControl();
        //ListView Localmovie_list = (ListView) findViewById(R.id.listView);/**/
        IniControl2();
    }
    void IniControl2()
    {
        setContentView(R.layout.rtmptest);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
       /* Button btx=(Button)findViewById(R.id.AVButton1);
         btx.setOnClickListener(new Button.OnClickListener(){//创建监听
            public void onClick(View v) {
                Intent intent = new Intent();

                intent.putExtra("MoviePath",  "rtmp://121.42.14.63/live/livestream");*/
        /* 指定intent要启动的类 */

        /*        intent.setClass(v.getContext(),CPlayerActivity.class);//(context.this, Activity02.class);*/
        /* 启动一个新的Activity */
        /*       v.getContext().startActivity(intent);
            }

        });*/

        txtAvUrl=( EditText)findViewById(R.id.AVeditText);
        Button btx2=(Button)findViewById(R.id.AVbutton2);
        //SurfaceRender=(CKKPlayerSurfaceRender)findViewById(R.id.surfaceView);
        btx2.setOnClickListener(new Button.OnClickListener(){//创建监听
            public void onClick(View v) {
               /* SurfaceRender.CreatePlayer();
                SurfaceRender.OpenMedia(txtAvUrl.getText().toString());*/
                 Intent intent = new Intent();

             EditText txtAv=  txtAvUrl;
             String xx=   txtAv.getText().toString();
                intent.putExtra("MoviePath",  xx);
               //指定intent要启动的类

                intent.setClass(v.getContext(),CPlayerActivity.class);//(context.this, Activity02.class);
               //启动一个新的Activity
                v.getContext().startActivity(intent);
            }

        });

    }
    void IniControl()
    {
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        //toolbar.setLogo(R.drawable.ic_launcher);
        // Title
        toolbar.setTitle("KK影音");
        // Sub Title
        toolbar.setSubtitle("本地资源");
        setSupportActionBar(toolbar);

        if(m_lstFile==null&&m_Handler==null)
        {
            m_Handler = new COs_KKHander(this);
            m_FileManage.start(m_Handler);
            m_HandleIni=true;
        }
        //设置动画效果。
        ImageView infoOperatingIV = (ImageView)findViewById(R.id.RotateImageView);
        Animation operatingAnim = AnimationUtils.loadAnimation(this, R.anim.dirtiprotate);
        LinearInterpolator lin = new LinearInterpolator();
        operatingAnim.setInterpolator(lin);
        if (operatingAnim != null) {
            infoOperatingIV.startAnimation(operatingAnim);
            infoOperatingIV.bringToFront();
            infoOperatingIV.setVisibility(View.VISIBLE);
        }
    }
    public void onConfigurationChanged (Configuration newConfig){

        super.onConfigurationChanged(newConfig);
        IniControl2();
       /* IniControl();
        if(m_lstFile!=null)
        {
            LoadFileInfo(m_lstFile);
        }*/
    }
    public void HandleKKObj(Object obj)
    {
        Message msg=(Message)obj;
        switch (msg.what) {
            case  COs_KKHander.LIST_MOVIE_INFO:
                m_lstFile =( List<CKKMoviePath> )msg.obj;
                LoadFileInfo(m_lstFile);
                break;
        }
        //this.finish();
    }
    void LoadFileInfo(List<CKKMoviePath> Partlist)
    {
        ListView Localmovie_list = (ListView) findViewById(R.id.listView);
        Localmovie_list.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE);
        if(Partlist.size()<=0) {
            CKKMoviePath KKpath = new CKKMoviePath();
            KKpath.MovieName="测试";
            Partlist.add(KKpath);
        }
        CKKMListAdapter adapter1=new CKKMListAdapter(this,Partlist);
        Localmovie_list.setAdapter( adapter1);

        ImageView infoOperatingIV = (ImageView)findViewById(R.id.RotateImageView);
        if(infoOperatingIV!=null)
        {
            infoOperatingIV.setVisibility(View.GONE);
            infoOperatingIV.setAnimation(null);
        }
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
   /* @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
       // finish();
        return false;
    }*/
}
