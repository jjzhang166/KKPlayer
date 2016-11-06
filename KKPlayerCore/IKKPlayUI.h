/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#ifndef KKPlayUI_H_
#define KKPlayUI_H_
/*******UI接口********/
class IKKPlayUI
{
   public:
	         virtual unsigned char* GetWaitImage(int &length,int curtime)=0;
			 virtual unsigned char* GetErrImage(int &length,int ErrType)=0;
		     virtual unsigned char* GetBkImage(int &length)=0;
			 virtual unsigned char* GetCenterLogoImage(int &length)=0;
			 //打开失败
			 virtual void OpenMediaFailure(char* strURL,int err)=0;
			 /*******视频流结束调用*******/
			 virtual void  AutoMediaCose(int Stata)=0;

			 virtual void AVRender()=0;
   protected:
	         unsigned char* m_pBkImage;
			 int m_pBkImageLen;
			 int m_pCenterLogoImageLen;

};
#endif
