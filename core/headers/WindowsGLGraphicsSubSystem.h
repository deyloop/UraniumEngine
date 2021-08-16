/******************************************************************************
Project	:	NOK Engine - An engine for networked games and Simulations

Copyright (C) 2018  Anurup Dey

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _WINDOWS_GL_GRAPHICS_SUBSYSTEM_H_
#define _WINDOWS_GL_GRAPHICS_SUBSYSTEM_H_

#include <Windows.h>
#include "OSGLGraphicsSubSystem.h"
#include "OSMessages.h"

namespace u92 {
	class WindowsGLGraphicsSubSystem :public OSGLGraphicsSubSystem {
	public:
		WindowsGLGraphicsSubSystem();
		~WindowsGLGraphicsSubSystem();

		int init();
		int release();

		int convertMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, WindowEvent& event);
		int handleCommandMsg(const WindowCommand msg);

		void createWindow (const char* title,int width,int hieght,bool fullscreen = false);
		bool initOpenGLContext (int versionMajor,int versionMinor);
		void swapBuffers ( );
		void setSwapInterval (int interval);

		GL& getGLInterface ( ) { return gl; }
	private:
		bool initGLExtentions ( );

		HDC m_deviceContext;
		HGLRC m_GLContext;
		GL gl;
	};
}

#endif