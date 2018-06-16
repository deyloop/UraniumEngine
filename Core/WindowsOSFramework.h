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
#ifndef _WINDOWS_OS_FRAMEWORK_
#define _WINDOWS_OS_FRAMEWORK_

#include <Windows.h>
#include <queue>
#include "OSFramework.h"
#include "OSMessages.h"

namespace u92 {
	class WindowsInputSubSystem;
	class WindowsGLGraphicsSubSystem;

	class WindowsOSFramework : public OSFramework {
	public:
		~WindowsOSFramework();

		int init(HINSTANCE instance, int cmdShow);
		void release();

		int initSubSystem	 (SubSystemType subSystemType);
		int releaseSubSystem (SubSystemType subsystemType);

		OSGLGraphicsSubSystem*	getOpenGLGraphicsSubSystem ( );
		OSInputSubSystem*		getInputSubSystem ( );

		int handleOSMessages();

		static WindowsOSFramework* getWindowsInstance();

		HWND getWindowHandle() { return m_windowHandle; };
		void setWindowHandle(HWND windowHandle) { m_windowHandle = windowHandle; };
		WNDCLASSEX getWindowClass() { return m_windowClass; }
		int getCmdShow ( ) { return m_cmdShow; }
		System* loadSystemModule(const char* moduleName);

		void threadInit ( ) { };

	private:
		WindowsOSFramework();

		void handleWindowCommand (const WindowCommand msg);
		void handleInputCommand (const InputCommand msg);
		void handleQuitMessage (const QuitMessage msg);

		static LRESULT CALLBACK staticWinProc(	HWND window,	UINT message,
												WPARAM wParam,	LPARAM lParam);
		LRESULT CALLBACK windowProc(HWND window, UINT message,
									WPARAM wParam, LPARAM lParam);
		static WindowsOSFramework* m_pInstance;
		
		WindowsInputSubSystem*		m_pInputSubSystem;
		WindowsGLGraphicsSubSystem*	m_pGraphicsSubsystem;

		HWND m_windowHandle;
		WNDCLASSEX m_windowClass;
		int m_cmdShow;

		std::queue<InputCommand> m_inputCommandQueue;
		std::queue<WindowCommand> m_windowCommandQueue;
		
		struct {
			bool posted;
			int exitCode;
		} m_quit;
	}; 
}

#endif