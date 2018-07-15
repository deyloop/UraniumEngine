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
#ifndef _WINDOWS_INPUT_SUBSYSTEM_H_
#define _WINDOWS_INPUT_SUBSYSTEM_H_

#include "OSInputSubSystem.h"
#include "OSMessages.h"
#include <Windows.h>

namespace u92 {
	class WindowsInputSubSystem : public OSInputSubSystem {
	public:
		WindowsInputSubSystem();
		~WindowsInputSubSystem();

		int init();
		int release();

		int convertMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, InputEvent& event);
		int handleCommandMsg(const InputCommand msg);

	private:
		bool m_ResetMouseTracking;
		bool m_Init;
		bool m_mouseRelMode;

		int  EvaluateKeyBaordInput(const RAWKEYBOARD& rawKB, InputEvent* pEvent);
		void setMouseRelMode (bool Relmode);
	};
}

#endif