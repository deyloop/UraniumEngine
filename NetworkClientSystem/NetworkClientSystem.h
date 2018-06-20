#pragma once
#ifndef _NETWORK_CLIENT_SYSTEM_H_
#define _NETWORK_CLIENT_SYSTEM_H_

#include <System.h>
#include <thread>
#include <OSNetworkSubSystem.h>
#include <TickMessage.h>
#include <vector>

using namespace u92;

struct client {
	int id;
	Socket socket;
};

class NetworkClientSystem : public System{
public:
	void init (OSFramework* pOS);
	void release ( );

	void threadInit ( );
private:
	void process_server_tick (const TickMessage msg);
	void process_client_tick (const TickMessage msg);

	void startServer (unsigned short port );
	void startClient (const char* ipaddr, const char* port );

	OSNetworkSubSystem* m_pNetworkSubSystem;
	bool m_running;
	std::vector<client> m_clients;
	Socket m_serverSoc;
	int clientID;
};

#ifdef URANIUM_EXPORTS
#define U92_API __declspec(dllexport) 
#else
#define U92_API __declspec(dllimport) 
#endif

extern "C" {
	U92_API System* getSystemInstance ( ) {
		return new NetworkClientSystem;
	};
}

#endif
