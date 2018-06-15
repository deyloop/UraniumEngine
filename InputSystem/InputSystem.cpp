#include "InputSystem.h"
#include <Windows.h>

void InputSystem::init (OSFramework * pOS) {
	pOS->initSubSystem (SUBSYSTEM_INPUT);
	registerHandler<InputEvent>(std::bind (&InputSystem::handleInputEvent,this,std::placeholders::_1));
	subscribe (1);

}

void InputSystem::release ( ) {

}

void InputSystem::threadInit ( ) {

}

#include <string>

void InputSystem::handleInputEvent (const InputEvent event) {
	std::string out;
	if (event.type==EVENT_MOUSEMOVE) {
		//OutputDebugString ("MOUSE\n");
	} else if (event.type==EVENT_KEYDOWN) {
		out = event.key.KeyName;
		out += "\n";
		OutputDebugString (out.c_str());
	}
}
