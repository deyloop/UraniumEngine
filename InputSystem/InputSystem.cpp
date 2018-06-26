#include "InputSystem.h"
#include "DataFile.h"
#include <UserInputEvent.h>

void InputSystem::init (OSFramework * pOS) {
	pOS->initSubSystem (SUBSYSTEM_INPUT);
	registerHandler<InputEvent>(std::bind (&InputSystem::handleInputEvent,this,std::placeholders::_1));
	registerHandler<TickMessage> (std::bind (&InputSystem::handleTickMessage,this,std::placeholders::_1));
	registerHandler<LoadContextFile> (std::bind (&InputSystem::handleLoadContextFile,this,std::placeholders::_1));
	registerHandler<ActivateContext> (std::bind (&InputSystem::handleActivateContext,this,std::placeholders::_1));
	registerHandler<DeactivateContext> (std::bind (&InputSystem::handleDeactivateContext,this,std::placeholders::_1));
	subscribe (7);
	subscribe (1);
	subscribe (0);

	for (auto& key:m_keyPressed) key = false;
}

void InputSystem::release ( ) {

}

void InputSystem::threadInit ( ) {

}

#include <string>
#include <sstream>
#include <Windows.h>

void InputSystem::handleInputEvent (const InputEvent event) {
	std::string out;
	switch (event.type) {
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

	handleInput (event);
}

void InputSystem::handleTickMessage (const TickMessage msg) { 
	//handle keyPressed messages here.

	for (int i = 0; i<256; i++) {
		if (m_keyPressed[i]) {
			InputEvent e;
			e.key.type	  = EVENT_KEYPRESS;
			e.key.keycode = (Key)i;
			strcpy_s (e.key.KeyName,"KEY");
			handleInput (e);
		}
	}

	/*//temporary
	if (m_keyPressed[KEY_W])
		postMessage<UserInputEvent> ({ "move-forward",0,0,0,0 },5);
	if (m_keyPressed[KEY_S])
		postMessage<UserInputEvent> ({ "move-back",0,0,0,0 },5);
	if (m_keyPressed[KEY_A])
		postMessage<UserInputEvent> ({ "move-left",0,0,0,0 },5);
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
	//end temp*/
}

constexpr unsigned int str2int (const char* str,int h = 0) {
	return !str[h] ? 5381 : (str2int (str,h+1)*33)^str[h];
}

Event_Type atoEvent_type (const char* str) {
	switch (str2int (str)) {
		case str2int ("EVENT_KEYDOWN"):	return EVENT_KEYDOWN;
		case str2int ("EVENT_KEYUP"):	return EVENT_KEYUP;
		case str2int ("EVENT_QUIT"):		return EVENT_QUIT;
		case str2int ("EVENT_KEYBOARD"): return EVENT_KEYBOARD;
		case str2int ("EVENT_MOUSE"):	return EVENT_MOUSE;
		case str2int ("EVENT_MOUSEMOVE"):return EVENT_MOUSEMOVE;
		case str2int ("EVENT_KEYPRESS"): return EVENT_KEYPRESS;
	}
	//keep adding all here.
}

Key atoKeyCode (const char* str) {

	switch (str2int (str)) {
		case str2int ("KEY_ARROW_UP"):	return KEY_ARROW_UP;
		case str2int ("KEY_ARROW_DOWN"):	return KEY_ARROW_DOWN;
		case str2int ("KEY_ARROW_LEFT"):	return KEY_ARROW_LEFT;
		case str2int ("KEY_ARROW_RIGHT"):	return KEY_ARROW_RIGHT;
		case str2int ("KEY_A"):	return KEY_A;
		case str2int ("KEY_W"):	return KEY_W;
		case str2int ("KEY_D"):	return KEY_D;
		case str2int ("KEY_S"):	return KEY_S;
		case str2int ("KEY_R"):	return KEY_R;
		case str2int ("KEY_E"):	return KEY_E;
		case str2int ("KEY_Q"):	return KEY_Q;
		case str2int ("KEY_Z"):	return KEY_Z;
		case str2int ("KEY_X"):	return KEY_X;
		case str2int ("KEY_L"):	return KEY_L;
		case str2int ("KEY_NUM_8"):	return KEY_NUM_8;
		case str2int ("KEY_NUM_2"):	return KEY_NUM_2;
		case str2int ("KEY_NUM_4"):	return KEY_NUM_4;
		case str2int ("KEY_NUM_6"):	return KEY_NUM_6;
		case str2int ("KEY_NUM_7"):	return KEY_NUM_7;
		case str2int ("KEY_ESCAPE"):	return KEY_ESCAPE;
		case str2int ("KEY_SPACEBAR"):	return KEY_SPACEBAR;
		case str2int ("KEY_LSHIFT"):	return KEY_LSHIFT;
	}
	//keep adding stuff
	return KEY_IRRELEVENT;
}

void InputSystem::handleLoadContextFile (const LoadContextFile msg) {
	DataFile context_file;
	context_file.parse (msg.filename);
	for (auto& node:context_file.RootNode.subNodes) {
		if (node.name=="context") {
			InputContext context;
			context.m_name = node.GetAttribute ("name");
			for (auto& event_node:node.subNodes) {
				if (event_node.name=="InputEvent") {
					InputEvent event;
					event.type = atoEvent_type (event_node.GetAttribute ("type"));
					switch (event.type) {
						case EVENT_KEYBOARD:
						case EVENT_KEYUP:
						case EVENT_KEYDOWN:
						case EVENT_KEYPRESS: {
							event.key.keycode = atoKeyCode (event_node.GetAttribute ("keycode"));
						}break;
					}//end of switch on event type.

					 //after all that event neusence,
					InputPair pair (event,event_node.GetAttribute ("name"));
					context.m_pairs.push_back (pair);
				}//if name is InputEvent
			}//end loop on events
			AddContext (context);
		}
	}
}

void InputSystem::handleActivateContext (const ActivateContext msg) {

	const int num_contexts = m_contexts.size ( );

	for (int i = 0; i < num_contexts; ++i) {
		if (m_contexts[i].m_name==msg.contextName) {
			//Add it to active list.
			m_activeContexts.push_back (m_contexts[i]);
			//remove from main list;
			m_contexts[i] = m_contexts.back ( );
			m_contexts.pop_back ( );
			return;
		}
	}
}

void InputSystem::handleDeactivateContext (const DeactivateContext msg) {
	const int num_contexts = m_activeContexts.size ( );

	for (int i = 0; i < num_contexts; ++i) {
		if (m_activeContexts[i].m_name==msg.contextName) {
			//Add it to main list.
			m_contexts.push_back (m_activeContexts[i]);
			//remove from active list;
			m_activeContexts[i] = m_activeContexts.back ( );
			m_activeContexts.pop_back ( );
			return;
		}
	}
}

void InputSystem::AddContext (InputContext context) {
	m_contexts.push_back (context);
}

void InputSystem::handleInput (const InputEvent & p_given_event) {
	if (m_activeContexts.size ( )==0) return;	//gotta do nothing 

	for (auto& context:m_activeContexts) {	//for each context,

		for (auto& pair:context.m_pairs) {	//...for each pair...

			if (p_given_event.event.type==pair.Desired_event.event.type) {

				switch (p_given_event.event.type) {

					case EVENT_KEYDOWN:
					case EVENT_KEYPRESS: {
						//check if keycode is relevent,
						if (pair.Desired_event.key.keycode==KEY_IRRELEVENT) {
							//just go forth with it
							UserInputEvent e;
							e.event = pair.event_name;
							e.dx = e.dy = e.x = e.y = 0;
							postMessage<UserInputEvent> (e,5);

						} else if (pair.Desired_event.key.keycode==p_given_event.key.keycode) {
							UserInputEvent e;
							e.event = pair.event_name;
							e.dx = e.dy = e.x = e.y = 0;
							postMessage<UserInputEvent> (e,5);
						}
					}break;

					case EVENT_MOUSEMOVE: {
						//just go forth with it
						UserInputEvent e;
						e.event = pair.event_name;
						e.dx = p_given_event.mouse_motion.delta_x;
						e.dy = p_given_event.mouse_motion.delta_y;
						e.x = e.y = 0;
						postMessage<UserInputEvent> (e,5);
					}break;
					case EVENT_MOUSEBUTTONDOWN:
					case EVENT_MOUSEBUTTONUP: {
						if (pair.Desired_event.mouse_button.button==p_given_event.mouse_button.button) {
							UserInputEvent e;
							e.event = pair.event_name;
							e.dx = e.dy = e.x = e.y = 0;
							postMessage<UserInputEvent> (e,5);
						}
					}break;
					default:
						continue;	//get on with the next pair.
				} //end the switch.

			}//the if statement which checks if event types match
		}//next pair in loop
	}//next context.
	return;
}

