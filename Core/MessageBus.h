#pragma once
#ifndef _MESSAGE_BUS_H_
#define _MESSAGE_BUS_H_

#include <vector>
#include <map>
#include "MessageClient.h"

namespace u92 {
	namespace core {
		class ThreadPool;
		class MessageClient;
		//class TaskManager;

		struct Channel {
			MessageStore store;
		};

		class MessageBus {
		public:
			MessageBus ( );
			~MessageBus ( );

			void syncMessages ( );
			void proccessMessages ( );

			void registerClient	  (MessageClient& client);
			void unregisterClient (MessageClient& client);

		private:
			void processClient (MessageClient* client);
			void clearAllChannels ( );

			std::map<int,Channel> m_channels;
			std::vector<MessageClient*> m_clients;
			ThreadPool* m_pThreadPool;
			//TaskManager* m_pTaskMAnager;
		};
	}
}

#endif