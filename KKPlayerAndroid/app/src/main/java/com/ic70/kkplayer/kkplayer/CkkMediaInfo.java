package com.ic70.kkplayer.kkplayer;

/**
 * Created by saint on 2017/7/7.
 */

public class CkkMediaInfo {


        public CkkMediaInfo()
        {

        }

        //分辨率
        public String              AVRes =new String();
        public String              AVinfo=new String();
        public String              AvFile=new String();
        public int                FileSize=0;
        public int                 CurTime=0;
        public int               TotalTime=0;//总时长
        public int              Serial=0;
        public int              Serial1=0;
        public int               Open=0;
        public int               KKState=0;
        public int            SegId=0;
        public String               SpeedInfo=new String();    ///下载速度
        public int CacheVideoSize=0;
        public int CacheAudioSize=0;
        public int CacheMaxTime=0;
}
