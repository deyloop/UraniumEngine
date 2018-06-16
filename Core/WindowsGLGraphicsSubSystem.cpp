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
		HWND window = CreateWindow (
			WindowsOSFramework::getWindowsInstance ( )->getWindowClass ( ).lpszClassName,
			title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,CW_USEDEFAULT,
			width,
			height,
			NULL,NULL,
			GetModuleHandle (0),
			NULL
		);
		ShowWindow (window,WindowsOSFramework::getWindowsInstance ( )->getCmdShow ( ));
		UpdateWindow (window);

		WindowsOSFramework::getWindowsInstance ( )->setWindowHandle (window);
	}
}
