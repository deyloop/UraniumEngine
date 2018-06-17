#pragma once
#ifndef _GLEXTENTIONS_H_
#define _GLEXTENTIONS_H_

#include <Windows.h>
#include <gl/GL.h>
#include <GLext/glext.h>
#include <GLext/wglext.h>

extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

struct GL{
	 PFNGLCREATESHADERPROC   CreateShader;
	 PFNGLCREATEPROGRAMPROC  CreateProgram;
	 PFNGLSHADERSOURCEPROC   ShaderSource;
	 PFNGLCOMPILESHADERPROC  CompileShader;
	 PFNGLATTACHSHADERPROC   AttachShader;
	 PFNGLLINKPROGRAMPROC    LinkProgram;
	 PFNGLDELETESHADERPROC   DeleteShader;
	 PFNGLGETSHADERIVPROC    GetShaderiv;
	 PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
	 PFNGLUSEPROGRAMPROC     UseProgram;
	
	 PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
	 PFNGLGENBUFFERSPROC      GenBuffers;
	 PFNGLBINDVERTEXARRAYPROC BindVertexArray;
	 PFNGLBINDBUFFERPROC      BindBuffer;
	 PFNGLBUFFERDATAPROC      BufferData;
	 PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
	 PFNGLVERTEXATTRIBPOINTERPROC     VertexAttribPointer;
	
	 PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
	 PFNGLUNIFORMMATRIX4FVPROC   UniformMatrix4fv;

	 void (__stdcall *ClearColor)(GLclampf r,GLclampf g,GLclampf b,GLclampf a);
	 void (__stdcall *Clear)(GLbitfield mask);
	 void (__stdcall *ClearDepth)(GLclampd depth);
	 void (__stdcall *Enable)(GLenum cap);
	 void (__stdcall *FrontFace)(GLenum mode);
	 void (__stdcall *DrawArrays)(GLenum mode, GLint first, GLsizei count);
	 void (__stdcall *Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);
};
bool LoadGLExtensions(GL& gl);
bool LoadWGLExtensions();
#endif