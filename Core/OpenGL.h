#pragma once
#ifndef _GLEXTENTIONS_H_
#define _GLEXTENTIONS_H_

#include <Windows.h>
#include <gl/GL.h>
#include <GLext/glext.h>
#include <GLext/wglext.h>

extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

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

	 PFNGLGENERATEMIPMAPPROC GenerateMipmap;
	 PFNGLGENSAMPLERSPROC GenSamplers;
	 PFNGLSAMPLERPARAMETERIPROC SamplerParameteri;
	 PFNGLACTIVETEXTUREPROC ActiveTexture;
	 PFNGLBINDSAMPLERPROC BindSampler;
	 PFNGLDELETESAMPLERSPROC DeleteSamplers;

	 PFNGLGETPROGRAMIVPROC GetProgramiv;
	 PFNGLDELETEPROGRAMPROC DeleteProgram;
	 PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
	 PFNGLUNIFORM1IPROC Uniform1i;
	 PFNGLDELETEBUFFERSPROC DeleteBuffers;
	 PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays;

	 void (__stdcall *ClearColor)(GLclampf r,GLclampf g,GLclampf b,GLclampf a);
	 void (__stdcall *Clear)(GLbitfield mask);
	 void (__stdcall *ClearDepth)(GLclampd depth);
	 void (__stdcall *Enable)(GLenum cap);
	 void (__stdcall *FrontFace)(GLenum mode);
	 void (__stdcall *DrawArrays)(GLenum mode, GLint first, GLsizei count);
	 void (__stdcall *Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);
	 void (__stdcall *GenTextures)(GLsizei n,GLuint *textures);
	 void (__stdcall *BindTexture)(GLenum target, GLuint texture);
	 void (__stdcall *TexImage2D)(GLenum target, GLint level,GLint internalformat,GLsizei width, GLsizei height,GLint border,GLenum format,GLenum type,const GLvoid *pixels);
	 void (__stdcall *DeleteTextures)(GLsizei n, const GLuint* textures);
	 void (__stdcall *Disable)(GLenum cap);
	 void (__stdcall *BlendFunc)(GLenum sfactor,GLenum dfactor);
	 void (__stdcall *Finish)();
};
bool LoadGLExtensions(GL& gl);
bool LoadWGLExtensions();
#endif