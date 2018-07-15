#include "InputSystem.h"
#include "DataFile.h"

#include <UserInputEvent.h>
#include <OSInputSubSystem.h>
#include <OSMessages.h>

void InputSystem::init (OSFramework * pOS) {
	pOS->initSubSystem (SUBSYSTEM_INPUT);
	m_pOS = pOS;
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

void InputSystem::handleInputEvent (const InputEvent event) {
	static bool b = false;
	std::string out;
	switch (event.type) {
		case EVENT_KEYDOWN: {
			if (m_keyPressed[event.key.keycode]) {
				//ignore.
				return;
			} else {
				m_keyPressed[event.key.keycode] = true;
			}
			if (event.key.keycode==KEY_E) {
				InputCommand cmd;
				cmd.type = INPUTCOMMAND_MOUSERELMODE;
				cmd.relMode.relMode = b;
				postMessage<InputCommand> (cmd,10);
				b = !b;
			}
		} break;
		case EVENT_KEYUP: {
			m_keyPressed[event.key.keycode] = false;
		} break;
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
}

constexpr unsigned int str2int (const char* str,int h = 0) {
	return !str[h] ? 5381 : (str2int (str,h+1)*33)^str[h];
}

constexpr const Event_Type atoEvent_type (const char* str) {
	//keep close attention to this macro. 
	//It was created to make writing code easier.
	#define chooseEvent(event) case str2int(#event) : return event
	
	switch (str2int (str)) {
		chooseEvent (EVENT_KEYDOWN);
		chooseEvent (EVENT_KEYUP);
		chooseEvent (EVENT_KEYBOARD);
		chooseEvent (EVENT_MOUSE);
		chooseEvent (EVENT_MOUSEMOVE);
		chooseEvent (EVENT_KEYPRESS);
		chooseEvent (EVENT_MOUSEBUTTONDOWN);
		chooseEvent (EVENT_MOUSEBUTTONUP);
		chooseEvent (EVENT_MOUSEWHEEL);
		chooseEvent (EVENT_FIRSTEVENT);
	}
	return EVENT_UNKOWN;
}

constexpr const Key atoKeyCode (const char* str) {

	//macro helps in writting the switch case easier.
	#define chooseKey(key) case str2int(#key): return key

	switch (str2int (str)) {
		chooseKey (KEY_ARROW_UP);
		chooseKey (KEY_ARROW_DOWN);
		chooseKey (KEY_ARROW_LEFT);
		chooseKey (KEY_ARROW_RIGHT);
		chooseKey (KEY_A);
		chooseKey (KEY_B);
		chooseKey (KEY_C);
		chooseKey (KEY_D);
		chooseKey (KEY_E);
		chooseKey (KEY_F);
		chooseKey (KEY_G);
		chooseKey (KEY_H);
		chooseKey (KEY_I);
		chooseKey (KEY_J);
		chooseKey (KEY_K);
		chooseKey (KEY_L);
		chooseKey (KEY_M);
		chooseKey (KEY_N);
		chooseKey (KEY_O);
		chooseKey (KEY_P);
		chooseKey (KEY_Q);
		chooseKey (KEY_R);
		chooseKey (KEY_S);
		chooseKey (KEY_T);
		chooseKey (KEY_U);
		chooseKey (KEY_V);
		chooseKey (KEY_W);
		chooseKey (KEY_X);
		chooseKey (KEY_Y);
		chooseKey (KEY_Z);
		chooseKey (KEY_F1);
		chooseKey (KEY_F2);
		chooseKey (KEY_F3);
		chooseKey (KEY_F4);
		chooseKey (KEY_F5);
		chooseKey (KEY_F6);
		chooseKey (KEY_F7);
		chooseKey (KEY_F8);
		chooseKey (KEY_F9);
		chooseKey (KEY_F10);
		chooseKey (KEY_F11);
		chooseKey (KEY_F12);
		chooseKey (KEY_BACKSPACE);
		chooseKey (KEY_ENTER);
		chooseKey (KEY_TAB);
		chooseKey (KEY_SHIFT);
		chooseKey (KEY_CTRL);
		chooseKey (KEY_ALT);
		chooseKey (KEY_CAPSLOCK);
		chooseKey (KEY_PAGEUP);
		chooseKey (KEY_PAGEDOWN);
		chooseKey (KEY_ESCAPE);
		chooseKey (KEY_SPACEBAR);
		chooseKey (KEY_END);
		chooseKey (KEY_HOME);
		chooseKey (KEY_DELETE);
		chooseKey (KEY_CLEAR);
		chooseKey (KEY_0);
		chooseKey (KEY_1);
		chooseKey (KEY_2);
		chooseKey (KEY_3);
		chooseKey (KEY_4);
		chooseKey (KEY_5);
		chooseKey (KEY_6);
		chooseKey (KEY_7);
		chooseKey (KEY_8);
		chooseKey (KEY_9);
		chooseKey (KEY_COMMA);
		chooseKey (KEY_PERIOD);
		chooseKey (KEY_NUM_0);
		chooseKey (KEY_NUM_1);
		chooseKey (KEY_NUM_2);
		chooseKey (KEY_NUM_3);
		chooseKey (KEY_NUM_4);
		chooseKey (KEY_NUM_5);
		chooseKey (KEY_NUM_6);
		chooseKey (KEY_NUM_7);
		chooseKey (KEY_NUM_8);
		chooseKey (KEY_NUM_9);
		chooseKey (KEY_NUM_DECIMAL);
		chooseKey (KEY_NUM_MINUS);
		chooseKey (KEY_NUM_PLUS);
		chooseKey (KEY_NUM_DIVIDE);
		chooseKey (KEY_NUM_MULTIPLY);
		chooseKey (KEY_QUESTIONMARK);
		chooseKey (KEY_FORWARDSLASH);
		chooseKey (KEY_QUOTE);
		chooseKey (KEY_CURLYBRACE_OPEN);
		chooseKey (KEY_CURLYBRACE_CLOSE);
		chooseKey (KEY_TILDY);
		chooseKey (KEY_MINUS);
		chooseKey (KEY_PLUS);
		chooseKey (KEY_NUMLOCK);
		chooseKey (KEY_SEPARATOR);
		chooseKey (KEY_SCROLLLOCK);
		chooseKey (KEY_NUM_ENTER);
		chooseKey (KEY_LSHIFT);
		chooseKey (KEY_RSHIFT);
		chooseKey (KEY_LCTRL);
		chooseKey (KEY_RCTRL);
		chooseKey (KEY_LALT);
		chooseKey (KEY_RALT);
		chooseKey (KEY_INSERT);
		chooseKey (KEY_SEMICOLON);
	}
	return KEY_IRRELEVENT;
}

constexpr const MouseButton atoMouseButton (const char* str) {
	switch (str2int (str)) {
		case str2int ("MOUSE_BUTTON_L"):return MOUSE_BUTTON_L;
		case str2int ("MOUSE_BUTTON_M"):return MOUSE_BUTTON_M;
		case str2int ("MOUSE_BUTTON_R"):return MOUSE_BUTTON_R;
	}
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
						case EVENT_MOUSEBUTTONDOWN: {
							event.mouse_button.button = atoMouseButton (event_node.GetAttribute ("button"));
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

	const unsigned int num_contexts = m_contexts.size ( );

	//Does a search throgh all the inactive contexts and finds the 
	//required context.
	for (int i = 0; i < num_contexts; ++i) {
		if (m_contexts[i].m_name==msg.contextName) { //found it
			//Add it to active list.
			m_activeContexts.push_back (m_contexts[i]);
			//remove from main list;
			m_contexts[i] = m_contexts.back ( );
			m_contexts.pop_back ( );
			return;
		}
	}

	//reaching this point means either the required context is already active
	//or does not exist.
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

