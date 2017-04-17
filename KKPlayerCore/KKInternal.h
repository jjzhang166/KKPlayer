/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#include "Includeffmpeg.h"
#include "KKVideoInfo.h"
void packet_queue_init(SKK_PacketQueue  *q);
void frame_queue_destory(SKK_FrameQueue *f);
int packet_queue_put(SKK_PacketQueue *q, AVPacket *pkt,AVPacket *flush_pkt,short SegId,bool bflush=true) ;
void init_clock(SKK_Clock *c, int *queue_serial);
int stream_component_open(SKK_VideoState *is, int stream_index);
//初始化队列
int frame_queue_init(SKK_FrameQueue *f, SKK_PacketQueue *pktq, int max_size, int keep_last);
//流媒体返回 1 
int is_realtime(AVFormatContext *s);
/*****计算延迟时间******/
double compute_target_delay(double delay, SKK_VideoState *is);
void update_video_pts(SKK_VideoState *is, double pts, int64_t pos, int serial);
SKK_Frame *frame_queue_peek_next(SKK_FrameQueue *f);
double get_clock(SKK_Clock *c);
int get_master_sync_type(SKK_VideoState *is);
double get_master_clock(SKK_VideoState *is);
/* seek in the stream */
void stream_seek(SKK_VideoState *is, int64_t pos, int64_t rel, int seek_by_bytes);
/**********清空队列***********/
void packet_queue_flush(SKK_PacketQueue *q);
void set_clock(SKK_Clock *c, double pts, int serial);

int get_master_sync_type(SKK_VideoState *is);
int frame_queue_nb_remaining(SKK_FrameQueue *f);
SKK_Frame *frame_queue_peek_last(SKK_FrameQueue *f);
SKK_Frame *frame_queue_peek(SKK_FrameQueue *f);
SKK_Frame *frame_queue_peek_readable(SKK_FrameQueue *f);
void frame_queue_unref_item(SKK_Frame *vp);
void frame_queue_next(SKK_FrameQueue *f,bool NeedLock);
double vp_duration(SKK_VideoState *is, SKK_Frame *vp, SKK_Frame *nextvp);
/****音频回调函数*****/
void audio_callback(void *userdata, char *stream, int len);
void RaiseVolume(char* buf, int size, int uRepeat, double vol);
void Packet_Queue_All_Flush(SKK_VideoState *pVideoInfo);