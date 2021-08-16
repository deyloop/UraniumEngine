#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#include <sstream>
#include "WindowsNetworkSubSystem.h"
#include "ErrorCode.h"

namespace u92 {
	WindowsNetworkSubSystem::WindowsNetworkSubSystem ( ) {
		m_winSockInitialized = false;
	}

	int WindowsNetworkSubSystem::init ( ) {
		WSAData wsa_data;

		//Initialize Winsock
		int iResult = 0;
		iResult = WSAStartup (MAKEWORD (2,2),&wsa_data);
		if (iResult!=0) {
			return E_CODE_WINSOCK_INIT_FAILED;
		}

		m_winSockInitialized = true;
		return E_CODE_SUCCESS;
	}
	void WindowsNetworkSubSystem::release ( ) {
		WSACleanup ( );
		m_winSockInitialized = false;
	}
	Socket WindowsNetworkSubSystem::connect (const char * ipaddr,const char * port) {
		if (!m_winSockInitialized) return Socket ( );

		addrinfo *result = NULL,*ptr = NULL,hints;
		SOCKET soc = INVALID_SOCKET;
		int iResult = 0;
		
		ZeroMemory (&hints,sizeof (hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port

		iResult = getaddrinfo (static_cast<LPCTSTR>(ipaddr),port,&hints,&result);
		if (iResult!=0) {
			//poststr("getaddrinfo() failed with error: " + iResult);
			return { -1 };
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr!=NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			soc = socket (ptr->ai_family,ptr->ai_socktype,
							   ptr->ai_protocol);
			if (soc==INVALID_SOCKET) {
				return { -1 };
			}

			// Connect to server.
			iResult = ::connect (soc,ptr->ai_addr,(int)ptr->ai_addrlen);
			if (iResult==SOCKET_ERROR) {
				closesocket (soc);
				soc = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo (result);

		if (soc==INVALID_SOCKET) {
			//poststr ("Unable to connect to server!");;
			return { -1 };
		}

		m_sockets.push_back (soc);
		Socket ret{(m_sockets.size()-1)};
		return ret;
	}
	int WindowsNetworkSubSystem::Send (Socket socket,const char * data,size_t data_size) {
		if (!m_winSockInitialized) {
			return E_CODE_SOCKET_NOINIT;
		}

		int iResult = send (m_sockets[socket.i],data,data_size,0);

		if (iResult<=0) {
			return E_CODE_SOCKET_SENDFAILED;
		}
		//todo handle errors more elegantly
		return E_CODE_SUCCESS;
	}
	
	int WindowsNetworkSubSystem::recieve (Socket soc,char * buffer,size_t buffer_size,bool block) {
		if (!m_winSockInitialized) return E_CODE_SOCKET_NOINIT;

		memset (buffer,0,buffer_size);
		// See if connection pending
		fd_set readSet;
		FD_ZERO (&readSet);
		FD_SET (m_sockets[soc.i],&readSet);
		timeval timeout;
		timeout.tv_sec = 0;  // Zero timeout (poll)
		timeout.tv_usec = 0;
		if (select (m_sockets[soc.i],&readSet,NULL,NULL,(block)?NULL:&timeout)==1) {
			int iResult = recv ((m_sockets[soc.i]),buffer,buffer_size,0);
			if (iResult==SOCKET_ERROR)
				return E_CODE_SOCKET_RECVFAILED;
			return E_CODE_SUCCESS;
		} else return E_CODE_SOCKET_RECV_NODATA;
		
	}

	int WindowsNetworkSubSystem::disconnect (Socket socket) {
		if (socket.i!= -1) {
			closesocket (m_sockets[socket.i]);
		}
		return E_CODE_SUCCESS;
	}

	const char OPTION_VALUE = 1;
	const int MAX_CLIENTS = 5;

	Socket WindowsNetworkSubSystem::setListen (unsigned short port) {
		Socket ret;
		SOCKET listenSoc = INVALID_SOCKET;
		addrinfo hints,*server = nullptr;

		//Setup hints
		ZeroMemory (&hints,sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		//Setup Server
		std::stringstream stream;
		stream << port;
		getaddrinfo (static_cast<LPCTSTR>("127.0.0.1"),stream.str().c_str(),&hints,&server);
		
		//Create a listening socket for connecting to server
		listenSoc = socket (server->ai_family,server->ai_socktype,server->ai_protocol);

		//Setup socket options
		setsockopt (listenSoc,SOL_SOCKET,SO_REUSEADDR,&OPTION_VALUE,sizeof (int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
		setsockopt (listenSoc,IPPROTO_TCP,TCP_NODELAY,&OPTION_VALUE,sizeof (int)); //Used for interactive programs

		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons (port);

		//Assign an address to the server socket.
		bind (listenSoc,(const sockaddr*)&address,sizeof (sockaddr_in));

		//Listen for incoming connections.
		listen (listenSoc,SOMAXCONN);

		//save the socket and return
		m_sockets.push_back (listenSoc);
		ret.i = m_sockets.size ( )-1;
		return ret;
	}
	Socket WindowsNetworkSubSystem::checkAccept (Socket listeningSocket) {
		SOCKET incoming = INVALID_SOCKET;
		Socket ret;
		ret.i = -1;
		if (listeningSocket.i!=-1) {
			// See if connection pending
			fd_set readSet;
			FD_ZERO (&readSet);
			FD_SET (m_sockets[listeningSocket.i],&readSet);
			timeval timeout;
			timeout.tv_sec = 0;  // Zero timeout (poll)
			timeout.tv_usec = 0;
			if (select (m_sockets[listeningSocket.i],&readSet,NULL,NULL,&timeout)==1) {
				incoming = accept (m_sockets[listeningSocket.i],NULL,NULL);
				if (incoming!=INVALID_SOCKET) {
					m_sockets.push_back (incoming);
					ret.i = m_sockets.size ( )-1;
				}
			}
		}

		return ret;
	}
}