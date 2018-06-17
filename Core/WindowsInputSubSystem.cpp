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
#include "WindowsInputSubSystem.h"
#include "WindowsOSFramework.h"
#include "ErrorCode.h"

namespace u92 {
	WindowsInputSubSystem::WindowsInputSubSystem ( ) {
		m_first = true;
		m_keyInit = false;
	}

	WindowsInputSubSystem::~WindowsInputSubSystem ( ) { }

	int WindowsInputSubSystem::init ( ) {
		if (!m_keyInit) {
			m_keyBoardDevice.usUsagePage = 0x01;
			m_keyBoardDevice.usUsage = 0x06;
			m_keyBoardDevice.dwFlags = RIDEV_NOLEGACY;
			m_keyBoardDevice.hwndTarget = WindowsOSFramework::getWindowsInstance ( )->getWindowHandle ( );
			RegisterRawInputDevices (&m_keyBoardDevice,1,sizeof (m_keyBoardDevice));
			m_keyInit = true;
		}
		return E_CODE_SUCCESS;
	}

	int WindowsInputSubSystem::release ( ) {
		return 0;
	}

	int WindowsInputSubSystem::convertMessage (HWND window,UINT message,WPARAM wParam,LPARAM lParam,InputEvent & event) {
		POINTS ptCursor;
		static POINTS ptPrevCursor;

		event.event.timestamp = GetTickCount64 ( );
		switch (message) {

			case WM_QUIT:
				//The OS wants the App to quit.
				//So tell the caller the same.
				event.event.type = EVENT_QUIT;
				return 0;

				//Mouse
			case WM_MOUSEMOVE:
				if (m_first) {
					ptPrevCursor = MAKEPOINTS (lParam);
					m_first = false;
				}

				event.event.type = EVENT_MOUSEMOVE;
				ptCursor = MAKEPOINTS (lParam);
				event.mouse_motion.mouse_pos_x = ptCursor.x;
				event.mouse_motion.mouse_pos_y = ptCursor.y;
				event.mouse_motion.delta_x = ptCursor.x-ptPrevCursor.x;
				event.mouse_motion.delta_y = ptPrevCursor.y-ptCursor.y;

				ptPrevCursor = ptCursor;
				return 0;
			case WM_LBUTTONDOWN:
				event.event.type = EVENT_MOUSEBUTTONDOWN;
				event.mouse_button.button = MOUSE_BUTTON_L;
				SetCapture (window);
				return 0;
			case WM_MBUTTONDOWN:
				event.event.type = EVENT_MOUSEBUTTONDOWN;
				event.mouse_button.button = MOUSE_BUTTON_M;
				SetCapture (window);
				return 0;
			case WM_RBUTTONDOWN:
				SetCapture (window);
				event.event.type = EVENT_MOUSEBUTTONDOWN;
				event.mouse_button.button = MOUSE_BUTTON_R;
				return 0;
			case WM_LBUTTONUP:
				event.event.type = EVENT_MOUSEBUTTONUP;
				event.mouse_button.button = MOUSE_BUTTON_L;
				ReleaseCapture ( );
				return 0;
			case WM_MBUTTONUP:
				event.event.type = EVENT_MOUSEBUTTONUP;
				event.mouse_button.button = MOUSE_BUTTON_M;
				ReleaseCapture ( );
				return 0;
			case WM_RBUTTONUP:
				event.event.type = EVENT_MOUSEBUTTONUP;
				event.mouse_button.button = MOUSE_BUTTON_R;
				ReleaseCapture ( );
				return 0;

				//Keyboard
			case WM_INPUT: {
				char buffer[sizeof (RAWINPUT)] = {};
				UINT size = sizeof (RAWINPUT);
				GetRawInputData (reinterpret_cast<HRAWINPUT>(lParam),
								 RID_INPUT,
								 buffer,
								 &size,
								 sizeof (RAWINPUTHEADER));
							 //extract keyboard input data
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);
				if (raw->header.dwType==RIM_TYPEKEYBOARD) {
					if (EvaluateKeyBaordInput (raw->data.keyboard,&event)>=0) {
						return 0;
					}
				}//keyboard

			}//input

		}//switch on message 

		return -1;
	}

	int WindowsInputSubSystem::handleCommandMsg (const InputCommand msg) {

		return 0;
	}

	
	int WindowsInputSubSystem::EvaluateKeyBaordInput (const RAWKEYBOARD & rawKB,InputEvent * pEvent) {
		UINT virtualKey = rawKB.VKey;
		UINT scanCode = rawKB.MakeCode;
		UINT flags = rawKB.Flags;

		// a key can either produce a "make" or "break" scancode. 
		//this is used to differentiate between down-presses and releases
		const bool wasUp = ((flags & RI_KEY_BREAK)!=0);
		if (wasUp) pEvent->type = EVENT_KEYUP;
		else pEvent->type = EVENT_KEYDOWN;

		if (virtualKey==255) {
			//discard 'fake keys' which are part of an 
			//escape sequence
			return -1;
		} else if (virtualKey==VK_SHIFT) {
			//correct left hand/right hand shift.
			virtualKey = MapVirtualKey (scanCode,MAPVK_VSC_TO_VK_EX);
		} else if (virtualKey==VK_NUMLOCK) {
			//correct pause/break and numlock silliness
			//and set extended bit.
			scanCode = MapVirtualKey (virtualKey,MAPVK_VK_TO_VSC)|0x100;
		}

		//e0 and e1 are escape sequences used for certain special keys
		//such as PRINT and PAUSE/BREAK.
		const bool isE0 = ((flags & RI_KEY_E0)!=0);
		const bool isE1 = ((flags & RI_KEY_E1)!=0);

		if (isE1)
			//for escaped sequences, turn the virtual key code 
			//into the correct Scancode.
			//However, MapVertualKey is unable to map VK_PAUSE 
			//(its a known bug. We do that by hand.
			if (virtualKey==VK_PAUSE)
				scanCode = 0x45;
			else
				scanCode = MapVirtualKey (virtualKey,MAPVK_VK_TO_VSC);

		// getting a human-readable string
		UINT key = (scanCode<<16)|(isE0<<24);
		GetKeyNameText ((LONG)key,pEvent->key.KeyName,20);

		switch (virtualKey) {
			// right-hand CONTROL and ALT have their e0 bit set
			case VK_CONTROL:
				if (isE0) {
					pEvent->key.keycode = KEY_RCTRL;
					return 0;
				} else {
					pEvent->key.keycode = KEY_LCTRL;
				}

			case VK_MENU:
				if (isE0) {
					pEvent->key.keycode = KEY_RALT;
					return 0;
				} else {
					pEvent->key.keycode = KEY_LALT;
					return 0;
				}

				// NUMPAD ENTER has its e0 bit set
			case VK_RETURN:
				if (isE0) {
					pEvent->key.keycode = KEY_NUM_ENTER;
					return 0;
				} else {
					pEvent->key.keycode = KEY_ENTER;
					return 0;
				}

				// the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys will always have their e0 bit set, but the
				// corresponding keys on the NUMPAD will not.
			case VK_INSERT:
				if (!isE0) {
					pEvent->key.keycode = KEY_NUM_0;
					return 0;
				} else {
					pEvent->key.keycode = KEY_INSERT;
					return 0;
				}

			case VK_DELETE:
				if (!isE0) {
					pEvent->key.keycode = KEY_NUM_DECIMAL;
					return 0;
				} else {
					pEvent->key.keycode = KEY_DELETE;
					return 0;
				}

			case VK_HOME:
				if (!isE0) {
					pEvent->key.keycode = KEY_NUM_7;
				} else {
					pEvent->key.keycode = KEY_HOME;
				}
				return 0;

			case VK_END:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_1;
				else
					pEvent->key.keycode = KEY_END;
				return 0;

			case VK_PRIOR:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_9;
				else
					pEvent->key.keycode = KEY_PAGEUP;
				return 0;

			case VK_NEXT:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_3;
				else
					pEvent->key.keycode = KEY_PAGEDOWN;
				return 0;

				// the standard arrow keys will always have their e0 bit set, but the
				// corresponding keys on the NUMPAD will not.
			case VK_LEFT:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_4;
				else
					pEvent->key.keycode = KEY_ARROW_LEFT;
				return 0;

			case VK_RIGHT:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_6;
				else
					pEvent->key.keycode = KEY_ARROW_RIGHT;
				return 0;

			case VK_UP:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_8;
				else
					pEvent->key.keycode = KEY_ARROW_UP;
				return 0;

			case VK_DOWN:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_2;
				else
					pEvent->key.keycode = KEY_ARROW_DOWN;
				return 0;

				// NUMPAD 5 doesn't have its e0 bit set
			case VK_CLEAR:
				if (!isE0)
					pEvent->key.keycode = KEY_NUM_5;
				else
					pEvent->key.keycode = KEY_CLEAR;
				return 0;

			case VK_ESCAPE:
				pEvent->key.keycode = KEY_ESCAPE;
				return 0;
				//Now for the Function keys

			case VK_F1:
				pEvent->key.keycode = KEY_F1;
				return 0;

			case VK_F2:
				pEvent->key.keycode = KEY_F2;
				return 0;

			case VK_F3:
				pEvent->key.keycode = KEY_F3;
				return 0;

			case VK_F4:
				pEvent->key.keycode = KEY_F4;
				return 0;

			case VK_F5:
				pEvent->key.keycode = KEY_F5;
				return 0;

			case VK_F6:
				pEvent->key.keycode = KEY_F6;
				return 0;

			case VK_F7:
				pEvent->key.keycode = KEY_F7;
				return 0;

			case VK_F8:
				pEvent->key.keycode = KEY_F8;
				return 0;

			case VK_F9:
				pEvent->key.keycode = KEY_F9;
				return 0;

			case VK_F10:
				pEvent->key.keycode = KEY_F10;
				return 0;

			case VK_F11:
				pEvent->key.keycode = KEY_F11;
				return 0;

			case VK_F12:
				pEvent->key.keycode = KEY_F12;
				return 0;
			case VK_SPACE:
				pEvent->key.keycode = KEY_SPACEBAR;
				return 0;
			case VK_LSHIFT:
				pEvent->key.keycode = KEY_LSHIFT;
				return 0;
			case VK_RSHIFT:
				pEvent->key.keycode = KEY_RSHIFT;
				return 0;
			case VK_NUMLOCK:
				pEvent->key.keycode = KEY_NUMLOCK;
				return 0;
			case VK_CAPITAL:
				pEvent->key.keycode = KEY_CAPSLOCK;
				return 0;
			case VK_NUMPAD0:
				pEvent->key.keycode = KEY_NUM_0;
				return 0;
			case VK_NUMPAD1:
				pEvent->key.keycode = KEY_NUM_1;
				return 0;
			case VK_NUMPAD2:
				pEvent->key.keycode = KEY_NUM_2;
				return 0;
			case VK_NUMPAD3:
				pEvent->key.keycode = KEY_NUM_3;
				return 0;
			case VK_NUMPAD4:
				pEvent->key.keycode = KEY_NUM_4;
				return 0;
			case VK_NUMPAD5:
				pEvent->key.keycode = KEY_NUM_5;
				return 0;
			case VK_NUMPAD6:
				pEvent->key.keycode = KEY_NUM_6;
				return 0;
			case VK_NUMPAD7:
				pEvent->key.keycode = KEY_NUM_7;
				return 0;
			case VK_NUMPAD8:
				pEvent->key.keycode = KEY_NUM_8;
				return 0;
			case VK_NUMPAD9:
				pEvent->key.keycode = KEY_NUM_9;
				return 0;
			case '\b':
				pEvent->key.keycode = KEY_BACKSPACE;
				return 0;
			case '\t':
				pEvent->key.keycode = KEY_TAB;
				return 0;
			case 'A':
				pEvent->key.keycode = KEY_A;
				return 0;
			case 'B':
				pEvent->key.keycode = KEY_B;
				return 0;
			case 'C':
				pEvent->key.keycode = KEY_C;
				return 0;
			case 'D':
				pEvent->key.keycode = KEY_D;
				return 0;
			case 'E':
				pEvent->key.keycode = KEY_E;
				return 0;
			case 'F':
				pEvent->key.keycode = KEY_F;
				return 0;
			case 'G':
				pEvent->key.keycode = KEY_G;
				return 0;
			case 'H':
				pEvent->key.keycode = KEY_H;
				return 0;
			case 'I':
				pEvent->key.keycode = KEY_I;
				return 0;
			case 'J':
				pEvent->key.keycode = KEY_J;
				return 0;
			case 'K':
				pEvent->key.keycode = KEY_K;
				return 0;
			case 'L':
				pEvent->key.keycode = KEY_L;
				return 0;
			case 'M':
				pEvent->key.keycode = KEY_M;
				return 0;
			case 'N':
				pEvent->key.keycode = KEY_N;
				return 0;
			case 'O':
				pEvent->key.keycode = KEY_O;
				return 0;
			case 'P':
				pEvent->key.keycode = KEY_P;
				return 0;
			case 'Q':
				pEvent->key.keycode = KEY_Q;
				return 0;
			case 'R':
				pEvent->key.keycode = KEY_R;
				return 0;
			case 'S':
				pEvent->key.keycode = KEY_S;
				return 0;
			case 'T':
				pEvent->key.keycode = KEY_T;
				return 0;
			case 'U':
				pEvent->key.keycode = KEY_U;
				return 0;
			case 'V':
				pEvent->key.keycode = KEY_V;
				return 0;
			case 'W':
				pEvent->key.keycode = KEY_W;
				return 0;
			case 'X':
				pEvent->key.keycode = KEY_X;
				return 0;
			case 'Y':
				pEvent->key.keycode = KEY_Y;
				return 0;
			case 'Z':
				pEvent->key.keycode = KEY_Z;
				return 0;
			case '1':
				pEvent->key.keycode = KEY_1;
				return 0;
			case '2':
				pEvent->key.keycode = KEY_2;
				return 0;
			case '3':
				pEvent->key.keycode = KEY_3;
				return 0;
			case '4':
				pEvent->key.keycode = KEY_4;
				return 0;
			case '5':
				pEvent->key.keycode = KEY_5;
				return 0;
			case '6':
				pEvent->key.keycode = KEY_6;
				return 0;
			case '7':
				pEvent->key.keycode = KEY_7;
				return 0;
			case '8':
				pEvent->key.keycode = KEY_8;
				return 0;
			case '9':
				pEvent->key.keycode = KEY_9;
				return 0;
			case '0':
				pEvent->key.keycode = KEY_0;
				return 0;
			case VK_OEM_COMMA:
				pEvent->key.keycode = KEY_COMMA;
				return 0;
			case VK_OEM_PERIOD:
				pEvent->key.keycode = KEY_PERIOD;
				return 0;
			case VK_OEM_MINUS:
				pEvent->key.keycode = KEY_MINUS;
				return 0;
			case VK_OEM_PLUS:
				pEvent->key.keycode = KEY_PLUS;
				return 0;
			case VK_SUBTRACT:
				pEvent->key.keycode = KEY_NUM_MINUS;
				return 0;
			case VK_ADD:
				pEvent->key.keycode = KEY_NUM_PLUS;
				return 0;
			case VK_MULTIPLY:
				pEvent->key.keycode = KEY_NUM_MULTIPLY;
				return 0;
			case VK_SEPARATOR:
				pEvent->key.keycode = KEY_QUESTIONMARK;
				return 0;
			case VK_DIVIDE:
				pEvent->key.keycode = KEY_NUM_DIVIDE;
				return 0;
			case VK_OEM_1://;: key
				pEvent->key.keycode = KEY_SEMICOLON;
				return 0;
			case VK_OEM_2://? key
				pEvent->key.keycode = KEY_QUESTIONMARK;
				return 0;
			case VK_OEM_3://~
				pEvent->key.keycode = KEY_TILDY;
				return 0;
			case VK_OEM_4://'[{' key
				pEvent->key.keycode = KEY_CURLYBRACE_OPEN;
				return 0;
			case VK_OEM_5://'\|' key
				pEvent->key.keycode = KEY_FORWARDSLASH;
				return 0;
			case VK_OEM_6://']}' key
				pEvent->key.keycode = KEY_CURLYBRACE_CLOSE;
				return 0;
			case VK_OEM_7://'single quote' key
				pEvent->key.keycode = KEY_QUOTE;
				return 0;
				//all keys covered.
			default:
				return -1;
		}
	}
}
