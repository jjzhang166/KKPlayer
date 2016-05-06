/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date£º2015-6-25**********************************************/
#ifndef KKPlayUI_H_
#define KKPlayUI_H_
/*******UI½Ó¿Ú********/
class IKKPlayUI
{
   public:
	         virtual unsigned char* GetWaitImage(int &length,int curtime)=0;
		     virtual unsigned char* GetBkImage(int &length)=0;
			 virtual unsigned char* GetCenterLogoImage(int &length)=0;
			 virtual void OpenMediaFailure(char* strURL)=0;
   protected:
	         unsigned char* m_pBkImage;
			 int m_pBkImageLen;
			 int m_pCenterLogoImageLen;

};
#endif
