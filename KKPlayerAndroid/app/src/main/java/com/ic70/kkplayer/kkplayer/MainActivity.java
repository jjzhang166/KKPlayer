package com.ic70.kkplayer.kkplayer;

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
import android.widget.Button;
import android.widget.ListView;

import com.ic70.kkplayer.kkplayer.CBtnClick;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements IKKMessageHandler
{

   private  Handler m_Handler;
    private  CFileManage m_FileManage;
    private GLSurfaceView glView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        m_Handler = new COs_KKHander(this);
        m_FileManage = new CFileManage();
        m_FileManage.start(m_Handler);

        ListView Localmovie_list = (ListView) findViewById(R.id.listView);

        Label
        Localmovie_list
       /* char a='A';
        int l=(int)a+32;
        a=(char)l;
        setTheme(android.R.style.Theme_Translucent_NoTitleBar);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
       // glView = new  GLES2_0_SurfaceView(this); // Allocate a GLSurfaceView

        glView = new  GLSurfaceView(this);
        //glView.setRenderer(new KKGLView(this)); // Use a custom renderer
        glView.setRenderer(new CKKPlayerReader()); // Use a custom renderer
        glView.getAlpha();
        this.setContentView(glView);*/


       Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

       /* FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });*/

       /*  Button btn=(Button)findViewById(R.id.BtnJni);
        btn.setOnClickListener(new CBtnClick(this));*/
    }
    public void HandleKKObj(Object obj)
    {

    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
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
