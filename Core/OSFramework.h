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
#ifndef _OS_FRAMEWORK_H_
#define _OS_FRAMEWORK_H_

#include "MessageClient.h"

namespace u92{
	enum SubSystemType {
		SUBSYSTEM_ALL,
		SUBSYSTEM_OPENGL_GRAPHICS,
		SUBSYSTEM_INPUT
	};

	class OSGLGraphicsSubSystem;
	class OSInputSubSystem;
	class System;

	class OSFramework : public core::MessageClient {
	public:
		virtual int handleOSMessages() = 0;
		virtual int initSubSystem	 (SubSystemType subSystemType) = 0;
		virtual int releaseSubSystem (SubSystemType subSystemType) = 0;

		virtual OSGLGraphicsSubSystem*	getOpenGLGraphicsSubSystem ( ) = 0;
		virtual OSInputSubSystem*  getInputSubSystem ( )	= 0;

		virtual System* loadSystemModule(const char* moduleName)		= 0;

		static OSFramework* getInstance();
	};
}

#endif