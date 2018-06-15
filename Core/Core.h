#pragma once
#ifndef _CORE_H_
#define	_CORE_H_

#include "MessageClient.h"
#include "ErrorCode.h"
#include "WindowEvent.h"
#include "OSMessages.h"

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

		private:
			void ha (const WindowEvent e) {
				if (e.event.type==WINDOWEVENTTYPE_CLOSE) {
					WindowCommand cmd;
					cmd.type = WINDOWCOMMAND_DESTROY;
					postMessage<WindowCommand> (cmd,10);
				} else if (e.event.type==WINDOWEVENTTYPE_DESTROY) {
					QuitMessage msg = { 0 };
					postMessage<QuitMessage> (msg,10);
				}
			}
			bool			m_running;

			MessageBus*		m_pMessageBus;
			OSFramework*	m_pOSFramework;
		};
	}
}

#endif 
