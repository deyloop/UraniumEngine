#include "OpenGL.h"

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

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
}

bool LoadGLExtensions(GL& gl) {
	
	if(LoadWGLExtensions()) {

		gl.ClearColor	= &glClearColor;
		gl.Clear		= &glClear;
		gl.ClearDepth	= &glClearDepth;
		gl.Enable		= &glEnable;
		gl.FrontFace	= &glFrontFace;
		gl.DrawArrays	= &glDrawArrays;
		gl.Viewport		= &glViewport;

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

		return true;
	} else return false;
}