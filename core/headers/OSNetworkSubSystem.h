#pragma once
#ifndef _OS_NETWORK_SUBSYSTEM_H_
#define _OS_NETWORK_SUBSYSTEM_H_

namespace u92 {
	struct Socket {
		int i;
	};

	class OSNetworkSubSystem {
	public:
		virtual Socket connect (const char* ipaddr,const char* port)		= 0;
		virtual int Send (Socket socket,const char* data,size_t data_size)	= 0;
		virtual int recieve (Socket socket,char* buffer,size_t buffer_size, bool block = false) = 0;
		virtual int disconnect (Socket socket) = 0;
		virtual Socket setListen (unsigned short port)		= 0;
		virtual Socket checkAccept (Socket listeningSocket) = 0;
	};
}

#endif