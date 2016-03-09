package com.ic70.kkplayer.kkplayer;
import  com.ic70.kkplayer.kkplayer.CJniKKPlayer;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

/**
 * Created by saint on 2016/2/29.
 */
public class CBtnClick implements View.OnClickListener {
    public CBtnClick(AppCompatActivity AppMain)
    {
        m_AppMain=AppMain;
    }
    @Override
    public void onClick(View view){
        switch(view.getId()) {
            case R.id.BtnJni:
                                          BtnJNiVoid();break;
        }
    }
    private void BtnJNiVoid()
    {
        CJniKKPlayer pp = new CJniKKPlayer();
        int l=0;
        l++;
        int ret=pp.IniKK();

        TextView txt=(TextView)m_AppMain.findViewById(R.id.textView);
        txt.setText("xxxxx"+Integer.toString(ret));
    }
    private AppCompatActivity m_AppMain;
}
