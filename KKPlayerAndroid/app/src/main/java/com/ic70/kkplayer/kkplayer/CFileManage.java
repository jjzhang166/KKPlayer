package com.ic70.kkplayer.kkplayer;


import android.os.Environment;
import android.os.Message;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
//import java.util.logging.Handler;

/**
 * Created by saint on 2016/4/11.
 */

public class CFileManage extends java.lang.Thread
{
    private android.os.Handler m_MsgHander;
    private boolean m_Start=false;

    public synchronized void start(android.os.Handler handel) {
        m_Start=true;
        m_MsgHander=handel;
        super.start();
    }
    public void run()
    {
        String Path="/";
        String Extension="mp4";
        String state;
        state = Environment.getExternalStorageState();
        if(state.equals(Environment.MEDIA_MOUNTED))
        {
            Path = Environment.getExternalStorageDirectory().getAbsolutePath();
        }
        GetFiles(Path, Extension);

        List<CKKMoviePath> Partlist = new ArrayList<CKKMoviePath>();
        Message message = new Message();
        int i=0,j=0;
        while (i<lstFile.size()&&i<100)
        {
            CKKMoviePath KKpath = new CKKMoviePath();
            KKpath.MovieName=lstFile.get(0);
            Partlist.add( KKpath);
            lstFile.remove(0);
            i++;
        }
        message.what = COs_KKHander.LIST_STRING;
        message.obj=Partlist;
        m_MsgHander.sendMessage(message);
    }

    private List<String> lstFile =new ArrayList<String>(); //结果 List
    public void GetFiles(String Path, String Extension) //搜索目录，扩展名，是否进入子文件夹
    {
        File[] files =new File(Path).listFiles();
        if(files!=null) {
            for (int i = 0; i < files.length; i++) {
                File f = files[i];
                if (f.isFile())
                {
                    String name=f.getPath().substring(f.getPath().length() - Extension.length());
                    if(name!=null)
                    {
                       name=name.toLowerCase();
                        if ( name.equals(Extension)) //判断扩展名
                            lstFile.add(f.getPath());
                    }


                } else if (f.isDirectory()&&f.getName().toLowerCase()!="data" && f.getPath().indexOf("/.") == -1&& !f.isHidden()) //忽略点文件（隐藏文件/文件夹）
                    GetFiles(f.getPath(), Extension);
            }
        }
    }
}
