#include "WindowsNetworkSubSystem.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "ErrorCode.h"

#pragma comment (lib, "Ws2_32.lib")

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

		struct addrinfo *result = NULL,*ptr = NULL,hints;
		SOCKET socket = INVALID_SOCKET;
		int iResult = 0;
		
		ZeroMemory (&hints,sizeof (hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port

		iResult = getaddrinfo (static_cast<LPCTSTR>(ipaddr),port,&hints,&result);
		if (iResult!=0) {
			//poststr("getaddrinfo() failed with error: " + iResult);
			return { nullptr };
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr!=NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			socket = ::socket (ptr->ai_family,ptr->ai_socktype,
							   ptr->ai_protocol);
			if (socket==INVALID_SOCKET) {
				return { nullptr };
			}

			// Connect to server.
			iResult = ::connect (socket,ptr->ai_addr,(int)ptr->ai_addrlen);
			if (iResult==SOCKET_ERROR) {
				closesocket (socket);
				socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo (result);

		if (socket==INVALID_SOCKET) {
			//poststr ("Unable to connect to server!");;
			WSACleanup ( );
			return;
		}

		m_sockets.push_back (socket);
		Socket ret{(void*)(m_sockets.back())};
		return ret;
	}
	int WindowsNetworkSubSystem::send (Socket socket,const char * data,size_t data_size) {
		if (!m_winSockInitialized) {
			return E_CODE_SOCKET_NOINIT;
		}

		int iResult = ::send (*((SOCKET*)socket.socket),data,data_size,0);

		if (iResult<=0) {
			return E_CODE_SOCKET_SENDFAILED;
		}
		//todo handle errors more elegantly
		return E_CODE_SUCCESS;
	}
	int WindowsNetworkSubSystem::recieve (Socket socket,char * buffer,size_t buffer_size) {
		if (!m_winSockInitialized) return E_CODE_SOCKET_NOINIT;

		memset (buffer,0,buffer_size);
		int iResult = recv (*((SOCKET*)socket.socket),buffer,buffer_size,0);
		if (iResult==SOCKET_ERROR)
			return E_CODE_SOCKET_RECVFAILED;
		return E_CODE_SUCCESS;
	}
}