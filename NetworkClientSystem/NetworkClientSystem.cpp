#include "NetworkClientSystem.h"
#include <string>
#include <Windows.h>
#include <ErrorCode.h>
#include <fstream>

void NetworkClientSystem::init (OSFramework * pOS) {
	pOS->initSubSystem (SUBSYSTEM_NETWORK);
	m_pNetworkSubSystem = pOS->getNetworkSubSystem ( );

	std::ifstream f ("..\\Data\\network.txt");
	std::string mode;
	f>>mode;
	if (mode=="SERVER") {
		unsigned short port;
		f>>port;
		startServer ( port );

		registerHandler<TickMessage> (std::bind (&NetworkClientSystem::process_server_tick,this,std::placeholders::_1));
	} else {
		std::string ipaddr, port;
		f>>ipaddr;
		f>>port;
		startClient (ipaddr.c_str ( ),port.c_str ( ));

		registerHandler<TickMessage> (std::bind (&NetworkClientSystem::process_client_tick,this,std::placeholders::_1));
	}

	subscribe (0);
}

void NetworkClientSystem::release ( ) { }

void NetworkClientSystem::threadInit ( ) { }

void NetworkClientSystem::process_server_tick (const TickMessage tmsg) {
	if (!m_running) return;
	char buffer[256];

	//see if any new connections
	Socket newClient = m_pNetworkSubSystem->checkAccept (m_serverSoc);
	if (newClient.i!=-1) {
		client newClientStruct;
		newClientStruct.id = m_clients.size ( );
		newClientStruct.socket = newClient;
		m_clients.push_back (newClientStruct);
		m_pNetworkSubSystem->Send (newClient,(const char*)&newClientStruct.id,sizeof (int));
		OutputDebugString ("Client Connected\n");
	}

	//handle messages from clients.
	for (auto& client:m_clients) {
		
		int r = E_CODE_SUCCESS;
		do {
			for (int i = 0; i<256; i++) buffer[i] = 0;

			r = m_pNetworkSubSystem->recieve (client.socket,buffer,256);
			if (r!=E_CODE_SUCCESS) {
				//if(r == E_CODE_SOCKET_RECV_NODATA)
				continue;
			}

			std::string msg = buffer;
			while (msg.back ( )=='\n') {
				msg.pop_back ( );
			}

			OutputDebugString (msg.c_str ( ));
			for (auto& sendclient:m_clients) {
				if (sendclient.id==client.id) continue;
				//send the message 
				m_pNetworkSubSystem->Send (sendclient.socket,msg.c_str ( ),msg.size ( )+1);
			}
		} while (r==E_CODE_SUCCESS);
	}
}

void NetworkClientSystem::process_client_tick (const TickMessage msg) {
	if (!m_running) return;
	char buffer[256];

	int r = E_CODE_SUCCESS;
	do {
		for (int i = 0; i<256; i++) buffer[i] = 0;

		r = m_pNetworkSubSystem->recieve (m_serverSoc,buffer,256);
		if (r!=E_CODE_SUCCESS) {
			//if(r == E_CODE_SOCKET_RECV_NODATA)
			continue;
		}

		std::string msg = buffer;
		while (msg.back ( )=='\n') {
			msg.pop_back ( );
		}

		OutputDebugString (msg.c_str ( ));

	} while (r==E_CODE_SUCCESS);
}

void NetworkClientSystem::startServer (unsigned short port) {
	m_serverSoc = m_pNetworkSubSystem->setListen (port);
	if (m_serverSoc.i!=-1) {
		m_running = true;
	}
}

void NetworkClientSystem::startClient (const char * ipaddr,const char * port) {
	m_serverSoc= m_pNetworkSubSystem->connect ("localhost","8888");
	if (m_serverSoc.i!=-1) {
		std::string msg;
		char msgrc[256];
		m_pNetworkSubSystem->recieve (m_serverSoc,msgrc,256,true);
		msg = msgrc;

		if (msg!="Server is full") {
			clientID = atoi (msgrc);
			m_running = true;
		}
	}
}
