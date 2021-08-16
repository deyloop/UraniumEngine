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
#include "WindowsOSFramework.h"
#include "ErrorCode.h"

#include "WindowsGLGraphicsSubSystem.h"
#include "WindowsInputSubSystem.h"
#include "WindowsNetworkSubSystem.h"
#include "System.h"

namespace u92 {
	WindowsOSFramework* WindowsOSFramework::m_pInstance = nullptr;

	System * WindowsOSFramework::loadSystemModule(const char * moduleName) {
		HMODULE module = LoadLibrary(moduleName);
		if (!module) return nullptr;
		//get the load function.
		System*(*getSystemInstance)(void) = (System*(*)())GetProcAddress(module, "getSystemInstance");
		System* pSystem = getSystemInstance ( );
		return pSystem;
	}

	WindowsOSFramework::WindowsOSFramework(){
		m_pGraphicsSubsystem	= nullptr;
		m_pInputSubSystem		= nullptr;
		m_pNetworkSubSystem		= nullptr;
		m_windowClass			= {};
		m_windowHandle			= NULL;
	}

	WindowsOSFramework::~WindowsOSFramework(){
		release();
	}

	int WindowsOSFramework::init(HINSTANCE instance,int cmdShow){
		m_windowClass.cbSize		= sizeof(WNDCLASSEX);
		m_windowClass.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		m_windowClass.lpfnWndProc	= (WNDPROC)WindowsOSFramework::staticWinProc;
		m_windowClass.cbClsExtra	= 0;
		m_windowClass.cbWndExtra	= 0;
		m_windowClass.hInstance		= instance;
		m_windowClass.hIcon			= NULL;
		m_windowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		m_windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		m_windowClass.lpszMenuName	= NULL;
		m_windowClass.lpszClassName = "NOK_EngineWindow";
		m_windowClass.hIconSm		= NULL;

		ATOM result = RegisterClassEx(&m_windowClass);
		if (result == NULL) {
			return E_CODE_REGISTERCLASSFAILED;
		}

		//register the message handlers
		registerHandler<WindowCommand> (std::bind (&WindowsOSFramework::handleWindowCommand,this,std::placeholders::_1));
		registerHandler<InputCommand> (std::bind (&WindowsOSFramework::handleInputCommand,this,std::placeholders::_1));
		registerHandler<QuitMessage> (std::bind (&WindowsOSFramework::handleQuitMessage,this,std::placeholders::_1));

		subscribe (10);

		m_cmdShow = cmdShow;
		
		m_quit.posted = false;

		return E_CODE_SUCCESS;
	}

	void WindowsOSFramework::release(){
		int ret = releaseSubSystem(SUBSYSTEM_ALL);
		//TODO: handle errors here.
	}

	int WindowsOSFramework::initSubSystem(SubSystemType subSystemType) {
		int ret = E_CODE_SUCCESS;
		switch (subSystemType) {
		case SUBSYSTEM_ALL:
			ret = initSubSystem(SUBSYSTEM_OPENGL_GRAPHICS);
			if (ret != E_CODE_SUCCESS)
				break;
			ret = initSubSystem(SUBSYSTEM_INPUT);
			break;
		case SUBSYSTEM_INPUT:
			m_pInputSubSystem = new WindowsInputSubSystem;
			ret = m_pInputSubSystem->init();
			break;
		case SUBSYSTEM_OPENGL_GRAPHICS:
			m_pGraphicsSubsystem = new WindowsGLGraphicsSubSystem;
			ret = m_pGraphicsSubsystem->init();
			break;
		case SUBSYSTEM_NETWORK:
			m_pNetworkSubSystem = new WindowsNetworkSubSystem;
			ret = m_pNetworkSubSystem->init ( );
			break;
		default:
			ret = E_CODE_UNKOWN_SUBSYSTEM;
		}
		return ret;
	}

	int WindowsOSFramework::releaseSubSystem(SubSystemType subsystemType){
		int ret = E_CODE_SUCCESS;
		switch (subsystemType) {
		case SUBSYSTEM_ALL:
			ret = releaseSubSystem(SUBSYSTEM_INPUT);
			if (ret != E_CODE_SUCCESS)
				break;
			ret = releaseSubSystem(SUBSYSTEM_OPENGL_GRAPHICS);
			break;
		case SUBSYSTEM_INPUT:
			if (m_pInputSubSystem) {
				ret = m_pInputSubSystem->release();
				if (ret != E_CODE_SUCCESS)
					break;
				delete m_pInputSubSystem;
				m_pInputSubSystem = nullptr;
			}
			break;
		case SUBSYSTEM_OPENGL_GRAPHICS:
			if (m_pGraphicsSubsystem) {
				ret = m_pGraphicsSubsystem->release();
				if (ret != E_CODE_SUCCESS)
					break;
				delete m_pGraphicsSubsystem;
				m_pGraphicsSubsystem = nullptr;
			} 
			break;
		case SUBSYSTEM_NETWORK: {
			if (m_pNetworkSubSystem) {
				m_pNetworkSubSystem->release ( );
				delete m_pNetworkSubSystem;
				m_pNetworkSubSystem = nullptr;
			}
		}break;
		default:
			ret = E_CODE_UNKOWN_SUBSYSTEM;
		}
		return ret;
	}

	OSGLGraphicsSubSystem * WindowsOSFramework::getOpenGLGraphicsSubSystem ( ) {
		return m_pGraphicsSubsystem;
	}

	OSInputSubSystem * WindowsOSFramework::getInputSubSystem ( )
	{
		return m_pInputSubSystem;
	}

	OSNetworkSubSystem* WindowsOSFramework::getNetworkSubSystem ( ) {
		return m_pNetworkSubSystem;
	}
	int WindowsOSFramework::handleOSMessages() {
		int ret = E_CODE_SUCCESS;

		//check to see if internal queues have anything, if yes,
		//process them from here.
		
		if (m_quit.posted) {
			PostQuitMessage (m_quit.exitCode);
		}

		//Window command check
		while (m_pInputSubSystem&&!m_inputCommandQueue.empty ( )) {
			m_pInputSubSystem->handleCommandMsg (m_inputCommandQueue.front ( ));
			m_inputCommandQueue.pop ( );
		}

		while (m_pGraphicsSubsystem&&!m_windowCommandQueue.empty()) {
			m_pGraphicsSubsystem->handleCommandMsg(m_windowCommandQueue.front ( ));
			m_windowCommandQueue.pop ( );
		}

		MSG msg;
		int nummsg = 0;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
			if (msg.message == WM_QUIT || m_quit.posted) {
				//return and error, so core will stop.
				ret = E_CODE_QUIT_MESSAGE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage	(&msg);
			nummsg++;
			//OutputDebugString ("msg\n");
		}
		//OutputDebugString ("frame\n");
		return ret;
	}

	void WindowsOSFramework::handleWindowCommand (const WindowCommand msg) {
		m_windowCommandQueue.push (msg);
	}

	void WindowsOSFramework::handleInputCommand (const InputCommand msg) {
		m_inputCommandQueue.push (msg);
	}

	void WindowsOSFramework::handleQuitMessage (const QuitMessage msg) {
		m_quit.exitCode = msg.exitCode;
		m_quit.posted = true;
	}

	OSFramework* OSFramework::getInstance() {
		return WindowsOSFramework::getWindowsInstance();
	}

	WindowsOSFramework * WindowsOSFramework::getWindowsInstance(){
		if (!m_pInstance)
			m_pInstance = new WindowsOSFramework;
		return m_pInstance;
	}

	LRESULT WindowsOSFramework::staticWinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return getWindowsInstance()->windowProc(window, message, wParam, lParam);
	}

	LRESULT WindowsOSFramework::windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
		LRESULT ret = 0;
		int handled = -1;

		//if the Input sub system is active, we want to let it convert 
		//the window message into a platform agnostic form of inputEvent.
		if (m_pInputSubSystem) {
			InputEvent event;
			handled = m_pInputSubSystem->convertMessage(window, message, wParam, lParam, event);
			if (handled == 0) {
				//we push this event into the Message bus;
				postMessage(event,1);
			}
		}

		if (m_pGraphicsSubsystem && handled != 0) {
			//if the Input subsystem cannot handle the event, pass it to the 
			//Graphics subsystem, to see if it is a Window Message.
			WindowEvent event;
			handled = m_pGraphicsSubsystem->convertMessage(window, message, wParam, lParam, event);
			if (handled == 0) {
				//push it to message bus
				postMessage(event,2);
			}
		}
		//if nothing comes up, then we just do nothing.
		if (handled == -1) {
			ret = DefWindowProc(window, message, wParam, lParam);
		}

		return ret;
	}
}
