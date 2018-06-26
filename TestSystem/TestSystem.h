#pragma once
#ifndef _TEST_SYSTEM_H_
#define _TEST_SYSTEM_H_

#include <System.h>

#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Font.h"

#include <WindowEvent.h>
#include <RenderMessage.h>
#include <TransformUpdate.h>

#include<CreateEntity.h>
#include<DeleteEntity.h>
#include<AddTransformComponent.h>
#include<AddCameraComponent.h>
#include<AddGraphic3DComponent.h>
#include<RemoveTransformComponent.h>
#include<RemoveGraphic3DComponent.h>
#include<RemoveCameraComponent.h>

#include <OpenGL.h>
#include "Camera.h"
#include <vector>
#include <map>

#include <glm/glm/glm.hpp>

using namespace u92;

struct Entity {
	int id;
	bool hasTransform; int TransformIndex;
	bool hasGraphic3D; int Graphic3DIndex;
	bool hasCamera;	   int CameraIndex;
};
	
struct Transform {
	glm::mat4 transform;

	glm::vec3 pos,rot,scale;
};

struct Graphic3D{
	int entityId;
	unsigned int VAO;
	Transform trans;
};

class TestSystem : public System {
public:
	TestSystem ( );

	void init ( OSFramework* pOS );
	void release ( );

	void threadInit ( );
private:
	void handleWindowMessage (const WindowEvent event);
	void render (const RenderMessage msg);
	void posUpdate (const PositionUpdate msg);
	void rotUpdate (const RotationUpdate msg);

	void handleCreateEntity				(const CreateEntity				msg);
	void handleDeleteEntity				(const DeleteEntity				msg);
	void handleAddTransformComponent	(const AddTransformComponent	msg);
	void handleAddCameraComponent		(const AddCameraComponent		msg);
	void handleAddGraphic3DComponent	(const AddGraphic3DComponent	msg);
	void handleRemoveTransformComponent	(const RemoveTransformComponent	msg);
	void handleRemoveGraphic3DComponent	(const RemoveGraphic3DComponent	msg);
	void handleRemoveCameraComponent	(const RemoveCameraComponent	msg);

	unsigned int loadShaders ( );
	void initGeometry ( );

	OSFramework* m_pOS;
	GL* gl;

	unsigned int VAO;

	Camera& m_cam;
	std::vector<Camera>	   m_Cameras;
	std::vector<Graphic3D> m_graphic3Ds;
	std::map<int,Entity>   m_entities;

	ShaderProgram m_3DShader;
	ShaderProgram m_FontShader;
	Font m_font;
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