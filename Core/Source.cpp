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

class MessageClient {
	friend class MessageBus;
public:
protected:
	template <typename Message>
	void postMessage(const Message message) {
		MessageHeader header;
		header.type = typeid(message).hash_code();
		header.index = m_messagesToSend.messages.size();
		for (int i = 0; i < sizeof(Message); i++) m_messagesToSend.messages.push_back('\0');
		memcpy_s(&m_messagesToSend.messages[header.index], sizeof(Message), &message, sizeof(message));
		m_messagesToSend.headers.push_back(header);
	}

	template <typename Message>
	void registerHandler(std::function<void(const Message)> handler) {
		m_handlers[typeid(Message).hash_code()] = [handler](const void* msg) {
			handler(*((const Message*)msg));
		};
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

	MessageStore m_messagesToSend;
	std::map<size_t, std::function<void(const void*)>> m_handlers;
};

#include "ThreadPool.h"

class MessageBus {
public:
	MessageBus() : m_threadPool(std::thread::hardware_concurrency()) {

	}

	void syncMessages() {
		//first we clear the message bus.
		m_messages.headers.clear();
		m_messages.messages.clear();

		//now we get all the messages from the clients.
		for (auto& client : m_clients) {
			const int cur_index = m_messages.messages.size();
			for (auto& header : client->m_messagesToSend.headers) {
				header.index += cur_index;
				m_messages.headers.push_back(header);
			}
			for (auto& byte : client->m_messagesToSend.messages) {
				m_messages.messages.push_back(byte);
			}
			client->m_messagesToSend.headers.clear();
			client->m_messagesToSend.messages.clear();
		}
	}
	
	void proccessMessages() {
		std::vector<std::future<void>> futures;
		for (auto& client : m_clients) {
			futures.emplace_back(m_threadPool.enqueue(&MessageClient::processMessages, client, m_messages));
		}
		for (auto& future : futures) future.wait();
	}

	void registerClient(MessageClient& client) {
		m_clients.push_back(&client);
	}

	void unregisterClient(MessageClient& client) {

	}

private:
	MessageStore m_messages;
	std::vector<MessageClient*> m_clients;
	ThreadPool m_threadPool;
};



class Core : MessageClient {
public:
	void init() {
		m_messageBus.registerClient(*this);
	}

	void run() {
		running = true;
		while (running) {
			m_messageBus.syncMessages();
			m_messageBus.proccessMessages();
		}
	}

private:
	bool running;
	MessageBus m_messageBus;
};

int main() {
	Core core;
	core.init();
	core.run();

	system("pause");
	return 0;
}