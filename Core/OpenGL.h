#pragma once
#ifndef _GLEXTENTIONS_H_
#define _GLEXTENTIONS_H_

#include <Windows.h>
#include <gl/GL.h>
#include <GLext/glext.h>
#include <GLext/wglext.h>

extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

extern PFNGLCREATESHADERPROC   glCreateShader;
extern PFNGLCREATEPROGRAMPROC  glCreateProgram;
extern PFNGLSHADERSOURCEPROC   glShaderSource;
extern PFNGLCOMPILESHADERPROC  glCompileShader;
extern PFNGLATTACHSHADERPROC   glAttachShader;
extern PFNGLLINKPROGRAMPROC    glLinkProgram;
extern PFNGLDELETESHADERPROC   glDeleteShader;
extern PFNGLGETSHADERIVPROC    glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLUSEPROGRAMPROC     glUseProgram;

extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLGENBUFFERSPROC      glGenBuffers;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLBINDBUFFERPROC      glBindBuffer;
extern PFNGLBUFFERDATAPROC      glBufferData;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;

extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;

bool LoadGLExtensions();
bool LoadWGLExtensions();
#endif