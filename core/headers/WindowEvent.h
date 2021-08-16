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
#ifndef _WINDOW_EVENT_H_
#define _WINDOW_EVENT_H_

namespace u92 {
	enum WindowEventType {
		WINDOWEVENTTYPE_NONE = 0,
		WINDOWEVENTTYPE_CREATE,
		WINDOWEVENTTYPE_PAINT,
		WINDOWEVENTTYPE_RESIZE,
		WINDOWEVENTTYPE_MINIMIZE,
		WINDOWEVENTTYPE_MAXIMIZE,
		WINDOWEVENTTYPE_KILLFOCUS,
		WINDOWEVENTTYPE_CLOSE,
		WINDOWEVENTTYPE_DESTROY
	};
	
	struct WindowEventCommon {
		WindowEventType type;
	};
	struct WindowResizeEvent {
		WindowEventType type;
		int newWidth, newHieght;
	};
	
	union WindowEvent {
		WindowEventCommon event;
		WindowResizeEvent resize;
	};
}

#endif
