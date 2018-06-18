#include "InputSystem.h"
#include <UserInputEvent.h>

void InputSystem::init (OSFramework * pOS) {
	pOS->initSubSystem (SUBSYSTEM_INPUT);
	registerHandler<InputEvent>(std::bind (&InputSystem::handleInputEvent,this,std::placeholders::_1));
	registerHandler<TickMessage> (std::bind (&InputSystem::handleTickMessage,this,std::placeholders::_1));
	subscribe (1);
	subscribe (0);

	for (auto& key:m_keyPressed) key = false;
}

void InputSystem::release ( ) {

}

void InputSystem::threadInit ( ) {

}

#include <string>

void InputSystem::handleInputEvent (const InputEvent event) {
	std::string out;
	switch (event.type) {
		case EVENT_MOUSEMOVE: {
			//OutputDebugString ("MOUSE\n");
		} break;
		case EVENT_KEYDOWN: {
			if (m_keyPressed[event.key.keycode]) {
				//ignore.
				return;
			} else {
				m_keyPressed[event.key.keycode] = true;
			}
		} break;
		case EVENT_KEYUP: {
			m_keyPressed[event.key.keycode] = false;
		}
	}
}

void InputSystem::handleTickMessage (const TickMessage msg) { 
	//handle keyPressed messages here.

	//temporary
	if (m_keyPressed[KEY_W])
		postMessage<UserInputEvent> ({ "move-forward",0,0,0,0 },5);
	if (m_keyPressed[KEY_S])
		postMessage<UserInputEvent> ({ "move-back",0,0,0,0 },5);
	if (m_keyPressed[KEY_A])
		postMessage<UserInputEvent> ({ "move-left",0,0,0,0 },5);
	if (m_keyPressed[KEY_D])
		postMessage<UserInputEvent> ({ "move-right",0,0,0,0 },5);
	if (m_keyPressed[KEY_D])
		postMessage<UserInputEvent> ({ "move-right",0,0,0,0 },5);
	if (m_keyPressed[KEY_SPACEBAR])
		postMessage<UserInputEvent> ({ "move-up",0,0,0,0 },5);
	if (m_keyPressed[KEY_LSHIFT])
		postMessage<UserInputEvent> ({ "move-down",0,0,0,0 },5);
	if (m_keyPressed[KEY_ARROW_RIGHT])
		postMessage<UserInputEvent> ({ "rotate-right",0,0,0,0 },5);
	if (m_keyPressed[KEY_ARROW_LEFT])
		postMessage<UserInputEvent> ({ "rotate-left",0,0,0,0 },5);
	if (m_keyPressed[KEY_ARROW_UP])
		postMessage<UserInputEvent> ({ "rotate-up",0,0,0,0 },5);
	if (m_keyPressed[KEY_ARROW_DOWN])
		postMessage<UserInputEvent> ({ "rotate-down",0,0,0,0 },5);
	//end temp
}
