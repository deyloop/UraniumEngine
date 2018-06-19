#pragma once
#ifndef _OS_NETWORK_SUBSYSTEM_H_
#define _OS_NETWORK_SUBSYSTEM_H_

namespace u92 {
	struct Socket {
		void* socket;
	};

	class OSNetworkSubSystem {
	public:
		virtual Socket connect (const char* ipaddr,const char* port)		= 0;
		virtual int send (Socket socket,const char* data,size_t data_size)	= 0;
		virtual int recieve (Socket socket,char* buffer,size_t buffer_size) = 0;
		//TODO add listen
	};
}

#endif