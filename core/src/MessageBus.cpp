#include <future>

#include "MessageBus.h"
#include "ThreadPool.h"
//#include "TaskManager.h"

#include <list>
namespace u92 {
	namespace core {
		MessageBus::MessageBus ( ) {
			m_pThreadPool = new ThreadPool (std::thread::hardware_concurrency ( ));
			//m_pTaskMAnager = new TaskManager();
		}

		MessageBus::~MessageBus ( ) {
			delete m_pThreadPool;
			//delete m_pTaskMAnager;
		}

		void MessageBus::syncMessages ( ) {
			//first we clear the message bus.
			clearAllChannels ( );

			//now we get all the messages from the clients.
			for (auto& client:m_clients) {
				for (auto& clientChannel:client->m_chanels) {

					const int cur_index = m_channels[clientChannel.chanelId].store[client].messages.size ( );
					for (auto& header:clientChannel.messages.headers) {
						header.index += cur_index;
						m_channels[clientChannel.chanelId].store[client].headers.push_back (header);
					}
					for (auto& byte:clientChannel.messages.messages) {
						m_channels[clientChannel.chanelId].store[client].messages.push_back (byte);
					}
					clientChannel.messages.headers.clear ( );
					clientChannel.messages.messages.clear ( );
				}
			}
		}

		void MessageBus::proccessMessages ( ) {
			std::vector<std::future<void>> futures;
			for (auto& client:m_clients) {
				if (client->m_threadAffinity&&client->m_threadAffinityID!=-1) {
					futures.emplace_back (m_pThreadPool->enqueue_theadSpecific (client->m_threadAffinityID,
																				&MessageBus::processClient,
																				this,
																				client)
					);
				} else {
					futures.emplace_back (m_pThreadPool->enqueue (&MessageBus::processClient,this,client));

				}
			}

			for (auto& future:futures) future.wait ( );
		}

		void MessageBus::registerClient (MessageClient& client) {
			m_clients.push_back (&client);
			if (client.m_threadAffinity) {
				static int affinThreadId = std::thread::hardware_concurrency ( )-1;
				client.m_threadAffinityID = affinThreadId;
				m_pThreadPool->enqueue_theadSpecific (client.m_threadAffinityID,&MessageClient::threadInit,&client);
				if (affinThreadId==0) {
					affinThreadId = std::thread::hardware_concurrency ( )-1;
				} else affinThreadId--;
			} else {
				for (int i = 0; i<std::thread::hardware_concurrency ( ); i++) {
					m_pThreadPool->enqueue_theadSpecific (i,&MessageClient::threadInit,&client);
				}
			}
		}

		void MessageBus::unregisterClient (MessageClient& client) {

		}

		void MessageBus::processClient (MessageClient* client){
			//get iterators to the channels that are subscribed
			for (auto& subId:client->m_subscriptions) {
				for (auto& a:m_channels[subId].store) {
					if(a.first != client)
						client->processMessages (a.second);
				}
			}

		}
	
		void MessageBus::clearAllChannels ( ) {
			for (auto& channel:m_channels) {
				channel.second.store.clear ( );
			}
		}
	}
}