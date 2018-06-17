#pragma once
#ifndef _INPUT_SYSTEM_H_
#define _INPUT_SYSTEM_H_

#include <System.h>
#include <InputEvent.h>
#include <TickMessage.h>

using namespace u92;

class InputSystem : public System {
public:
	void init (OSFramework* pOS);
	void release ( );

	void threadInit ( );
private:
	void handleInputEvent (const InputEvent event);
	void handleTickMessage (const TickMessage msg);
	OSFramework* m_pOS;

	bool m_keyPressed[256];
};

#ifdef URANIUM_EXPORTS
#define U92_API __declspec(dllexport) 
#else
#define U92_API __declspec(dllimport) 
#endif

extern "C" {
	U92_API System* getSystemInstance ( ) { 
		return new InputSystem;
	};
}
#endif