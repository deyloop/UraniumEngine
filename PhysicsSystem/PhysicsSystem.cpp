#include "PhysicsSystem.h"

void PhysicsSystem::init (OSFramework * pOS) { 
	registerHandler<TickMessage> (std::bind (&PhysicsSystem::tick,this,std::placeholders::_1));
	subscribe (0);
}

void PhysicsSystem::release ( ) { }

void PhysicsSystem::threadInit ( ) { }

#include <Windows.h>
void PhysicsSystem::tick (const TickMessage msg) {
	OutputDebugString ("physicsTick\n");
}
