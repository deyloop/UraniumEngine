#pragma once
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "MessageClient.h"
#include "OSFramework.h"

namespace u92 {
	class System : public core::MessageClient {
	public:
		virtual void init (OSFramework* pOS )		= 0;
		virtual void release ( )	= 0;
	};
}

#endif