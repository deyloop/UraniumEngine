#pragma once
#ifndef _IRESOURCEMANAGER_H_
#define _IRESOURCEMANAGER_H_

#include <functional>

namespace u92 {
	struct ResourceHandle {
		unsigned int size;
		const char* type;
		void* data;
	};

	class IResourceManager {
	public:
		virtual ResourceHandle load (const char* filename,const char* type) = 0;
		virtual void release (ResourceHandle resource) = 0;
		
		virtual ResourceHandle createResource (const __int8* data,const unsigned int size,const char* type) = 0;
		
		virtual void registerResourceCreator (std::function<ResourceHandle (void)> resourceCreator,const char* type) = 0;
		virtual void unregisterResourceCreator (const char* type) = 0;
	};
}

#endif