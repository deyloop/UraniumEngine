#include <vector>
#include <map>
#include <functional>
#include <type_traits>

struct MessageHeader {
	size_t type;
	unsigned int index;
};

struct MessageStore {
	std::vector<MessageHeader> headers;
	std::vector<char> messages;
};
struct Channel {
	MessageStore store;
	std::vector<unsigned int> subscribers;
};

struct ClientChannel {
	int chanelId;
	MessageStore messages;
};

class MessageClient {
	friend class MessageBus;
public:
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

	void subscribe(int channelId) {
		m_subscriptions.push_back(channelId);
	}

private:
	void processMessages(const MessageStore& messages) {
		if (messages.headers.size() == 0) return;
		for (auto& header : messages.headers) {
			//check if we handle such a type of message
			if (m_handlers.find(header.type) == m_handlers.end()) {
				//not found
				continue;
			}
			else {
				//process this thing.
				(m_handlers[header.type])((const void*)(&messages.messages[header.index]));
			}
		}

	}

	std::vector<ClientChannel> m_chanels;
	std::vector<int> m_subscriptions;
	std::map<size_t, std::function<void(const void*)>> m_handlers;
};

#include "ThreadPool.h"

class MessageBus {
public:
	MessageBus() : m_threadPool(std::thread::hardware_concurrency()) {

	}
	
	void syncMessages() {
		//first we clear the message bus.
		clearAllChannels();

		//now we get all the messages from the clients.
		for (auto& client : m_clients) {
			for (auto& clientChannel : client->m_chanels) {

				while (m_channels.size() <= clientChannel.chanelId) {
					Channel newChannel;
					m_channels.push_back(newChannel);
				}

				const int cur_index = m_channels[clientChannel.chanelId].store.messages.size();
				for (auto& header : clientChannel.messages.headers) {
					header.index += cur_index;
					m_channels[clientChannel.chanelId].store.headers.push_back(header);
				}
				for (auto& byte : clientChannel.messages.messages) {
					m_channels[clientChannel.chanelId].store.messages.push_back(byte);
				}
				clientChannel.messages.headers.clear();
				clientChannel.messages.messages.clear();
			}
		}
	}
	
	void proccessMessages() {
		std::vector<std::future<void>> futures;
		for (auto& client : m_clients) {
			futures.emplace_back(m_threadPool.enqueue(&MessageBus::processClient,this,client));
		}
		for (auto& future : futures) future.wait();
	}

	void registerClient(MessageClient& client) {
		m_clients.push_back(&client);
	}

	void unregisterClient(MessageClient& client) {

	}

private:
	void processClient(MessageClient* client) {
		for (auto& subId : client->m_subscriptions) {
			if (m_channels.size() <= subId) return;
			client->processMessages(m_channels[subId].store);
		}
	}
	void clearAllChannels() {
		for (auto& channel : m_channels) {
			channel.store.headers.clear();
			channel.store.messages.clear();
		}
	}

	std::vector<Channel> m_channels;
	std::vector<MessageClient*> m_clients;
	ThreadPool m_threadPool;
};

struct msg {
	int i;
};
#include <iostream>

class Core : MessageClient {
public:
	void init() {
		m_messageBus.registerClient(*this);
		registerHandler<msg>(std::bind(&Core::handlemsg, this, std::placeholders::_1));
		subscribe(0);
		subscribe(1);
	}

	void run() {
		running = true;
		postMessage<msg>({ 0 }, 0);
		while (running) {
			m_messageBus.syncMessages();
			m_messageBus.proccessMessages();
		}
	}

private:
	void handlemsg(const msg m) {
		std::cout << m.i << std::endl;
		if (m.i % 2 == 0) {
			postMessage<msg>({ m.i + 1 }, 0);
		} else postMessage<msg>({ m.i + 1 }, 1);
	}
	bool running;
	MessageBus m_messageBus;
};

int main() {
	Core core;
	
	core.init();
	core.run();


	return 0;
}