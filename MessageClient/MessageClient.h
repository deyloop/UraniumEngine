#pragma once
#ifndef _MESSAGE_CLIENT_H_
#define	_MESSAGE_CLIENT_H_

#include <vector>
#include <map>
#include <functional>
#include <chrono>

namespace u92 {
	namespace core {
		struct MessageHeader {
			size_t type;
			unsigned int index;
		};

		struct MessageStore {
			std::vector<MessageHeader> headers;
			std::vector<char> messages;
		};

		struct ClientChannel {
			int chanelId;
			MessageStore messages;
		};

		class MessageClient {
			friend class MessageBus;
		public:
			MessageClient ( ) {
				m_threadAffinity = false;
				m_threadAffinityID = -1;
			}

		protected:
			template <typename Message>
			void postMessage(const Message message, int channelId = 0) {
				//first, get the channel
				ClientChannel* destChannel = nullptr;
				for (auto& channel : m_chanels) {
					if (channel.chanelId == channelId) {
						destChannel = &channel;
						break;
					}
				}

				if (destChannel == nullptr) {
					//add channel to the client channels list.
					ClientChannel newChanel;
					newChanel.chanelId = channelId;
					m_chanels.push_back(newChanel);
					destChannel = &m_chanels.back();
				}

				MessageHeader header;
				header.type = typeid(message).hash_code();
				header.index = destChannel->messages.messages.size();
				for (int i = 0; i < sizeof(Message); i++) destChannel->messages.messages.push_back('\0');
				memcpy_s(&destChannel->messages.messages[header.index], sizeof(Message), &message, sizeof(message));
				destChannel->messages.headers.push_back(header);
			}

			template <typename Message>
			void registerHandler(std::function<void(const Message)> handler) {
				m_handlers[typeid(Message).hash_code()] = [handler](const void* msg) {
					handler(*((const Message*)msg));
				};
			}

			void subscribe(int channelId);
			void unsubscribe(int channelId);

			virtual void threadInit ( ) = 0;
			void setthreadAfinity (bool val) {
				m_threadAffinity = val;
			}

		private:
			void processMessages (const MessageStore& messages);

			std::vector<ClientChannel>	m_chanels;
			std::vector<int>			m_subscriptions;
			std::map<size_t, std::function<void(const void*)>> m_handlers;

			int m_threadAffinityID;
			bool m_threadAffinity;
		};
	}
}
#endif
