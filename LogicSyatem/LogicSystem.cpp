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
#include <cmath>

#define uvec3(a) a.x,a.y,a.z

int e = 0;
struct Chunk {
	glm::vec3 pos;
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

struct point {
	double x,y,z;
};
struct ray {
	point x0,n;
};
struct box {
	point min,max;
};

bool intersect (box b,ray r) {
	double tmin = -INFINITY,tmax = INFINITY;
	if (r.n.x!=0) {
		double tx1 = (b.min.x-r.x0.x)/r.n.x;
		double tx2 = (b.max.x-r.x0.x)/r.n.x;
		tmin = glm::max (tmin,glm::min (tx1,tx2));
		tmax = glm::min (tmax,glm::max (tx1,tx2));
	}
	if (r.n.y!=0) {
		double ty1 = (b.min.y-r.x0.y)/r.n.y;
		double ty2 = (b.max.y-r.x0.y)/r.n.y;
		tmin = glm::max (tmin,glm::min (ty1,ty2));
		tmax = glm::min (tmax,glm::max (ty1,ty2));
	}
	if (r.n.z!=0) {
		double tz1 = (b.min.z-r.x0.z)/r.n.z;
		double tz2 = (b.max.z-r.x0.z)/r.n.z;
		tmin = glm::max (tmin,glm::min (tz1,tz2));
		tmax = glm::min (tmax,glm::max (tz1,tz2));
	}
	return tmax>=tmin;
}

struct boxe {
	int e;
	box b;
};
std::vector<boxe> boxes;


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
	} else if (event.event=="mine") {
		for (int i = 0; i<boxes.size ( );i++) {
			//now we get the ray.
			ray r;
			r.x0.x = m_camtrans.getXPosition ( );
			r.x0.y = m_camtrans.getYPosition ( );
			r.x0.z = m_camtrans.getZPosition ( );

			glm::vec3 forward = m_camtrans.GetForward ( );
			r.n.x = forward.x;
			r.n.y = forward.y;
			r.n.z = forward.z;
			if (intersect (boxes[i].b,r)) {
				postMessage<DeleteEntity> ({ boxes[i].e },6);
				boxes.erase (boxes.begin ( )+i);
				break;
			}
		}
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
	glm::vec3 campos (m_camtrans.getXPosition ( ),m_camtrans.getYPosition ( ),m_camtrans.getZPosition ( ));

	//generate just one cube;
	
	float x = 2.0f, y=3.0f, z=5.0f;
	static bool f = true;
	if (f) {
		for (int i = 0; i<20; i++) {
			for (int j = 0; j<20; j++) {
				postMessage<CreateEntity> ({ e },6);
				postMessage<AddTransformComponent> ({ e },6);
				postMessage<AddGraphic3DComponent> ({ e },6);
				postMessage<PositionUpdate> ({ e , x+i , y+j, z },6);
				e++;

				//now the box
				box b;
				b.min.x = x+i-0.5;
				b.min.y = y+j-0.5;
				b.min.z = z-0.5;

				b.max.x = x+i+0.5;
				b.max.y = y+j+0.5;
				b.max.z = z+0.5;

				boxe bo;
				bo.b = b;
				bo.e = e-1;
				boxes.push_back (bo);
			}
		}
		f = false;
	}
	

	
	//GenChunks (campos);
}

void LogicSystem::GenChunks (glm::vec3 campos) {

	//convert that into chunk positions.
	glm::vec3 chunk = glm::floor (campos/16.0f);
	//see if we already have this chunk generated.

	const int renderdist = 2;

	//now, find chunks that need to be generated, and generate one of those chunks this tick
	glm::vec3 chunToGen;
	bool exists = false;
	bool needagen = false;
	for (int i = renderdist; i>=-renderdist; i--) {
		for (int j = renderdist; j>=-renderdist; j--) {
			for (int k = renderdist; k>=-renderdist; k--) {
				glm::vec3 testChunk (chunk.x+i,chunk.y+j,chunk.z+k);

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
			for (int j = 0; j<16; j++) {
				for (int k = 0; k<16; k++) {
					int x = chunToGen.x*16+i;
					int y = chunToGen.y*16+j;
					int z = chunToGen.z*16+k;
					float val = glm::perlin (glm::vec3 (x/16.0f,y/16.0f,z/16.0f))*20;
					if (val>10&&val<=12) {
						postMessage<CreateEntity> ({ e },6);
						postMessage<AddTransformComponent> ({ e },6);
						postMessage<AddGraphic3DComponent> ({ e },6);
						postMessage<PositionUpdate> ({ e , float (x) ,float (int (y)) , float (z) },6);
						newChunk.entities.push_back (e);
						e++;
					}
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
			||ch.pos.y<chunk.y-renderdist
			||ch.pos.z>chunk.z+renderdist
			||ch.pos.z<chunk.z-renderdist) {
			for (auto ent:ch.entities)
				postMessage<DeleteEntity> ({ ent },6);
			chunks.erase (chunks.begin ( )+i);
			break;
		}
	}

}
