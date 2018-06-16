#include "TestSystem.h"
#include <OSMessages.h>

#include <Windows.h>
#include <OSGLGraphicsSubSystem.h>

void TestSystem::init (OSFramework* pOS ) {
	m_pOS = pOS;
	m_pOS->initSubSystem (SUBSYSTEM_OPENGL_GRAPHICS);
	registerHandler<WindowEvent> (std::bind (&TestSystem::handleWindowMessage,this,std::placeholders::_1));
	subscribe (2);

	(pOS->getOpenGLGraphicsSubSystem())->createWindow("kuchbhi",500,500);

	setthreadAfinity (true);
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
