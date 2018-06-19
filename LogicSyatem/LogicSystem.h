#pragma once
#ifndef _LOGIC_SYSTEM_H_
#define _LOGIC_SYSTEM_H_

#include <System.h>
#include <UserInputEvent.h>
#include <TickMessage.h>
#include "Transform.h"

using namespace u92;

class LogicSystem : public System {
public:
	void init (OSFramework* pOS);
	void release ( );

	void threadInit ( );
private:
	void handleUserInput (const UserInputEvent event);
	void tick (const TickMessage msg);

	Transform m_camtrans;
};

#ifdef URANIUM_EXPORTS
#define U92_API __declspec(dllexport) 
#else
#define U92_API __declspec(dllimport) 
#endif

extern "C" {
	U92_API System* getSystemInstance ( ) {
		return new LogicSystem;
	};
}
#endif