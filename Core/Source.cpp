#include <vector>
#include <map>
#include <functional>
#include <type_traits>


#include "MessageClient.h"
using namespace u92::core;

struct Channel {
	MessageStore store;
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
class test : public MessageClient {
public:
	void init() {
		subscribe(1);
		registerHandler<msg>(std::bind(&test::handlemsg, this, std::placeholders::_1));
	}
private:
	void handlemsg(const msg m) {
		std::cout << "1: " << m.i << std::endl;
	}
};

class Core : public MessageClient {
public:
	void init() {
		m_messageBus.registerClient(*this);
		registerHandler<msg>(std::bind(&Core::handlemsg, this, std::placeholders::_1));
		subscribe(0);
		subscribe(1);
		t.init();
		m_messageBus.registerClient(t);
		
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
		if (m.i == 300) running = false;
	}
	bool running;
	MessageBus m_messageBus;
	test t;
};

int main() {
	Core core;
	
	core.init();
	core.run();

	system("pause");
	return 0;
}