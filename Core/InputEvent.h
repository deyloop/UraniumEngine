/******************************************************************************
Project	:	Uranium Engine - An engine for networked games and Simulations

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
#ifndef _INPUT_EVENT_H_
#define _INPUT_EVENT_H_

namespace u92{
	//Types of events.
	typedef enum {
		EVENT_FIRSTEVENT = 0,

		EVENT_QUIT = 0x100,	//App wants to quit.

		//Keyboard Events
		EVENT_KEYBOARD,
		EVENT_KEYDOWN = 0x200,	//Keyboard button pressed down
		EVENT_KEYUP,				//Keyboard button released
		EVENT_KEYPRESS,				//Keyboard button still pressed

		//Mouse Events
		EVENT_MOUSE,
		EVENT_MOUSEMOVE = 0x300,	//Mouse moved position.
		EVENT_MOUSEMOVE_HORIZONTAL_AXIS,
		EVENT_MOUSEMOVE_VERTICAL_AXIS,
		EVENT_MOUSEBUTTONDOWN,		//mouse button was pressed
		EVENT_MOUSEBUTTONUP,		//mouse button was released.
		EVENT_MOUSEWHEEL,			//mouse wheel was rotated.

	} Event_Type;

	//Keycodes for key board events.
	typedef enum Key {
		KEY_ESCAPE = 1,
		//function keys
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		//Arrow Keys
		KEY_ARROW_UP,
		KEY_ARROW_DOWN,
		KEY_ARROW_LEFT,
		KEY_ARROW_RIGHT,
		//some other keys,
		KEY_BACKSPACE,
		KEY_ENTER,
		KEY_TAB,
		KEY_SHIFT,
		KEY_CTRL,
		KEY_ALT,
		KEY_CAPSLOCK,
		KEY_SPACEBAR,
		KEY_PAGEUP,
		KEY_PAGEDOWN,
		KEY_END,
		KEY_HOME,
		KEY_DELETE,
		KEY_CLEAR,
		//Numbers ( top row )
		KEY_0,
		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
		KEY_6, KEY_7, KEY_8, KEY_9,
		//Alphabet keys,
		KEY_A,
		KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
		KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q,
		KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y,
		KEY_Z, KEY_COMMA, KEY_PERIOD,
		//NumPad keys,
		KEY_NUM_0, KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4,
		KEY_NUM_5, KEY_NUM_6, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
		KEY_NUM_DECIMAL, KEY_NUM_MINUS, KEY_NUM_PLUS, KEY_NUM_DIVIDE,
		KEY_NUM_MULTIPLY,

		KEY_QUESTIONMARK,
		KEY_FORWARDSLASH,
		KEY_QUOTE,
		KEY_CURLYBRACE_OPEN,
		KEY_CURLYBRACE_CLOSE,
		KEY_TILDY,
		KEY_MINUS,
		KEY_PLUS,
		KEY_NUMLOCK,
		KEY_SEPARATOR,
		KEY_SCROLLLOCK,
		KEY_NUM_ENTER,
		//Some more wierd keys,'
		KEY_LSHIFT,
		KEY_RSHIFT,
		KEY_LCTRL,
		KEY_RCTRL,
		KEY_LALT,
		KEY_RALT,
		KEY_INSERT,
		KEY_SEMICOLON,
		//...
		KEY_IRRELEVENT = 0
	} key_code;

	enum MouseButton {
		MOUSE_BUTTON_R,
		MOUSE_BUTTON_L,
		MOUSE_BUTTON_M
	};
	//Data common to all events
	typedef struct CommonEventData {
		Event_Type      type;
		unsigned _int64 timestamp;
	}CommonEvent;

	//Data regarding Keybourd events
	typedef struct KeyBoardEvent {
		Event_Type      type;
		unsigned _int64 timestamp;
		Key			    keycode;
		char            KeyName[20];
	} KeyBoardEvent;

	//Data of mouse movement
	typedef struct MouseMotionEvent {
		Event_Type type;
		unsigned _int64 timestamp;
		short mouse_pos_x;
		short mouse_pos_y;
		short delta_x;
		short delta_y;
	} MouseMotionEvent;

	//Data of mouse button event.
	struct MouseButtonEvent {
		Event_Type      type;
		unsigned _int64 timestamp;
		MouseButton     button;
		//...
	};

	//Data of mouse wheel movement.
	struct MouseWheelEvent {
		Event_Type      type;
		unsigned _int64 timestamp;
	};

	//the input data structure.
	union InputEvent {
		Event_Type			type;
		unsigned _int64     timestamp;
		CommonEvent			event;
		KeyBoardEvent		key;
		MouseMotionEvent	mouse_motion;
		MouseButtonEvent	mouse_button;
		MouseWheelEvent		mouse_wheel;
	};
};
#endif
