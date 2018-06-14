#include <vector>
#include <map>
#include <functional>
#include <type_traits>


#include "MessageBus.h"
using namespace u92::core;

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