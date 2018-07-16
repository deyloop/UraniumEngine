#pragma once
#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_

#include <System.h>
#include <TickMessage.h>
using namespace u92;

class PhysicsSystem : public System {
public:
	void init (OSFramework* pOS);
	void release ( );

	void threadInit ( );
private:
	void tick (const TickMessage msg);
};


#ifdef URANIUM_EXPORTS
#define U92_API __declspec(dllexport) 
#else
#define U92_API __declspec(dllimport) 
#endif

extern "C" {
	U92_API System* getSystemInstance ( ) {
		return new PhysicsSystem;
	};
}
#endif