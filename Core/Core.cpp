#include "Core.h"
#include "MessageBus.h"

namespace u92 {
	namespace core {
		Core::Core ( ) {
			m_pMessageBus = nullptr;
			m_running = false;
		}
		
		Core::~Core ( ) {
			release ( );
		}

		void Core::init ( ){
			m_pMessageBus = new MessageBus;
			m_pMessageBus->registerClient (*this);
		}

		void Core::release ( ) {
			if (m_pMessageBus) {
				delete m_pMessageBus;
				m_pMessageBus = nullptr;
			}
		}

		void Core::run ( ) {
			m_running = true;
			while (m_running) {
				m_pMessageBus->syncMessages ( );
				m_pMessageBus->proccessMessages ( );
			}
		}


	}
}