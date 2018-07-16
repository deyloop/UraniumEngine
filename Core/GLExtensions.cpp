#include "OpenGL.h"

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
#define LoadGLExtention(name,type)\
    name = reinterpret_cast<type>(wglGetProcAddress( #name ));\
    if(name == nullptr){\
        return false;\
	}

#define LoadGLExtentionGL(glname,name,type)\
	name = reinterpret_cast<type>(wglGetProcAddress(#glname));\
	if(name == nullptr){\
		return false;\
	}

bool LoadWGLExtensions() {
	LoadGLExtention(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC);
	LoadGLExtention(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC);
	LoadGLExtention (wglSwapIntervalEXT,PFNWGLSWAPINTERVALEXTPROC);
}

bool LoadGLExtensions(GL& gl) {
	
	if(LoadWGLExtensions()) {

		gl.ClearColor	= glClearColor;
		gl.Clear		= glClear;
		gl.ClearDepth	= glClearDepth;
		gl.Enable		= glEnable;
		gl.FrontFace	= glFrontFace;
		gl.DrawArrays	= glDrawArrays;
		gl.Viewport		= glViewport;
		gl.GenTextures  = glGenTextures;
		gl.BindTexture  = glBindTexture;
		gl.TexImage2D	= glTexImage2D;
		gl.DeleteTextures = glDeleteTextures;
		gl.Disable		= glDisable;
		gl.BlendFunc	= glBlendFunc;
		gl.Finish		= glFinish;

		LoadGLExtentionGL(glCreateShader,		gl.CreateShader,        PFNGLCREATESHADERPROC);
		LoadGLExtentionGL(glCreateProgram,		gl.CreateProgram,		PFNGLCREATEPROGRAMPROC);
		LoadGLExtentionGL(glShaderSource,		gl.ShaderSource,		PFNGLSHADERSOURCEPROC);
		LoadGLExtentionGL(glCompileShader,		gl.CompileShader,		PFNGLCOMPILESHADERPROC);
		LoadGLExtentionGL(glAttachShader,		gl.AttachShader,		PFNGLATTACHSHADERPROC);
		LoadGLExtentionGL(glLinkProgram,		gl.LinkProgram,			PFNGLLINKPROGRAMPROC);
		LoadGLExtentionGL(glDeleteShader,		gl.DeleteShader,		PFNGLDELETESHADERPROC);
		LoadGLExtentionGL(glGetShaderiv,		gl.GetShaderiv,			PFNGLGETSHADERIVPROC);
		LoadGLExtentionGL(glGetShaderInfoLog,	gl.GetShaderInfoLog,	PFNGLGETSHADERINFOLOGPROC);
		LoadGLExtentionGL(glUseProgram,			gl.UseProgram,			PFNGLUSEPROGRAMPROC);
						
		LoadGLExtentionGL(glGenVertexArrays,			gl.GenVertexArrays,			PFNGLGENVERTEXARRAYSPROC);
		LoadGLExtentionGL(glGenBuffers,					gl.GenBuffers,				PFNGLGENBUFFERSPROC);
		LoadGLExtentionGL(glBindVertexArray,			gl.BindVertexArray,			PFNGLBINDVERTEXARRAYPROC);
		LoadGLExtentionGL(glBindBuffer,					gl.BindBuffer,				PFNGLBINDBUFFERPROC);
		LoadGLExtentionGL(glBufferData,					gl.BufferData,				PFNGLBUFFERDATAPROC);
		LoadGLExtentionGL(glEnableVertexAttribArray,	gl.EnableVertexAttribArray,	PFNGLENABLEVERTEXATTRIBARRAYPROC);
		LoadGLExtentionGL(glVertexAttribPointer,		gl.VertexAttribPointer,		PFNGLVERTEXATTRIBPOINTERPROC);
						
		LoadGLExtentionGL(glGetUniformLocation,	gl.GetUniformLocation,	PFNGLGETUNIFORMLOCATIONPROC);
		LoadGLExtentionGL(glUniformMatrix4fv,	gl.UniformMatrix4fv,	PFNGLUNIFORMMATRIX4FVPROC);
		LoadGLExtentionGL (glGenerateMipmap,gl.GenerateMipmap,PFNGLGENERATEMIPMAPPROC);
		LoadGLExtentionGL (glGenSamplers,gl.GenSamplers,PFNGLGENSAMPLERSPROC);
		LoadGLExtentionGL (glSamplerParameteri,gl.SamplerParameteri,PFNGLSAMPLERPARAMETERIPROC);
		LoadGLExtentionGL (glActiveTexture,gl.ActiveTexture,PFNGLACTIVETEXTUREPROC);
		LoadGLExtentionGL (glBindSampler,gl.BindSampler,PFNGLBINDSAMPLERPROC);
		LoadGLExtentionGL (glDeleteSamplers,gl.DeleteSamplers,PFNGLDELETESAMPLERSPROC);

		LoadGLExtentionGL (glGetProgramiv,gl.GetProgramiv,PFNGLGETPROGRAMIVPROC);
		LoadGLExtentionGL (glDeleteProgram,gl.DeleteProgram,PFNGLDELETEPROGRAMPROC);
		LoadGLExtentionGL (glGetProgramInfoLog,gl.GetProgramInfoLog,PFNGLGETPROGRAMINFOLOGPROC);

		LoadGLExtentionGL (glUniform1i,gl.Uniform1i,PFNGLUNIFORM1IPROC);
		LoadGLExtentionGL (glDeleteBuffers,gl.DeleteBuffers,PFNGLDELETEBUFFERSPROC);
		LoadGLExtentionGL (glDeleteVertexArrays,gl.DeleteVertexArrays,PFNGLDELETEVERTEXARRAYSPROC);

		return true;
	} else return false;
}