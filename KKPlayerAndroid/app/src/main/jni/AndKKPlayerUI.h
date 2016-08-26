//#define __cplusplus
#include <GLES2/gl2.h>
//#include <GLES/glext.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#include "AndKKAudio.h"

#include "../../../../../KKPlayerCore/IKKPlayUI.h"
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
             void render(char* buf,int width,int height,int imgwith);
             //呈现背景图片
             void renderBk(unsigned char* buf,int len);
             void AVRender();
             void SetWaitPic(unsigned char* buf,int len);
             void SetBkImagePic(unsigned char* buf,int len);
             unsigned char* GetCenterLogoImage(int &length);
             MEDIA_INFO GetMediaInfo();
             int OpenMedia(char *str);
             int GetIsReady();
             /******是否是流媒体,该方法不是很准确*****/
             int GetRealtime();
             //得到延迟
             int GetRealtimeDelay();
             //
             int SetMinRealtimeDelay(int value);
             //强制刷新Que
             void ForceFlushQue();
    /***********UI调用***********/
   public:
            virtual unsigned char* GetWaitImage(int &length,int curtime);
            virtual unsigned char* GetBkImage(int &length);
            virtual void OpenMediaFailure(char* strURL);
            virtual void  AutoMediaCose(int Stata);
            void LoadCenterLogo(unsigned char* buf,int len);
            bool GetNeedReconnect();
            void Pause();
            void Seek(int value);
            int GetPlayerState();
   private:
        GLuint buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
        void GLES2_Renderer_reset();
        void AVTexCoords_reset();
        void AVTexCoords_cropRight(GLfloat cropRight);
        GLuint g_texYId;
        GLuint g_texUId;
        GLuint g_texVId;
        GLuint g_glProgram;
        GLuint g_av2_texcoord;
        GLuint g_av4_position;

        GLuint m_vertexShader;
        GLuint m_fragmentShader;
        GLuint m_plane_textures[3];
        GLint m_us2_sampler[3];

        GLfloat m_AVVertices[8];
        GLfloat m_AVTexcoords[8];
        int m_glwidth;
        int m_glheight;
        int m_Picwidth;
        int m_Picheight;
        bool m_bAdJust;
        /*************浏览器状态***************/
        bool m_bNeedReconnect;
        int m_playerState;
        KKPlayer m_player;
        CAndKKAudio m_Audio;
        GLuint m_pGLHandle;
        GLuint gvPositionHandle;
        int m_Screen_Width;
        int m_Screen_Height;
        unsigned int  m_nTextureID;

       CKKLock m_RenderLock;
};
#endif