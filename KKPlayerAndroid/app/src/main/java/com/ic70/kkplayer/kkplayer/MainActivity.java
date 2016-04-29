package com.ic70.kkplayer.kkplayer;

import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.ic70.kkplayer.kkplayer.CBtnClick;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements IKKMessageHandler
{
    private  Handler m_Handler;
    private CFileManage m_FileManage;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        //toolbar.setLogo(R.drawable.ic_launcher);
        // Title
        toolbar.setTitle("KK影音");
        // Sub Title
        toolbar.setSubtitle("本地资源");
        setSupportActionBar(toolbar);

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
        //扫描文件系统
        m_Handler = new COs_KKHander(this);
        m_FileManage = new CFileManage();
        m_FileManage.start(m_Handler);
        ListView Localmovie_list = (ListView) findViewById(R.id.listView);/**/
    }
    public void onConfigurationChanged (Configuration newConfig){

        super.onConfigurationChanged(newConfig);
        setContentView(R.layout.activity_main);
        //注意，这里删除了init()，否则又初始化了，状态就丢失
        //findViews();
        //setListensers();
    }
    public void HandleKKObj(Object obj)
    {
        Message msg=(Message)obj;
        switch (msg.what) {
            case  COs_KKHander.LIST_STRING:
                List<CKKMoviePath> Partlist =( List<CKKMoviePath> )msg.obj;
                LoadFileInfo(Partlist);
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
}
