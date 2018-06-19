#pragma once
#ifndef _WINDOWS_NETWORK_SUBSYSTEM_H_
#define _WINDOWS_NETWORK_SUBSYSTEM_H_

#include "OSNetworkSubSystem.h"
#include <Windows.h>
#include <list>

namespace u92 {
	class WindowsNetworkSubSystem : public OSNetworkSubSystem {
	public:
		WindowsNetworkSubSystem ( );

		int init ( );
		void release ( );

		Socket connect (const char* ipaddr,const char* port);
		int send (Socket socket,const char* data,size_t data_size);
		int recieve (Socket socket,char* buffer,size_t buffer_size);
		int disconnect (Socket socket);
	private:
		bool m_winSockInitialized;
		std::list<SOCKET> m_sockets;
	};
}
#endif