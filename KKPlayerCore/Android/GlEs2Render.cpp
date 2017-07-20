#include "GlEs2Render.h"
#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
enum {
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
};
//Shader.frag文件内容
static const char g_FRAG_shader[] =
        "precision highp float;  \n"
                "varying   highp vec2 vv2_Texcoord;\n"
                "uniform         mat3 um3_ColorConversion;\n"
                "uniform   lowp  sampler2D us2_SamplerX;\n"
                "uniform   lowp  sampler2D us2_SamplerY;\n"
                "uniform   lowp  sampler2D us2_SamplerZ;\n"
                "void main()\n"
                "{\n"
                "   mediump vec3 yuv;\n"
                "    lowp    vec3 rgb;\n"
                "   yuv.x = (texture2D(us2_SamplerX, vv2_Texcoord).r - (16.0 / 255.0));\n"
                "   yuv.y = (texture2D(us2_SamplerY, vv2_Texcoord).r - 0.5);\n"
                "   yuv.z = (texture2D(us2_SamplerZ, vv2_Texcoord).r - 0.5);\n"
                "   rgb = um3_ColorConversion * yuv;\n"
                "   gl_FragColor = vec4(rgb, 1);\n"
                "}\n";


static const char G_VERTEX_shader[] =  
        "precision highp float; \n"
        "varying   highp vec2 vv2_Texcoord; \n"
        "attribute highp vec4 av4_Position; \n"
        "attribute highp vec2 av2_Texcoord; \n"
        "uniform         mat4 um4_ModelViewProjection; \n"
		
        "void main() \n"
        "{ \n"
        "    gl_Position  = um4_ModelViewProjection * av4_Position; \n"
        "    vv2_Texcoord = av2_Texcoord.xy; \n"
        "} \n";
		
static const char GSurfaceVertexShader[] =
        "attribute vec4 aPosition;\n"
                "attribute vec4 aTexCoordinate;\n"
                "uniform mat4 texTransform;\n"
                "varying vec2 v_TexCoordinate;\n"
                "void main() {\n"
                "v_TexCoordinate = (texTransform * aTexCoordinate).xy;\n"
                "gl_Position = aPosition;\n"
                "}\n";

static const char GSurfaceFragmentShader[] =
        "#extension GL_OES_EGL_image_external : require\n"
                "precision mediump float;\n"
                "uniform samplerExternalOES  texture;\n"
                "varying vec2 v_TexCoordinate;\n"
                "void main() {\n"
                "vec4 color = texture2D(texture,v_TexCoordinate);\n"
                "gl_FragColor = color;\n"
                "}\n";
typedef struct KK_GLES_Matrix
{
    GLfloat m[16];
} KK_GLES_Matrix;
static const GLfloat g_bt709[] = {
        1.164,  1.164,  1.164,
        0.0,   -0.213,  2.112,
        1.793, -0.533,  0.0,
};
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
static GLuint buildShader(const char* source, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);

    if (shaderHandle)
    {
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);

        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shaderHandle, infoLen, NULL, buf);
                    LOGE("error::Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }

    }

    return shaderHandle;
}

static void AVTexCoords_reloadVertex(GLuint av2_texcoord,const void *texcoords )
{
    glVertexAttribPointer(av2_texcoord, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
    glEnableVertexAttribArray(av2_texcoord);
}

static void AVVertices_reloadVertex(GLuint av4_position,const void *vertices)
{
    glVertexAttribPointer(av4_position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(av4_position);
}

//PlayVideo-OpenGL
GlEs2Render::GlEs2Render(KKPlayer* pPlayer):m_pGLHandle(0),gvPositionHandle(0),m_Screen_Width(0)
,m_Screen_Height(0),m_nTextureID(0),m_bAdJust(false)
,m_RenderWidth(0),m_RenderHeight(0),m_Picwidth(0)
,m_Picheight(0),m_pPlayer(pPlayer),m_nKeepRatio(1)
,g_texYId(0),g_texUId(0),g_texVId(0)
,g_glProgram(0),g_av2_texcoord(0),g_av4_position(0)
,m_vertexShader(0),m_fragmentShader(0)
,g_SurfaceTextVId(0)
,updateTexImageMethodId(0)
,getTimestampMethodId(0)
,getTransformMtxId(0)
,m_penv(0)
,javaSurfaceTextureObj(0)
,m_bfameAvailable(false)
,g_glSurfaceProgram(0)
,m_vertexShaderSurfaceTexture(0)
,m_fragmentShaderSurfaceTexture(0)
,m_textureParamHandle(0)
,m_texturepositionHandle(0)
,m_textureCoordHandle(0)
,m_textureTranformHandle(0)
,m_bAvPicLoaded(0)
{
	

    m_plane_textures[0]=0;
    m_plane_textures[1]=0;
    m_plane_textures[2]=0;

    m_us2_sampler[0]=0;
    m_us2_sampler[1]=0;
    m_us2_sampler[2]=0;

   
   

    m_AVVertices[0]= -1.0f;
    m_AVVertices[1]= -1.0f;
    m_AVVertices[2]=  1.0f;
    m_AVVertices[3]=  -1.0f;
    m_AVVertices[4]=  -1.0f;
    m_AVVertices[5]= 1.0f;
    m_AVVertices[6]=  1.0f;
    m_AVVertices[7]=  1.0f;


    m_AVTexcoords[0]=0.0f;
    m_AVTexcoords[1]=1.0f;
    m_AVTexcoords[2]=1.0f;
    m_AVTexcoords[3]=1.0f;
    m_AVTexcoords[4]=0.0f;
    m_AVTexcoords[5]=0.0f;
    m_AVTexcoords[6]=1.0f;
    m_AVTexcoords[7]=0.0f;
	
}
GlEs2Render::~GlEs2Render()
{
	GLES2_Renderer_reset();
	 if (m_penv&&javaSurfaceTextureObj) {
        m_penv->DeleteGlobalRef( javaSurfaceTextureObj );
        javaSurfaceTextureObj = 0;
    }
}
void  GlEs2Render::SetKeepRatio(int KeepRatio)
{
	m_nKeepRatio=KeepRatio;
}
void GlEs2Render::GLES2_Renderer_reset()
{
	///gl view
   
			if (m_vertexShader) {
				glDeleteShader(m_vertexShader);
				m_vertexShader=0;
			}
			if (m_fragmentShader){
				glDeleteShader(m_fragmentShader);
				m_fragmentShader=0;
			}

			if (g_glProgram){
				glDeleteProgram(g_glProgram);
				g_glProgram=0;
			}


			for (int i = 0; i < 3; ++i) {
				if (m_plane_textures[i]) {
					glDeleteTextures(1, &m_plane_textures[i]);
					m_plane_textures[i]=0;
				}
			}
			
			
			//GLuint g_glSurfaceProgram;
		if (m_vertexShaderSurfaceTexture){
			glDeleteShader(m_vertexShaderSurfaceTexture);
			m_vertexShaderSurfaceTexture=0;
		}
		if (m_fragmentShader){
				glDeleteShader(m_fragmentShaderSurfaceTexture);
				m_fragmentShader=0;
		}
		if (g_glSurfaceProgram){
			 glDeleteProgram(g_glSurfaceProgram);
			 g_glSurfaceProgram=0;
		}
		if(g_SurfaceTextVId!=0)
		    glDeleteTextures(1, &g_SurfaceTextVId);
		m_bAvPicLoaded=0;
   /*   glDeleteTextures(1, &m_textureParamHandle);
	  m_textureParamHandle=0;
		
		glDeleteTextures(1, &m_texturepositionHandle);
	  m_texturepositionHandle=0;
     
	 glDeleteTextures(1, &m_texturepositionHandle);
	  m_texturepositionHandle=0;
        GLuint m_textureCoordHandle;
        GLuint m_textureTranformHandle;
		
			if(g_SurfaceTextVId!=0)
				glDeleteTextures(1, &g_SurfaceTextVId);*/
}


GLuint GlEs2Render::buildProgram(const char* vertexShaderSource,
                           const char* fragmentShaderSource)
{
    m_vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    m_fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();

    if (programHandle)
    {
        glAttachShader(programHandle, m_vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(programHandle, m_fragmentShader);
        checkGlError("glAttachShader");
        glLinkProgram(programHandle);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
                    LOGE("error::Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }

    }

    return programHandle;
}
GLuint GlEs2Render::buildProgramSurfaceTexture(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	m_vertexShaderSurfaceTexture = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    m_fragmentShaderSurfaceTexture = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();

    if (programHandle)
    {
        glAttachShader(programHandle, m_vertexShaderSurfaceTexture);
        checkGlError("glAttachShader");
        glAttachShader(programHandle, m_fragmentShaderSurfaceTexture);
        checkGlError("glAttachShader");
        glLinkProgram(programHandle);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
                    LOGE("error::Could not link Surface program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }

    }

    return programHandle;
}
void KK_GLES2_loadOrtho(KK_GLES_Matrix *matrix, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
    GLfloat r_l = right - left;
    GLfloat t_b = top - bottom;
    GLfloat f_n = far - near;
    GLfloat tx = - (right + left) / (right - left);
    GLfloat ty = - (top + bottom) / (top - bottom);
    GLfloat tz = - (far + near) / (far - near);

    matrix->m[0] = 2.0f / r_l;
    matrix->m[1] = 0.0f;
    matrix->m[2] = 0.0f;
    matrix->m[3] = 0.0f;

    matrix->m[4] = 0.0f;
    matrix->m[5] = 2.0f / t_b;
    matrix->m[6] = 0.0f;
    matrix->m[7] = 0.0f;

    matrix->m[8] = 0.0f;
    matrix->m[9] = 0.0f;
    matrix->m[10] = -2.0f / f_n;
    matrix->m[11] = 0.0f;

    matrix->m[12] = tx;
    matrix->m[13] = ty;
    matrix->m[14] = tz;
    matrix->m[15] = 1.0f;
}

int GlEs2Render::IniGl()
{

    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);



    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    g_glProgram = buildProgram(G_VERTEX_shader, g_FRAG_shader);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glUseProgram(g_glProgram);

    if( m_plane_textures[0]==NULL)
    {
        glGenTextures(1, &g_texYId);
        glGenTextures(1, &g_texUId);
        glGenTextures(1, &g_texVId);
		
    }


    m_us2_sampler[0] = glGetUniformLocation(g_glProgram, "us2_SamplerX");
    m_us2_sampler[1] = glGetUniformLocation(g_glProgram, "us2_SamplerY");
    m_us2_sampler[2] = glGetUniformLocation(g_glProgram, "us2_SamplerZ");
    GLuint um3_color_conversion = glGetUniformLocation(g_glProgram, "um3_ColorConversion");

    m_plane_textures[0]=g_texYId;
    m_plane_textures[1]=g_texUId;
    m_plane_textures[2]=g_texVId;

    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_plane_textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glUniform1i(m_us2_sampler[i], i);
    }

    


    glUniformMatrix3fv(um3_color_conversion, 1, GL_FALSE, g_bt709);

    g_av4_position = glGetAttribLocation(g_glProgram, "av4_Position");
    g_av2_texcoord = glGetAttribLocation(g_glProgram, "av2_Texcoord");
    GLuint  um4_mvp      = glGetUniformLocation(g_glProgram, "um4_ModelViewProjection");


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);


    KK_GLES_Matrix modelViewProj;
    KK_GLES2_loadOrtho(&modelViewProj, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(um4_mvp, 1, GL_FALSE, modelViewProj.m);
	
   //	glUseProgram(0);
    ///surfacetexture 着色器
	/*g_glSurfaceProgram=buildProgramSurfaceTexture(GSurfaceVertexShader, GSurfaceFragmentShader);
	
	m_textureParamHandle = glGetUniformLocation(g_glSurfaceProgram, "texture");
    m_texturepositionHandle = glGetAttribLocation(g_glSurfaceProgram, "aPosition");
    m_textureCoordHandle = glGetAttribLocation(g_glSurfaceProgram, "aTexCoordinate");
    m_textureTranformHandle = glGetUniformLocation(g_glSurfaceProgram, "texTransform");*/
   
    return m_pGLHandle;
}


void GlEs2Render::setFrameAvailable(bool const available) {
    m_bfameAvailable = available;
}

jobject GlEs2Render::SetSurfaceTexture(JNIEnv *env)
{
	m_penv=env;
    glGenTextures(1,&g_SurfaceTextVId);
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, g_SurfaceTextVId);
	glTexParameterf(GL_TEXTURE_EXTERNAL_OES,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		
    const char *stClassPath = "android/graphics/SurfaceTexture";
    const jclass surfaceTextureClass = env->FindClass(stClassPath);
    if (surfaceTextureClass == 0) {
       LOGE("FindClass (%s) failed", stClassPath);
    }

//    // find the constructor that takes an int
    const jmethodID constructor = env->GetMethodID( surfaceTextureClass, "<init>", "(I)V" );
    if (constructor == 0) {
      LOGE("GetMethonID(<init>) failed");
    }

    jobject  obj = env->NewObject(surfaceTextureClass, constructor, g_SurfaceTextVId);
    if (obj == 0) {
       LOGE("NewObject() failed");
    }

    javaSurfaceTextureObj = env->NewGlobalRef(obj);
    if (javaSurfaceTextureObj == 0) {
       LOGE("NewGlobalRef() failed");
    }

    //Now that we have a globalRef, we can free the localRef
   env->DeleteLocalRef(obj);

    updateTexImageMethodId = env->GetMethodID( surfaceTextureClass, "updateTexImage", "()V");
    if ( !updateTexImageMethodId ) {
       LOGE("couldn't get updateTexImageMethonId");
    }

    getTimestampMethodId =env->GetMethodID(surfaceTextureClass, "getTimestamp", "()J");
    if (!getTimestampMethodId) {
       LOGE("couldn't get TimestampMethodId");
    }

    getTransformMtxId = env->GetMethodID(surfaceTextureClass, "getTransformMatrix", "([F)V");
    if (!getTransformMtxId) {
        LOGE("couldn't get getTransformMtxId");
    }

    // jclass objects are loacalRefs that need to be free;
    env->DeleteLocalRef( surfaceTextureClass );  /**/
	
	return javaSurfaceTextureObj;
}
jobject  GlEs2Render::GetSurfaceTexture()
{
	
	return javaSurfaceTextureObj;
}
void GlEs2Render::AVTexCoords_reset()
{
    m_AVTexcoords[0] = 0.0f;
    m_AVTexcoords[1] = 1.0f;
    m_AVTexcoords[2] = 1.0f;
    m_AVTexcoords[3] = 1.0f;
    m_AVTexcoords[4] = 0.0f;
    m_AVTexcoords[5] = 0.0f;
    m_AVTexcoords[6] = 1.0f;
    m_AVTexcoords[7] = 0.0f;
}

void GlEs2Render::AVTexCoords_cropRight(GLfloat cropRight)
{
    m_AVTexcoords[0] = 0.0f;
    m_AVTexcoords[1] = 1.0f;
    m_AVTexcoords[2] = 1.0f - cropRight;
    m_AVTexcoords[3] = 1.0f;
    m_AVTexcoords[4] = 0.0f;
    m_AVTexcoords[5] = 0.0f;
    m_AVTexcoords[6] = 1.0f - cropRight;
    m_AVTexcoords[7] = 0.0f;
}



void GlEs2Render::GlViewRender(bool ReLoad)
{
	 //return;
    if(g_glProgram==0|| m_vertexShader==0||m_fragmentShader==0)
	{
		 LOGE("g_glProgram=%d m_vertexShader=%d||m_fragmentShader=%d \n", g_glProgram,m_vertexShader,m_fragmentShader);
         return;
	}
   /* if(m_bfameAvailable&&m_penv&&javaSurfaceTextureObj){
	  m_penv->CallVoidMethod(javaSurfaceTextureObj, updateTexImageMethodId);
	}*/
	glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram(g_glProgram);
    if(!m_bAdJust&&m_Picwidth!=0&& m_Picheight!=0||m_nKeepRatio!=m_nLastKeepRatio)
    {
        float width     =m_Picwidth;
        float height    = m_Picheight;
		m_nLastKeepRatio=m_nKeepRatio;
		if(m_nKeepRatio==2){
			///4:3
			float height1    =(float) (m_RenderWidth*3.0)/4.0;
			if(height1>m_RenderHeight)
			{
				width    =(float) (m_RenderHeight*4.0)/3.0;
				height    = m_RenderHeight;
			}else{
				width     = m_RenderWidth;
                height    = height1;
			}
		}else if(m_nKeepRatio==3){
			
			///16:9
			float height1    =(float) (m_RenderWidth*9.0)/16.0;
			if(height1>m_RenderHeight)
			{
				width    =(float) (m_RenderHeight*16.0)/9.0;
				height    = m_RenderHeight;
			}else{
				width     = m_RenderWidth;
                height    = height1;
			}
		}/**/
		
        float dW  = m_RenderWidth	/ width;
        float dH  = m_RenderHeight / height;
        float dd        = 1.0f;
        float nW        = 1.0f;
        float nH        = 1.0f;


		
		
        dd = FFMIN(dW, dH);
		
		
		if(m_nKeepRatio!=0){
           nW = (width  * dd / (float)m_RenderWidth);
           nH = (height * dd / (float)m_RenderHeight);
		}
		
		// LOGE("m_nKeepRatio=%d \n", m_nKeepRatio);
		else if(m_nKeepRatio==2){
           nW = (width  * dW / (float)m_RenderWidth);
           nH = (height * dH / (float)m_RenderHeight);
		}else if(m_nKeepRatio==3){
           nW = (width  * dW / (float)m_RenderWidth);
           nH = (height * dH / (float)m_RenderHeight);
		}/**/


        m_AVVertices[0] = - nW;
        m_AVVertices[1] = - nH;
        m_AVVertices[2] =   nW;
        m_AVVertices[3] = - nH;
        m_AVVertices[4] = - nW;
        m_AVVertices[5] =   nH;
        m_AVVertices[6] =   nW;
        m_AVVertices[7] =   nH;

        AVVertices_reloadVertex(g_av4_position, m_AVVertices);
        AVTexCoords_reset();
        AVTexCoords_cropRight(0);
        AVTexCoords_reloadVertex(g_av2_texcoord,  m_AVTexcoords);
        m_bAdJust=true;
		
    }

	if(ReLoad){
		
       m_pPlayer->RenderImage(this, false); 
	  
	} 
	if(m_bAvPicLoaded==0)
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	else
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGlError("glDrawArrays");
	//glUseProgram(0);
    if(m_Picwidth==0|| m_Picheight==0)
         return;
   
    return;
}


bool GlEs2Render::init(HWND hView)
{
	IniGl();
	return true;
}
void GlEs2Render::destroy()
{
	
}
void GlEs2Render::resize(unsigned int w, unsigned int h)
{
	m_RenderWidth=w;
    m_RenderHeight=h;
	
    // 重置当前的视口
	glViewport(0, 0,w, h);
    m_bAdJust= false;
}
void GlEs2Render::render(kkAVPicInfo *Picinfo,bool wait)
{
	
    if(Picinfo!=NULL&&Picinfo->width!=0&&Picinfo->height!=0)
    {
		m_bAvPicLoaded=1;
				if(m_Picheight!= Picinfo->height||m_Picwidth!=Picinfo->width)
				{
					m_Picwidth=Picinfo->width;
					m_Picheight=Picinfo->height;
					m_bAdJust=false;
					
				}
				LOGI("MEDIACODEC 22-- %d  %d \n",(int)AV_PIX_FMT_MEDIACODEC, Picinfo->picformat);
                if(Picinfo->picformat!=(int)AV_PIX_FMT_MEDIACODEC)
				{
					
					int     planes[3]    = { 0, 1, 2 };
					const GLsizei widths[3]    = { Picinfo->linesize[0], Picinfo->linesize[1], Picinfo->linesize[2] };
				   // const GLsizei widths[3]    = { Picinfo->width, Picinfo->width/2, Picinfo->width/2};
					const GLsizei heights[3]   = { Picinfo->height,Picinfo->height / 2,     Picinfo->height / 2 };
					//***********************************Y***********************U**************************************V
					const GLubyte *pixels[3]   = {(GLubyte *)Picinfo->data[0], (GLubyte *)Picinfo->data[1] ,  (GLubyte *)Picinfo->data[2] };
					GLuint  plane_textures[]={g_texYId,g_texUId,g_texVId};
					 for (int i = 0; i < 3; ++i) {
						int plane = planes[i];

						glBindTexture(GL_TEXTURE_2D, plane_textures[i]);

						glTexImage2D(GL_TEXTURE_2D,
									 0,
									 GL_LUMINANCE,
									 widths[plane],
									 heights[plane],
									 0,
									 GL_LUMINANCE,
									 GL_UNSIGNED_BYTE,
									 pixels[plane]);
				        }
						/*
						 if (javaSurfaceTextureObj) {
							 m_penv->CallVoidMethod(javaSurfaceTextureObj, updateTexImageMethodId);
                        }*/
   
						
	            }else{
					  //glActiveTexture(GL_TEXTURE0) ;
                      //glBindTexture(GL_TEXTURE_EXTERNAL_OES, g_SurfaceTextVId) ;
					  //LOGI("MEDIACODEC  xxxxx \n");
				}
    }
}
void GlEs2Render::renderBk(unsigned char* buf,int len)
{
	
}
void GlEs2Render::SetWaitPic(unsigned char* buf,int len)
{
	
}
void GlEs2Render::LoadCenterLogo(unsigned char* buf,int len)
{
	
}
void GlEs2Render::SetErrPic(unsigned char* buf,int len)
{
	
}
void GlEs2Render::ShowErrPic(bool show)
{
	
}
void GlEs2Render::SetLeftPicStr(const char *str)
{

}
void GlEs2Render::FillRect(kkBitmap img,kkRect rt,unsigned int color)
{
	
	
}
void GlEs2Render::SetRenderImgCall(fpRenderImgCall fp,void* UserData)
{
	
	
}
bool GlEs2Render::GetHardInfo(void** pd3d,void** pd3ddev,int *ver)
{			
	return 0;
}
void GlEs2Render::SetResetHardInfoCall(fpResetDevCall call,void* UserData)
{
	
}
long long GlEs2Render::GetOnSizeTick()
{
 return 0;
}
void GlEs2Render::RetSetSizeTick()
{

}
void GlEs2Render::renderLock()
{
	
}
void GlEs2Render::renderUnLock()
{
	
}