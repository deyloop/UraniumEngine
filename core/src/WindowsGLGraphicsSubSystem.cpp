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
#include "WindowsGLGraphicsSubSystem.h"
#include "WindowsOSFramework.h"
#include "OpenGL.h"

namespace u92 {
	WindowsGLGraphicsSubSystem::WindowsGLGraphicsSubSystem() {

	}
	
	WindowsGLGraphicsSubSystem::~WindowsGLGraphicsSubSystem() {
	
	}
	
	int WindowsGLGraphicsSubSystem::init() {
		return 0;
	}
	
	int WindowsGLGraphicsSubSystem::release() {
		return 0;
	}
	
	int WindowsGLGraphicsSubSystem::convertMessage(
		HWND window, UINT message, WPARAM wParam, 
		LPARAM lParam, WindowEvent & event) {
		if (window!=WindowsOSFramework::getWindowsInstance ( )->getWindowHandle ( ))
			return -1;
		switch (message) {
		case WM_CREATE:
			event.event.type = WINDOWEVENTTYPE_CREATE;
			return 0;
		case WM_PAINT: {
			event.event.type = WINDOWEVENTTYPE_PAINT;
			//it is compulsory to do this
			//This 'validates the window'
			//Else the OS will spam us with WM_PAINT forever.
			PAINTSTRUCT ps;
			HDC dc = BeginPaint (window,&ps);
			EndPaint (window,&ps );
		}return 0;
		case WM_SIZE:
			switch (wParam) {
			case SIZE_MINIMIZED: {
				event.resize.type = WINDOWEVENTTYPE_MINIMIZE;
			} return 0; 
			case SIZE_MAXIMIZED: {
				int newWidth  = LOWORD(lParam),
					newHieght = HIWORD(lParam);
				event.resize.type = WINDOWEVENTTYPE_MAXIMIZE;
				event.resize.newHieght = newHieght;
				event.resize.newWidth = newWidth;
			}return 0;
			default: {
				int newWidth  = LOWORD(lParam),
					newHieght = HIWORD(lParam);
				event.resize.type = WINDOWEVENTTYPE_RESIZE;
				event.resize.newHieght = newHieght;
				event.resize.newWidth = newWidth;
			}return 0;
			}//switch on wParam

		case WM_KILLFOCUS: {
			event.event.type = WINDOWEVENTTYPE_KILLFOCUS;
		}return 0;
		case WM_CLOSE: {
			event.event.type = WINDOWEVENTTYPE_CLOSE;
		}return 0;
		case WM_DESTROY: {
			event.event.type = WINDOWEVENTTYPE_DESTROY;
		}return 0;
		}

		return -1;
	}
	
	int WindowsGLGraphicsSubSystem::handleCommandMsg(const WindowCommand msg){
		switch (msg.type) {
			case WINDOWCOMMAND_NONE: {
				break;
			}
			case WINDOWCOMMAND_CREATEWINDOW: {
				createWindow (msg.command.createWindow.title,
							  msg.command.createWindow.width,
							  msg.command.createWindow.width,
							  msg.command.createWindow.fullscreen);
			} break;
			case WINDOWCOMMAND_DESTROY: {
				DestroyWindow (WindowsOSFramework::getWindowsInstance ( )->getWindowHandle ( ));
				WindowsOSFramework::getWindowsInstance ( )->setWindowHandle (NULL);
			} break;
			default: {
				break;
			}
		}
		return 0;
	}

	void WindowsGLGraphicsSubSystem::createWindow (const char * title,int width,int height,bool fullscreen) {
		int fwidth,fheight;
		RECT r;
		GetWindowRect (GetDesktopWindow ( ),&r);
		fwidth = r.right-r.left;
		fheight = r.bottom-r.top;

		HWND window = CreateWindow (
			WindowsOSFramework::getWindowsInstance ( )->getWindowClass ( ).lpszClassName,
			title,
			(fullscreen)? WS_SYSMENU|WS_POPUP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE:WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,CW_USEDEFAULT,
			(fullscreen)?fwidth:width,
			(fullscreen)?fheight:height,
			NULL,NULL,
			GetModuleHandle (0),
			NULL
		);
		ShowWindow (window,WindowsOSFramework::getWindowsInstance ( )->getCmdShow ( ));
		UpdateWindow (window);

		WindowsOSFramework::getWindowsInstance ( )->setWindowHandle (window);
		m_deviceContext = GetDC (window);

		initGLExtentions ( );
	}
	bool WindowsGLGraphicsSubSystem::initOpenGLContext (int versionMajor,int versionMinor) {
		bool bError = false;
		PIXELFORMATDESCRIPTOR pfd;

		if (versionMajor<=2) {
			return false;
		} else if (wglChoosePixelFormatARB!=nullptr && wglCreateContextAttribsARB!=nullptr) {
			const int iPixelFormatAttribList[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, true,
				WGL_SUPPORT_OPENGL_ARB, true,
				WGL_DOUBLE_BUFFER_ARB, true,
				WGL_TRANSPARENT_ARB, false,
				//WGL_PIXEL_TYPE_ARB		,		PixelParams.ePixel_Type		,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				0 // End of attributes list
			};
			int iContextAttribs[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, versionMajor,
				WGL_CONTEXT_MINOR_VERSION_ARB, versionMinor,
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0 // End of attributes list
			};

			int iPixelFormat,iNumFormats;
			wglChoosePixelFormatARB (m_deviceContext,
									 iPixelFormatAttribList,
									 NULL,
									 1,
									 &iPixelFormat,
									 (UINT*)&iNumFormats);

			// PFD seems to be only redundant parameter now
			if (!SetPixelFormat (m_deviceContext,iPixelFormat,&pfd))return false;

			m_GLContext = wglCreateContextAttribsARB (m_deviceContext,
													   0,iContextAttribs);
			// If everything went OK
			if (m_GLContext) wglMakeCurrent (m_deviceContext,m_GLContext);
			else bError = true;

		} else bError = true;

		if (bError) {
			// Generate error messages
			char sErrorMessage[255],sErrorTitle[255];
			sprintf_s (sErrorMessage,"OpenGL %d.%d is not supported! Please download latest GPU drivers!",
					  versionMajor,versionMinor);
			sprintf_s (sErrorTitle,"OpenGL %d.%d Not Supported",
					   versionMajor,versionMinor);
			MessageBox (NULL,sErrorMessage,sErrorTitle,MB_ICONINFORMATION);
			return false;
		}

		//Load all the opengl functions on this context now.
		if (!LoadGLExtensions (gl)) return false;

		return true;
	}
	void WindowsGLGraphicsSubSystem::swapBuffers ( ) {
		SwapBuffers (m_deviceContext);
	}

	void WindowsGLGraphicsSubSystem::setSwapInterval (int interval) {
		wglSwapIntervalEXT (interval);
	}

	bool WindowsGLGraphicsSubSystem::initGLExtentions ( ) {
		//To Initialize glew, we need a fake OpenGL Context.
		//For this Fake Context, We need a fake Pixel Format
		//For this fake Pixel Format, We need a fake window.
		HWND fakeWindow = CreateWindow(WindowsOSFramework::getWindowsInstance()->getWindowClass().lpszClassName,
									   "Fake",
									   WS_OVERLAPPEDWINDOW,
									   CW_USEDEFAULT,CW_USEDEFAULT,
									   1,1,
									   NULL,
									   NULL,
									   GetModuleHandle(0),
									   NULL);

		HDC fakeDC = GetDC (fakeWindow);

		PIXELFORMATDESCRIPTOR pfd = { };
		pfd.nSize = sizeof (PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER|PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int iPixelFormat = ChoosePixelFormat (fakeDC,&pfd);
		if (iPixelFormat==0) {
			DestroyWindow (fakeWindow);
			return false;
		}

		if (!SetPixelFormat (fakeDC,iPixelFormat,&pfd)) {
			DestroyWindow (fakeWindow);
			return false;
			//TODO: Better way to handle?
		}

		// Create the false, old style context (OpenGL 2.1 and before)

		HGLRC fakeGLContext = wglCreateContext (fakeDC);
		wglMakeCurrent (fakeDC,fakeGLContext);

		if (LoadWGLExtensions ( )==false) {
			return false;
		}

		wglMakeCurrent (NULL,NULL);
		wglDeleteContext (fakeGLContext);
		DestroyWindow (fakeWindow);

		return true;
	}
		
}
