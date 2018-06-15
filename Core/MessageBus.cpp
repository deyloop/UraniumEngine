#include <future>

#include "MessageBus.h"
#include "ThreadPool.h"

namespace u92 {
	namespace core {
		MessageBus::MessageBus ( ) {
			m_pThreadPool = new ThreadPool (std::thread::hardware_concurrency ( ));
		}

		MessageBus::~MessageBus ( ) {
			delete m_pThreadPool;
		}

		void MessageBus::syncMessages ( ) {
			//first we clear the message bus.
			clearAllChannels ( );

			//now we get all the messages from the clients.
			for (auto& client:m_clients) {
				for (auto& clientChannel:client->m_chanels) {

					const int cur_index = m_channels[clientChannel.chanelId].store.messages.size ( );
					for (auto& header:clientChannel.messages.headers) {
						header.index += cur_index;
						m_channels[clientChannel.chanelId].store.headers.push_back (header);
					}
					for (auto& byte:clientChannel.messages.messages) {
						m_channels[clientChannel.chanelId].store.messages.push_back (byte);
					}
					clientChannel.messages.headers.clear ( );
					clientChannel.messages.messages.clear ( );
				}
			}
		}

		void MessageBus::proccessMessages ( ) {
			std::vector<std::future<void>> futures;
			for (auto& client:m_clients) {
				futures.emplace_back (m_pThreadPool->enqueue (&MessageBus::processClient,this,client));
			}
			for (auto& future:futures) future.wait ( );
		}

		void MessageBus::registerClient (MessageClient& client) {
			m_clients.push_back (&client);
			for (int i = 0; i<std::thread::hardware_concurrency ( ); i++) {
				m_pThreadPool->enqueue_theadSpecific (i,&MessageClient::threadInit,&client);
			}
		}

		void MessageBus::unregisterClient (MessageClient& client) {

		}

		void MessageBus::processClient (MessageClient* client) {
			for (auto& subId:client->m_subscriptions) {
				client->processMessages (m_channels[subId].store);
			}
		}
	
		void MessageBus::clearAllChannels ( ) {
			for (auto& channel:m_channels) {
				channel.second.store.headers.clear ( );
				channel.second.store.messages.clear ( );
			}
		}
	}
}