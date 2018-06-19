#pragma once
#ifndef _CORE_H_
#define	_CORE_H_

#include "MessageClient.h"
#include "ErrorCode.h"

namespace u92 {

	class OSFramework;

	namespace core {
		
		class MessageBus;
		
		class Core : public MessageClient {
		public:
			Core ( );
			~Core ( );

			ErrorCode	init ( );
			void		release ( );
			
			void run ( );

			void threadInit ( ) { }

		private:
			bool			m_running;
			
			void LoadSystems ( );
			void initSystem (const char* systemModuleName);

			MessageBus*		m_pMessageBus;
			OSFramework*	m_pOSFramework;
		};
	}
}

#endif 
