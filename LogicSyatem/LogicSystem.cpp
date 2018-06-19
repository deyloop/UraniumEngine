#include "LogicSystem.h"
#include <Windows.h>
#include "TransformUpdate.h"
#include <glm/glm/gtx/transform.hpp>

#define uvec3(a) a.x,a.y,a.z

void LogicSystem::init (OSFramework * pOS) {
	registerHandler<UserInputEvent> (std::bind (&LogicSystem::handleUserInput,this,std::placeholders::_1));
	registerHandler<TickMessage> (std::bind (&LogicSystem::tick,this,std::placeholders::_1));

	subscribe (5);
	subscribe (0);

	m_camtrans.setXPosition (10);
	m_camtrans.setYPosition (10);
	m_camtrans.setZPosition (10);

	m_camtrans.setLookAt (0,0,0);
}

void LogicSystem::release ( ) {

}

void LogicSystem::threadInit ( ) {
	OutputDebugString ("thread init on logic\n");
}

void LogicSystem::handleUserInput (const UserInputEvent event) {
	float s = 0.05f;
	if (event.event=="move-forward") {
		m_camtrans.translateRel (uvec3 (s*m_camtrans.GetForward ( )));
	} else if (event.event=="move-back") {
		m_camtrans.translateRel (uvec3 (s*m_camtrans.GetBackWard ( )));
	} else if (event.event=="move-left") {
		m_camtrans.translateRel (uvec3 (s*m_camtrans.GetLeft ( )));
	} else if (event.event=="move-right") {
		m_camtrans.translateRel (uvec3 (s*m_camtrans.GetRight ( )));
	} else if (event.event=="move-up") {
		m_camtrans.translateRel (0,s,0);
	} else if (event.event=="move-down") {
		m_camtrans.translateRel (0,-s,0);
	} else if (event.event=="rotate-right") {
		m_camtrans.rotateRel (0,-1,0);
	} else if (event.event=="rotate-left") {
		m_camtrans.rotateRel (0,1,0);
	} else if (event.event=="rotate-up") {
		m_camtrans.rotateRel (1,0,0);
	} else if (event.event=="rotate-down") {
		m_camtrans.rotateRel (-1,0,0);
	} else if (event.event=="look-move") {
		m_camtrans.rotateRel (0.0001*event.dy,0.00001*event.dx,0);
	}
	postMessage<TransformUpdate> ({ 0,glm::lookAt(m_camtrans.getPosition(),m_camtrans.getPosition() +m_camtrans.GetForward(),glm::vec3(0,1,0)) },9);
}

void LogicSystem::tick (const TickMessage msg) {

}
