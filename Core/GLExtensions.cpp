#include "OpenGL.h"

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

PFNGLCREATESHADERPROC   glCreateShader   = nullptr;
PFNGLCREATEPROGRAMPROC  glCreateProgram = nullptr;
PFNGLSHADERSOURCEPROC   glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC  glCompileShader = nullptr;
PFNGLATTACHSHADERPROC   glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC    glLinkProgram = nullptr;
PFNGLDELETESHADERPROC   glDeleteShader = nullptr;
PFNGLGETSHADERIVPROC    glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLUSEPROGRAMPROC     glUseProgram = nullptr;

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLGENBUFFERSPROC      glGenBuffers = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLBINDBUFFERPROC      glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC      glBufferData = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer = nullptr;

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv = nullptr;

#define LoadGLExtention(name,type)\
    name = reinterpret_cast<type>(wglGetProcAddress( #name ));\
    if(name == nullptr){\
        return false;\
	}
bool LoadWGLExtensions() {
	LoadGLExtention(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC);

	LoadGLExtention(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC);
}

bool LoadGLExtensions() {
	
	if(LoadWGLExtensions()) {
		LoadGLExtention(glCreateShader, PFNGLCREATESHADERPROC);
		LoadGLExtention(glCreateProgram, PFNGLCREATEPROGRAMPROC);
		LoadGLExtention(glShaderSource, PFNGLSHADERSOURCEPROC);
		LoadGLExtention(glCompileShader, PFNGLCOMPILESHADERPROC);
		LoadGLExtention(glAttachShader, PFNGLATTACHSHADERPROC);
		LoadGLExtention(glLinkProgram, PFNGLLINKPROGRAMPROC);
		LoadGLExtention(glDeleteShader, PFNGLDELETESHADERPROC);
		LoadGLExtention(glGetShaderiv, PFNGLGETSHADERIVPROC);
		LoadGLExtention(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
		LoadGLExtention(glUseProgram, PFNGLUSEPROGRAMPROC);

		LoadGLExtention(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
		LoadGLExtention(glGenBuffers, PFNGLGENBUFFERSPROC);
		LoadGLExtention(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
		LoadGLExtention(glBindBuffer, PFNGLBINDBUFFERPROC);
		LoadGLExtention(glBufferData, PFNGLBUFFERDATAPROC);
		LoadGLExtention(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
		LoadGLExtention(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);

		LoadGLExtention(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
		LoadGLExtention(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);

		return true;
	} else return false;
}