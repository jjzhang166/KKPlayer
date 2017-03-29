package com.ic70.kkplayer.kkplayer;
import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceHolder.Callback2;
/**
 * Created by saint on 2017/3/28.
 * SurfaceView 由于性能原因不支持
 */

public class CKKPlayerSurfaceRender extends SurfaceView  implements Callback2
{
    public CkkPlayerBase m_Player =null;
    CKKPlayerSurfaceRender OwnSurfaceView;
    public CKKPlayerSurfaceRender(Context context) {
        super(context);

    }

    public CKKPlayerSurfaceRender(Context context, AttributeSet attrs) {
        super(context, attrs);

    }

    public CKKPlayerSurfaceRender(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

    }

    public  void CreatePlayer()
    {
        if(m_Player ==null){
            m_Player = new CkkPlayerBase();
            m_Player.CreatePlayer(CkkPlayerBase.EnRenderType.SurfaceView);

        }
    }
    public  int OpenMedia(String Url)
    {
        if(m_Player !=null){
            int ret= m_Player.OpenMedia(Url);
            OwnSurfaceView=this;
            new Thread(new Runnable() {
                @Override
                public void run() {
                    while (true) {
                        Surface surface = OwnSurfaceView.getHolder().getSurface();
                        m_Player.SurfaceRender(surface);
                        try {
                          Thread.sleep(10);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }).start();
        }
        return -1;
    }
    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        if (holder.getSurface() == null) {
            return;
        }
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        if (holder.getSurface() == null) {
            return;
        }

        if(m_Player!=null){
            m_Player.OnSize(width,height);
        }
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder var1)
    {

    }
    @Override
    public void surfaceRedrawNeeded(SurfaceHolder var1)
    {
       if(m_Player!=null){

       }
    }/**/
}
