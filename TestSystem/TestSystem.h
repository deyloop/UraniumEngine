#pragma once
#ifndef _TEST_SYSTEM_H_
#define _TEST_SYSTEM_H_

#include <System.h>
#include <WindowEvent.h>
#include <TickMessage.h>
#include <OpenGL.h>
#include "Camera.h"
#include <UserInputEvent.h>
#include <vector>
#include <glm/glm/glm.hpp>

using namespace u92;

struct Graphic3D{
	int entityId;
	unsigned int VAO;
	glm::mat4 worldTransform;
};

class TestSystem : public System {
public:
	TestSystem ( );

	void init ( OSFramework* pOS );
	void release ( );

	void threadInit ( );
private:
	void handleWindowMessage (const WindowEvent event);
	void render (const TickMessage msg);
	void input (const UserInputEvent event);

	unsigned int loadShaders ( );
	void initGeometry ( );

	OSFramework* m_pOS;
	GL* gl;

	unsigned int program;
	unsigned int VAO;

	Camera m_cam;
	std::vector<Graphic3D> m_entities;
};

#ifdef URANIUM_EXPORTS
#define U92_API __declspec(dllexport) 
#else
#define U92_API __declspec(dllimport) 
#endif

extern "C" {
U92_API System* getSystemInstance ( );
}
#endif