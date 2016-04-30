//#define __cplusplus
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES/gl.h>
#include <android/log.h>
#include "AndKKAudio.h"

#include "../../../../../KKPlayerCore/IKKPlayUI.h"
         //"F:/ProgramTool/OpenPro/KKPlayer/KKPlayerAndroid/app/src/main/jni/"
#include "../../../../../KKPlayerCore/KKPlayer.h"
#ifndef AndKKPlayerUI_H_
#define AndKKPlayerUI_H_
class CAndKKPlayerUI :public  IKKPlayUI,CRender
{
   public:
            CAndKKPlayerUI();
            ~CAndKKPlayerUI();
           //OpenGlES 1.0 返回没有实际作用
            int IniGl();
            void renderFrame();
            int Resizeint(int w,int h);
            int CloseMedia();
   public:
             bool init(HWND hView);
             void destroy();
             void resize(unsigned int w, unsigned int h);
             void WinSize(unsigned int w, unsigned int h);
             void render(char* buf,int width,int height);
             //呈现背景图片
             void renderBk(unsigned char* buf,int len);
             void SetWaitPic(unsigned char* buf,int len);
             void SetBkImagePic(unsigned char* buf,int len);
             MEDIA_INFO GetMediaInfo();
             int OpenMedia(char *str);
    /***********UI调用***********/
   public:
            virtual unsigned char* GetWaitImage(int &length,int curtime);
            virtual unsigned char* GetBkImage(int &length);
            virtual void OpenMediaFailure(char* strURL);
   private:
        /*************浏览器状态***************/
        int m_playerState;
        KKPlayer m_player;
        CAndKKAudio m_Audio;
        GLuint m_pGLHandle;
        GLuint gvPositionHandle;
        int m_Screen_Width;
        int m_Screen_Height;
        unsigned int  m_nTextureID;
};
#endif