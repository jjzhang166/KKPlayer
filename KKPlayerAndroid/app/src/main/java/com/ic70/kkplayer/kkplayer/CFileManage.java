package com.ic70.kkplayer.kkplayer;


import android.os.Environment;
import android.os.Message;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
//import java.util.logging.Handler;

/**
 * Created by saint on 2016/4/11.
 */

public class CFileManage extends java.lang.Thread
{
    private android.os.Handler m_MsgHander;
    private boolean m_Start=false;
    HashMap<String,String> m_PathMap=new HashMap<String,String>();
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
        Message message = new Message();

        message.what = COs_KKHander.LIST_MOVIE_INFO;
        message.obj=lstFile;
        m_MsgHander.sendMessage(message);
    }

    private List<CKKMoviePath> lstFile =new ArrayList<CKKMoviePath>(); //结果 List
    public void GetFiles(String Path, String Extension) //搜索目录，扩展名，是否进入子文件夹
    {
        if(m_PathMap.containsKey(Path))
        {
            return;
        }else
        {
            m_PathMap.put(Path,Path);
        }
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
                        {
                            CKKMoviePath KKPath = new CKKMoviePath();
                            KKPath.MovieName=f.getName();
                            KKPath.MoviePath=f.getPath();
                            lstFile.add(KKPath);
                        }
                    }
                } else if (f.isDirectory()&&f.getPath().indexOf("/.") == -1&& !f.isHidden()) //忽略点文件（隐藏文件/文件夹）
                {
                    //&&f.getName().toLowerCase()!="data" &&
                        GetFiles(f.getPath(), Extension);
                }

            }
        }
    }
}
