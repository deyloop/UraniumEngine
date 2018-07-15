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

#include <sstream>

namespace u92 {
	/*-------------------------------------------------------------------------
	THe default constructor
	-------------------------------------------------------------------------*/
	WindowsInputSubSystem::WindowsInputSubSystem ( ) {
		m_ResetMouseTracking		= true;				
		m_Init					= false;
	}

	/*-------------------------------------------------------------------------
	Default destructor.
	-------------------------------------------------------------------------*/
	WindowsInputSubSystem::~WindowsInputSubSystem ( ) {
	
	}

	/*-------------------------------------------------------------------------
	initializes the system. Can be called multiple times, only the first time 
	is effective, successive attempts at initialisation are ignored.
	RETURNS: 
		E_CODE_SUCCESS if all goes good.
	-------------------------------------------------------------------------*/
	int WindowsInputSubSystem::init ( ) {
		if (!m_Init) {
			#ifndef HID_USAGE_PAGE_GENERIC
			#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
			#endif
			#ifndef HID_USAGE_GENERIC_MOUSE
			#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
			#endif

			#define RAWMOUSEDEVICE		0
			#define RAWKEYBOARDDEVICE	1

			const HWND window = WindowsOSFramework::getWindowsInstance ( )->getWindowHandle ( );
			RAWINPUTDEVICE Rid[2];
			Rid[RAWMOUSEDEVICE].usUsagePage	= HID_USAGE_PAGE_GENERIC;
			Rid[RAWMOUSEDEVICE].usUsage		= HID_USAGE_GENERIC_MOUSE;
			Rid[RAWMOUSEDEVICE].dwFlags		= 0; //we do want legacy mouse input to be active
			Rid[RAWMOUSEDEVICE].hwndTarget	= window;
			
			Rid[RAWKEYBOARDDEVICE].usUsagePage	= 0x01;
			Rid[RAWKEYBOARDDEVICE].usUsage		= 0x06;				//this is the keyboard
			Rid[RAWKEYBOARDDEVICE].dwFlags		= RIDEV_NOLEGACY;	//No legacy keyboard input wanted
			Rid[RAWKEYBOARDDEVICE].hwndTarget	= window;

			if (RegisterRawInputDevices (Rid,2,sizeof (RAWINPUTDEVICE))==FALSE) {
				//registration failed.
				DWORD ecode = GetLastError ( );
				//TODO: Check what error happened, act accordingly.
				return E_CODE_FAIL;
			}

			setMouseRelMode (true);

			m_Init = true;
		}
		return E_CODE_SUCCESS;
	}

	/*-------------------------------------------------------------------------
	Destructor. Frees up all resources.
	-------------------------------------------------------------------------*/
	int WindowsInputSubSystem::release ( ) {
		//TODO: check if raw input devices need to un registered.
		
		return 0;
	}

	/*-------------------------------------------------------------------------
	Extracts X and Y coords of Mouse position from the LPARAM value.
	PARAMETERS:
	[IN]	window		-	Handle to the window that the input is for.
	[IN]	lParam		-	the LPARAM value containing the coords.
	[OUT]	x			-	will be set to the extracted x coord.
	[OUT]	y			-	will be set to the extracted y coord.
	-------------------------------------------------------------------------*/
	void LParamToMouseXY (const HWND window,const LPARAM lParam,float &x,float &y) {
		RECT   rect;
		GetWindowRect (window,&rect);
		int width  = rect.right-rect.left;
		int height = rect.bottom-rect.top;

		POINTS ptCursor = MAKEPOINTS (lParam);
		x = ptCursor.x/(width/2.0f);
		y = ptCursor.y/(height/2.0f);
	}

	/*-------------------------------------------------------------------------
	Converts Windows Messages into InputEvent Messages, if they can be 
	converted.
	PARAMETERS:
	[IN]	window		-	handle to the window for which the message coresponds
	[IN]	message		-	the windows message to process/convert.
	[IN]	wParam		-	the WPARAM of the message.
	[IN]	lParam		-	the LPARAM of the message.
	[OUT]	event		-	the converted event, if it can be processed.
	RETURNS:
		0	if the message was successfully processed.
		-1  otherwise.
	-------------------------------------------------------------------------*/
	int WindowsInputSubSystem::convertMessage (HWND window,UINT message,
											   WPARAM wParam,LPARAM lParam,
											   InputEvent & event) {
		static POINTS ptPrevCursor;

		event.event.timestamp = GetTickCount64 ( );
		switch (message) {

			//Mouse
			case WM_MOUSEMOVE: {
				if (m_mouseRelMode) return 0;

				RECT rect;
				GetWindowRect (window,&rect);
				int width  = rect.right-rect.left;
				int height = rect.bottom-rect.top;

				POINTS ptCursor = MAKEPOINTS(lParam);
				if (m_ResetMouseTracking) {
					m_ResetMouseTracking = false;
					ptPrevCursor = ptCursor;
				}

				event.mouse_motion.type = EVENT_MOUSEMOVE;
				
				event.mouse_motion.mouse_pos_x = ptCursor.x/(width/2.0f);
				event.mouse_motion.mouse_pos_y = ptCursor.y/(height/2.0f);

				float prev_x = ptPrevCursor.x/(width/2.0f);
				float prev_y = ptPrevCursor.y/(height/2.0f);

				event.mouse_motion.delta_x = event.mouse_motion.mouse_pos_x-prev_x;
				event.mouse_motion.delta_y = event.mouse_motion.mouse_pos_y-prev_y;

				ptPrevCursor = ptCursor;
			} return 0;
			case WM_LBUTTONDOWN: {
				if (m_mouseRelMode) return 0;
				event.event.type = EVENT_MOUSEBUTTONDOWN;
				event.mouse_button.button = MOUSE_BUTTON_L;
				
				LParamToMouseXY(window,lParam,
								event.mouse_button.mouse_pos_x, 
								event.mouse_button.mouse_pos_y);
				SetCapture (window);
			} return 0;
			case WM_MBUTTONDOWN: {
				if (m_mouseRelMode) return 0;
				event.event.type = EVENT_MOUSEBUTTONDOWN;
				event.mouse_button.button = MOUSE_BUTTON_M;

				LParamToMouseXY (window,lParam,
								 event.mouse_button.mouse_pos_x,
								 event.mouse_button.mouse_pos_y);

				SetCapture (window);
			} return 0;
			case WM_RBUTTONDOWN: {
				if (m_mouseRelMode) return 0;
				SetCapture (window);
				event.event.type = EVENT_MOUSEBUTTONDOWN;
				event.mouse_button.button = MOUSE_BUTTON_R;

				LParamToMouseXY (window,lParam,
								 event.mouse_button.mouse_pos_x,
								 event.mouse_button.mouse_pos_y);
				SetCapture (window);

			} return 0;
			case WM_LBUTTONUP: {
				if (m_mouseRelMode) return 0;
				event.event.type = EVENT_MOUSEBUTTONUP;
				event.mouse_button.button = MOUSE_BUTTON_L;

				LParamToMouseXY (window,lParam,
								 event.mouse_button.mouse_pos_x,
								 event.mouse_button.mouse_pos_y);

				ReleaseCapture ( );
			} return 0;
			case WM_MBUTTONUP: {
				if (m_mouseRelMode) return 0;
				event.event.type = EVENT_MOUSEBUTTONUP;
				event.mouse_button.button = MOUSE_BUTTON_M;

				LParamToMouseXY (window,lParam,
								 event.mouse_button.mouse_pos_x,
								 event.mouse_button.mouse_pos_y);

				ReleaseCapture ( );
			} return 0;
			case WM_RBUTTONUP: {
				if (m_mouseRelMode) return 0;
				event.event.type = EVENT_MOUSEBUTTONUP;
				event.mouse_button.button = MOUSE_BUTTON_R;

				LParamToMouseXY (window,lParam,
								 event.mouse_button.mouse_pos_x,
								 event.mouse_button.mouse_pos_y);

				ReleaseCapture ( );
			} return 0;

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
				} else if (raw->header.dwType==RIM_TYPEMOUSE && m_mouseRelMode) {
					float x = raw->data.mouse.lLastX;
					float y = raw->data.mouse.lLastY;

					if (x==0&&y==0) {
						
						event.mouse_button.mouse_pos_x = 0;
						event.mouse_button.mouse_pos_y = 0;

						if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
							event.event.type = EVENT_MOUSEBUTTONDOWN;
							event.mouse_button.button = MOUSE_BUTTON_L;
							SetCapture (window);
						} else if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
							event.event.type = EVENT_MOUSEBUTTONDOWN;
							event.mouse_button.button = MOUSE_BUTTON_R;
							SetCapture (window);
						} else if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
							event.event.type = EVENT_MOUSEBUTTONUP;
							event.mouse_button.button = MOUSE_BUTTON_R;
							ReleaseCapture ( );
						} else if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
							event.event.type = EVENT_MOUSEBUTTONUP;
							event.mouse_button.button = MOUSE_BUTTON_L;
							ReleaseCapture ( );
						} else if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
							event.event.type = EVENT_MOUSEBUTTONDOWN;
							event.mouse_button.button = MOUSE_BUTTON_M;
							SetCapture (window);
						} else if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
							event.event.type = EVENT_MOUSEBUTTONUP;
							event.mouse_button.button = MOUSE_BUTTON_M;
							ReleaseCapture ( );
						}
					} else {
						event.event.type = EVENT_MOUSEMOVE;

						RECT rect;
						GetWindowRect (window,&rect);
						int width = rect.right-rect.left;
						int height = rect.bottom-rect.top;

						x = x/(width/2.0f);
						y = y/(height/2.0f);

						event.mouse_motion.delta_x = x;
						event.mouse_motion.delta_y = y;
						event.mouse_motion.mouse_pos_x = 0;
						event.mouse_motion.mouse_pos_y = 0;
					}
					return 0;
				}

			}//input

		}//switch on message 

		return -1;
	}

	int WindowsInputSubSystem::handleCommandMsg (const InputCommand msg) {
		switch (msg.type) {
			case INPUTCOMMAND_MOUSERELMODE: {
				setMouseRelMode (msg.relMode.relMode);
			}
		}
		return 0;
	}

	void WindowsInputSubSystem::setMouseRelMode (bool Relmode) {
		RECT rect;
		HWND window = WindowsOSFramework::getWindowsInstance ( )->getWindowHandle ( );
		GetClientRect (window,&rect);

		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;

		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;

		MapWindowPoints (window,NULL,&ul,1);
		MapWindowPoints (window,NULL,&lr,1);

		rect.left = ul.x;
		rect.top = ul.y;

		rect.right = lr.x;
		rect.bottom = lr.y;

		if (Relmode) {
			//Going to Relative Mouse Tracking
			ClipCursor (&rect);
			while(ShowCursor (FALSE)>=0);
		} else {
			//Going to Absolute Mouse Tracking
			ClipCursor (NULL);
			while(ShowCursor (TRUE)<0);
			SetCursorPos (rect.left+(rect.right-rect.left)/2,rect.top+(rect.bottom-rect.top)/2);
			m_ResetMouseTracking = true;
		}

		m_mouseRelMode = Relmode;
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
