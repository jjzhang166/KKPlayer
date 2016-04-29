package com.ic70.kkplayer.kkplayer;

import android.os.Message;

import java.util.List;

/**
 * Created by saint on 2016/4/11.
 */
public class COs_KKHander extends android.os.Handler
{
    public static final  int  LIST_MOVIE_INFO=100;
    private IKKMessageHandler m_KKMessageHandler;
    public COs_KKHander(IKKMessageHandler obj)
    {

        m_KKMessageHandler=obj;
    }
    public void handleMessage(Message msg)
    {
        m_KKMessageHandler.HandleKKObj(msg);
        //super.handleMessage(msg);
    }
}
