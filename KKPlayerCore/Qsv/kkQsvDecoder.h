/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2017-2-25**********************************************/
//inter 硬件码准
#ifndef kkQsvDecoder_H_
#define kkQsvDecoder_H_

//是否支持QSV硬解码
bool KKTestQsvHardWare();
int BindQsvModule(AVCodecContext  *pCodecCtx);

#endif 