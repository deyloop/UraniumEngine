#include "LogicSystem.h"
#include <Windows.h>
#include <TransformUpdate.h>
#include <glm/glm/gtx/transform.hpp>
#include <glm/glm/gtc/noise.hpp>

#include<CreateEntity.h>
#include<DeleteEntity.h>
#include<AddTransformComponent.h>
#include<AddCameraComponent.h>
#include<AddGraphic3DComponent.h>
#include<RemoveTransformComponent.h>
#include<RemoveGraphic3DComponent.h>
#include<RemoveCameraComponent.h>
#include<InputContexMessagest.h>

#define uvec3(a) a.x,a.y,a.z

int e = 0;
struct Chunk {
	glm::vec2 pos;
	std::vector<int> entities;
};

std::vector<Chunk> chunks;
void LogicSystem::init (OSFramework * pOS) {
	registerHandler<UserInputEvent> (std::bind (&LogicSystem::handleUserInput,this,std::placeholders::_1));
	registerHandler<TickMessage> (std::bind (&LogicSystem::tick,this,std::placeholders::_1));

	subscribe (5);
	subscribe (0);

	m_camtrans.setXPosition (10);
	m_camtrans.setYPosition (10);
	m_camtrans.setZPosition (10);

	m_camtrans.setLookAt (0,0,0);

	postMessage<CreateEntity> ({ 0 },6);
	postMessage<AddCameraComponent> ({ 0 },6);

	
	e = 1;


	//postMessage<PositionUpdate> ({ 2 , 1 , 0 , 1 },6);
	postMessage<LoadContextFile> ({ "..\\Data\\input.xml" },7);
	postMessage<ActivateContext> ({ "system-control" },7);
	postMessage<ActivateContext> ({ "camera" },7);
}

void LogicSystem::release ( ) {

}

void LogicSystem::threadInit ( ) {
	OutputDebugString ("thread init on logic\n");
}

#include <OSMessages.h>

void LogicSystem::handleUserInput (const UserInputEvent event) {
	float s = 0.5f;
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
		m_camtrans.rotateRel (50*event.dy,-50*event.dx,0);
	} else if (event.event=="exit") {
		postMessage<QuitMessage> ({ 0 },10);
		return;
	}

	postMessage<PositionUpdate> ({ 0,	m_camtrans.getXPosition ( ),
								 m_camtrans.getYPosition ( ),
								 m_camtrans.getZPosition ( )
								 },9);
	postMessage<RotationUpdate> ({ 0,
								 m_camtrans.getXRotation ( ),
								 m_camtrans.getYRotation ( ),
								 m_camtrans.getZRotation ( )
								 },9);

}
#include <sstream>

void LogicSystem::tick (const TickMessage msg) {
	//we get the 2d position of the camera
	glm::vec2 campos (m_camtrans.getXPosition ( ),m_camtrans.getZPosition ( ));
	//convert that into chunk positions.
	glm::vec2 chunk = glm::floor(campos/16.0f);
	//see if we already have this chunk generated.
	
	const int renderdist = 2;

	//now, find chunks that need to be generated, and generate one of those chunks this tick
	glm::vec2 chunToGen;
	bool exists = false;
	bool needagen = false;
	for (int i = renderdist; i>=-renderdist; i--) {
		for (int j = renderdist; j>=-renderdist; j--) {
			glm::vec2 testChunk (chunk.x+i,chunk.y+j);
			
			std::stringstream s;
			s<<"Checking chunk "<<testChunk.x<<" "<<testChunk.y<<std::endl;
			//OutputDebugString (s.str ( ).c_str ( ));
			exists = false;
			for (auto& ch:chunks) {
				if (testChunk==ch.pos) {
					exists = true;
					break;
				}
			}

			if (!exists) {
				chunToGen = testChunk;
				needagen = true;
				break;
			}
		}
		if (needagen) break;
	}

	if (needagen) {

		std::stringstream s;
		s<<"Generating chunk "<<chunToGen.x<<" "<<chunToGen.y<<std::endl;
		OutputDebugString (s.str ( ).c_str ( ));

		//we generate the chunk
		Chunk newChunk;
		newChunk.pos = chunToGen;
		
		for (int i = 0; i<16; i++) {
			for (int j = 0; j<16; j++){
				int x = chunToGen.x*16+i;
				int y = chunToGen.y*16+j;
				float hight = glm::perlin (glm::vec2 (x/16.0f,y/16.0f))*20;
				for (int k = 0; k<hight; k++) {
					postMessage<CreateEntity> ({ e },6);
					postMessage<AddTransformComponent> ({ e },6);
					postMessage<AddGraphic3DComponent> ({ e },6);
					postMessage<PositionUpdate> ({ e , float (x) ,float (k) , float (y) },6);
					newChunk.entities.push_back (e);
					e++;
				}
			}
		}//*/
		chunks.push_back (newChunk);

	}

	//remove chunks that are not in the range.
	for (int i = 0; i<chunks.size ( ); i++) {
		auto& ch = chunks[i];
		if (ch.pos.x>chunk.x+renderdist
			||ch.pos.y>chunk.y+renderdist
			||ch.pos.x<chunk.x-renderdist
			||ch.pos.y<chunk.y-renderdist) {
			for(auto ent : ch.entities)
				postMessage<DeleteEntity> ({ ent },6);
			chunks.erase (chunks.begin ( )+i);
			break;
		}
	}
}
