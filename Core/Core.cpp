#include "Core.h"
#include "MessageBus.h"
#include "OSFramework.h"
#include "OSMessages.h"
#include <cstring>

namespace u92 {
	namespace core {
		Core::Core ( ) {
			m_pMessageBus  = nullptr;
			m_pOSFramework = nullptr;
			m_running = false;
		}
		
		Core::~Core ( ) {
			release ( );
		}

		ErrorCode Core::init ( ){
			m_pMessageBus = new MessageBus;
			m_pMessageBus->registerClient (*this);

			m_pOSFramework = OSFramework::getInstance ( );
			m_pMessageBus->registerClient (*m_pOSFramework);

			return E_CODE_SUCCESS;
		}

		void Core::release ( ) {
			if (m_pMessageBus) {
				delete m_pMessageBus;
				m_pMessageBus = nullptr;
			}
		}

		void Core::run ( ) {

			m_pOSFramework->initSubSystem (SUBSYSTEM_OPENGL_GRAPHICS);
			registerHandler<WindowEvent> (std::bind (&Core::ha,this,std::placeholders::_1));
			subscribe (2);
			//temporary code
			WindowCommand cmd;
			cmd.type = WINDOWCOMMAND_CREATEWINDOW;
			strcpy_s(cmd.command.createWindow.title,"Kuchbhi");
			cmd.command.createWindow.fullscreen = false;
			cmd.command.createWindow.height = 500;
			cmd.command.createWindow.width = 500;

			postMessage<WindowCommand> (cmd,10);
			//end temporary

			m_running = true;
			while (m_running) {
				int result = m_pOSFramework->handleOSMessages ( );
				if (result==E_CODE_QUIT_MESSAGE) {
					m_running = false;
					break;
				}
				m_pMessageBus->syncMessages ( );
				m_pMessageBus->proccessMessages ( );
			}
		}


	}
}