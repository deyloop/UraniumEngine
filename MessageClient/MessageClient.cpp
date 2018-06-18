#include "MessageClient.h"

namespace u92 {
	namespace core {
		void MessageClient::subscribe(int channelId) {
			//first we check if we have already subscribed.
			for(auto& sub : m_subscriptions)
				if (sub==channelId) 
					return;
				
			m_subscriptions.push_back(channelId);
		}

		void MessageClient::unsubscribe (int channelId) {
			const auto size = m_subscriptions.size ( );
			for (int i = 0; i<size; i++) {
				if (m_subscriptions[i]==channelId) {
					m_subscriptions.erase (m_subscriptions.begin ( )+i);
					break;
				}
			}
		}

		void MessageClient::processMessages (const MessageStore& messages) {
			for (auto& header:messages.headers) {
				//check if we handle such a type of message
				if (m_handlers.find (header.type)==m_handlers.end ( )) {
					//not found
					continue;
				} else {
					//process this thing.
					(m_handlers[header.type])((const void*)(&messages.messages[header.index]));
				}
			}

		}

	}
}