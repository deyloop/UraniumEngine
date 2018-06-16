#include "Core.h"
#include "MessageBus.h"
#include "OSFramework.h"
#include "OSMessages.h"
#include <cstring>
#include "System.h"
#include "TickMessage.h"

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

			System* pTest = m_pOSFramework->loadSystemModule ("TestSystem.dll");
			pTest->init (m_pOSFramework);
			m_pMessageBus->registerClient (*pTest);
			
			System* pInputSystem = m_pOSFramework->loadSystemModule ("InputSystem.dll");
			pInputSystem->init (m_pOSFramework);
			m_pMessageBus->registerClient (*pInputSystem);

			return E_CODE_SUCCESS;
		}

		void Core::release ( ) {
			if (m_pMessageBus) {
				delete m_pMessageBus;
				m_pMessageBus = nullptr;
			}
		}

		void Core::run ( ) {

			m_running = true;
			unsigned long long int tick = 0;
			while (m_running) {
				postMessage<TickMessage> ({ tick },0);
				int result = m_pOSFramework->handleOSMessages ( );
				if (result==E_CODE_QUIT_MESSAGE) {
					m_running = false;
					break;
				}
				m_pMessageBus->syncMessages ( );
				m_pMessageBus->proccessMessages ( );
				
				tick++;
			}
		}


	}
}