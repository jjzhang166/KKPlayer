package com.ic70.kkplayer.kkplayer;
import android.view.Surface;
import android.view.SurfaceView;

/**
 * Created by saint on 2017/3/28.
 */

public class CkkPlayerBase
{
    public enum EnRenderType
    {
        GLSurfaceView,
        SurfaceView,
    }
    private  CJniKKPlayer m_JniKKPlayer;
    private int m_nKKPlayer=0;
    public CkkPlayerBase()
    {
        m_JniKKPlayer = new CJniKKPlayer();

    }
    public void finalize()
    {
             KKDel();
    }
    public boolean CreatePlayer(EnRenderType type)
    {
        if(m_nKKPlayer==0) {
            if(type==EnRenderType.GLSurfaceView)
                 m_nKKPlayer = m_JniKKPlayer.IniKK(0);
            else  if(type==EnRenderType.SurfaceView)
                m_nKKPlayer = m_JniKKPlayer.IniKK(1);
            if(m_nKKPlayer==0)
               return false;
            return true;
        }
        return false;
    }
    public int OpenMedia(String Url)
    {
        if(m_nKKPlayer!=0) {
           return m_JniKKPlayer.KKOpenMedia(Url,m_nKKPlayer);
        }
        return  -1;
    }
    //暂停
    public void Pause()
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.Pause(m_nKKPlayer);
        }
    }
    //
    public void Seek(int l)
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.Seek(m_nKKPlayer, l);
        }
    }

    /***
     * 删除播放器实例
     */
    public void KKDel()
    {
        if(m_nKKPlayer!=0)
        {
            int ll=m_nKKPlayer;
            m_nKKPlayer=0;
            m_JniKKPlayer.DelKK(ll);

        }
    }
    public int GetPlayerState()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKGetPlayerState(m_nKKPlayer);
        }
        return -1;
    }
    public int GetReady()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKIsReady(m_nKKPlayer);
        }
        return 0;
    }

    /***
     * 获取播放器时间
     * @return
     */
    public int GetRealtime()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKGetRealtime(m_nKKPlayer);
        }
        return 0;
    }

    /***
     * 获取延迟
     * @return
     */
    public int GetRealtimeDelay()
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKGetRealtimeDelay(m_nKKPlayer);
        }
        return 0;
    }

    /****
     * 设置最小延迟 只对srs:rtmp://....有用
     * @param value
     * @return
     */
    public int SetMinRealtimeDelay(int value)
    {
        if(m_nKKPlayer!=0) {
            return m_JniKKPlayer.KKSetMinRealtimeDelay(m_nKKPlayer, value);
        }
        return 0;
    }

    /***
     * rtmp 需要重连
     * @return
     */
    public int GetNeedReConnect()
    {
        if(m_nKKPlayer!=0) {
            return  m_JniKKPlayer.KKIsNeedReConnect(m_nKKPlayer);
        }
        return 0;
    }

    /***
     * 显示调用
     * @param surface
     */
    public void SurfaceRender(Surface surface)
    {
        if(m_nKKPlayer!=0) {
             m_JniKKPlayer.SurfaceRender(m_nKKPlayer,surface);
        }
    }
    public void OnSize(int w,int h)
    {
        if(m_nKKPlayer!=0) {
            m_JniKKPlayer.OnSize(m_nKKPlayer,w,h);
        }
    }
}
