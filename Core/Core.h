#pragma once
#ifndef _CORE_H_
#define	_CORE_H_

#include "MessageClient.h"

namespace u92 {
	namespace core {
		
		class MessageBUs;

		class Core : public MessageClient {
		public:
			Core ( );
			~Core ( );

			void init ( );
			void release ( );
			
			void run ( );

		private:
			bool		m_running;
			MessageBus* m_pMessageBus;
		};
	}
}

#endif 
