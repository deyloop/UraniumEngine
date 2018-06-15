#include "TestSystem.h"
#include <OSMessages.h>

#include <Windows.h>

void TestSystem::init (OSFramework* pOS ) {
	m_pOS = pOS;
	m_pOS->initSubSystem (SUBSYSTEM_OPENGL_GRAPHICS);
	registerHandler<WindowEvent> (std::bind (&TestSystem::handleWindowMessage,this,std::placeholders::_1));
	subscribe (2);

	WindowCommand cmd;
	cmd.type = WINDOWCOMMAND_CREATEWINDOW;
	strcpy_s (cmd.command.createWindow.title,"Kuchbhi");
	cmd.command.createWindow.fullscreen = false;
	cmd.command.createWindow.height = 500;
	cmd.command.createWindow.width  = 500;

	postMessage<WindowCommand> (cmd,10);

}

void TestSystem::release ( ) {

}

void TestSystem::threadInit ( ) {
	OutputDebugString ("thread init\n");
}

void TestSystem::handleWindowMessage (const WindowEvent event) {
	switch (event.event.type) {
		case WINDOWEVENTTYPE_CLOSE: {
			WindowCommand cmd;
			cmd.type = WINDOWCOMMAND_DESTROY;
			postMessage<WindowCommand> (cmd,10);
		} break;
		case WINDOWEVENTTYPE_DESTROY: {
			QuitMessage msg = { 0 };
			postMessage<QuitMessage> (msg,10);
		} break;
	}
}

System * getSystemInstance ( ) {
	return new TestSystem;
}
