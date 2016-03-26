#include "AndKKPlayerUI.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
static const char *gVertexShader=  "attribute vec4 vPosition;\n \
                void main() {\n \
                gl_Position = vPosition;\n \
                }\n";

static const char *gFragmentShader =
        "precision mediump float;\n \
        uniform sampler2D u_Texture;\n \
        varying lowp vec2 v_TexCoordinate;\n \
        void main() {\n \
        gl_FragColor =texture2D(u_Texture,v_TexCoordinate);\n \
        }\n";

static void printGLString(const char *name, GLenum s)
{
    const char *v = (const char *)::glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op)
{
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

/*GLuint loadShader(GLenum shaderType, const char* pSource) {
   //创建着色器
    //shaderType代表着色器的类型，可以是GL_VERTEX_SHADER（顶点着色器）或GL_FRAGMENT_SHADER（片元着色器）
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        //指定着色器源代码
        //source代表要执行的源代码字符串数组，1表示源代码字符串数组的字符串个数是一个，0表示源代码字符串长度数组的个数为0个
        glShaderSource(shader, 1, &pSource, NULL);
        //编译着色器
        glCompileShader(shader);
        GLint compiled = 0;
        //检查编译是否成功
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }else{
                   LOGE("Could not compile shader infoLen=0");
            }
        }
    }else{
        LOGE("Could not compile shader");
    }
    return shader;
}*/
CAndKKPlayerUI::CAndKKPlayerUI():m_player(this,&m_Audio)
{
    m_pGLHandle=0;
    gvPositionHandle=0;
    m_Screen_Width=0;
    m_Screen_Height=0;
    m_nTextureID=0;
}
CAndKKPlayerUI::~CAndKKPlayerUI()
{

}
int CAndKKPlayerUI::IniGl()
{
    m_player.InitSound();
    m_player.SetWindowHwnd(0);
    m_player.OpenMedia("rtmp://live.hkstv.hk.lxdns.com/live/hks live=1");
    //m_player.OpenMedia("/storage/emulated/0/Android/aaa.flv");
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    // 启用阴影平滑
    glShadeModel(GL_SMOOTH);

    // 黑色背景
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // 设置深度缓存
    glClearDepthf(1.0f);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 所作深度测试的类型
    glDepthFunc(GL_LEQUAL);

    // 对透视进行修正
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return m_pGLHandle;
}
// 定义π
const GLfloat PI = 3.1415f;
static void _gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat top = zNear * ((GLfloat) tan(fovy * PI / 360.0));
    GLfloat bottom = -top;
    GLfloat left = bottom * aspect;
    GLfloat right = top * aspect;
    glFrustumf(left, right, bottom, top, zNear, zFar);
}
int CAndKKPlayerUI::Resizeint(int w,int h)
{
    // 重置当前的视口
    glViewport(0, 0,w, h);
    // 选择投影矩阵
    glMatrixMode(GL_PROJECTION);
    // 重置投影矩阵
    glLoadIdentity();

    // 设置视口的大小
    _gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);

    // 选择模型观察矩阵
    glMatrixMode(GL_MODELVIEW);

    // 重置模型观察矩阵
    glLoadIdentity();
    m_player.AdjustDisplay(128,128);
}
// 顶点数组
const GLfloat gVertices[] = {
        0.0f, 1.0f, 0.0f,   // 上
        -1.0f,-1.0f, 0.0f,  // 左下
        1.0f,-1.0f, 0.0f,   // 右下
};

const GLfloat gVerticesSquare[] = {
        -1.0f, -1.0f, 0.0f, // 左下
        1.0f, -1.0f, 0.0f,  // 右下
        -1.0f, 1.0f, 0.0f,  // 左上
        1.0f, 1.0f, 0.0f    // 右上
};

// 纹理坐标
// 纹理坐标原点会因不同系统环境而有所不同。
// 比如在iOS以及Android上，纹理坐标原点（0, 0）是在左上角
// 而在OS X上，纹理坐标的原点是在左下角
const GLfloat gTextureCoord[] = {
        0.5f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
};

const GLfloat gTextureSquareCoord[] = {
        0.0f,1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
};
// 旋转角度
static GLfloat gAngle = 0.0f;
void CAndKKPlayerUI::renderFrame()
{
    // 清除屏幕及深度缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置背景颜色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);



    // 重置当前的模型观察矩阵
    glLoadIdentity();
    // 启用顶点数组
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // 纹理设置
    glEnable(GL_TEXTURE_2D);                                // 启用纹理映射
        // 选择纹理

    m_player.RenderImage(this);
    if(m_nTextureID==0)
        return;

    glBindTexture(GL_TEXTURE_2D, m_nTextureID);              // 选择纹
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);            // 启用纹理坐标数组


    // 绘制三角形
    glTranslatef(0.0f,2.0f,-10.0f);                         // 设置三角形位置
   // glRotatef(gAngle,0.0f,1.0f,0.0f);                       // 旋转三角形
    glVertexPointer(3, GL_FLOAT, 0, gVertices);             // 指定顶点数组
    glTexCoordPointer(2, GL_FLOAT, 0, gTextureCoord);       // 设置纹理坐标
    glDrawArrays(GL_TRIANGLES, 0, 3);  /**/                     // 绘制三角形

    // 绘制正方形
   /* glTranslatef(0.0f,-4.0f,0.0f);                          // 设置正方形位置
    glRotatef(-gAngle*2,0.0f,1.0f,0.0f);                    // 旋转正方形
    glVertexPointer(3, GL_FLOAT, 0, gVerticesSquare);       // 指定顶点数组
    glTexCoordPointer(2, GL_FLOAT, 0, gTextureSquareCoord); // 设置纹理坐标
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);                  // 绘制正方形
*/
    // 关闭顶点数组
    glDisableClientState(GL_VERTEX_ARRAY);
    // 关闭纹理数组
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    // 增加旋转角度
    gAngle += 2.0f;

    return;
}
void CAndKKPlayerUI::test()
{

}
unsigned char* CAndKKPlayerUI::GetWaitImage(int &length,int curtime)
{
    return NULL;
}
unsigned char* CAndKKPlayerUI::GetBkImage(int &length)
{
    return NULL;
}
void CAndKKPlayerUI::OpenMediaFailure(char *strURL)
{
    LOGE("Open Err");
    return;
}

bool CAndKKPlayerUI::init(HWND hView){}
void CAndKKPlayerUI::destroy(){}
void CAndKKPlayerUI::resize(unsigned int w, unsigned int h){}
void CAndKKPlayerUI::WinSize(unsigned int w, unsigned int h){}

float quadVertex[] = {
        -0.5f, 0.5f, 0.0f, // Position 0
        0, 1.0f, // TexCoord 0
        -0.5f, -0.5f, 0.0f, // Position 1
        0, 0, // TexCoord 1
        0.5f , -0.5f, 0.0f, // Position 2
        1.0f, 0, // TexCoord 2
        0.5f, 0.5f, 0.0f, // Position 3
        1.0f, 1.0f, // TexCoord 3
};

void CAndKKPlayerUI::render(char* buf,int width,int height){
    if(buf!=NULL)
    {
        if(m_nTextureID==0)
        {
           /* glDeleteTextures(2,&m_nTextureID);
            m_nTextureID=0;*/
            glGenTextures(2, &m_nTextureID);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_nTextureID );
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,buf);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

}
//呈现背景图片
void CAndKKPlayerUI::renderBk(unsigned char* buf,int len){}
void CAndKKPlayerUI::SetWaitPic(unsigned char* buf,int len){}
void CAndKKPlayerUI::SetBkImagePic(unsigned char* buf,int len){}