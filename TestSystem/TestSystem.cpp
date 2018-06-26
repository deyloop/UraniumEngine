#include "TestSystem.h"
#include <OSMessages.h>

#include <Windows.h>
#include <OSGLGraphicsSubSystem.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

TestSystem::TestSystem ( ) : m_cam(*(new Camera)){
	gl = nullptr;
}

void TestSystem::init (OSFramework* pOS ) {
	m_pOS = pOS;
	m_pOS->initSubSystem (SUBSYSTEM_OPENGL_GRAPHICS);
	registerHandler<WindowEvent> (std::bind (&TestSystem::handleWindowMessage,this,std::placeholders::_1));
	registerHandler<RenderMessage> (std::bind (&TestSystem::render,this,std::placeholders::_1));
	registerHandler<PositionUpdate> (std::bind (&TestSystem::posUpdate,this,std::placeholders::_1));
	registerHandler<RotationUpdate> (std::bind (&TestSystem::rotUpdate,this,std::placeholders::_1));

	registerHandler<CreateEntity			> (std::bind (&TestSystem::handleCreateEntity,            this,std::placeholders::_1));
	registerHandler<DeleteEntity			> (std::bind (&TestSystem::handleDeleteEntity,            this,std::placeholders::_1));
	registerHandler<AddTransformComponent	> (std::bind (&TestSystem::handleAddTransformComponent,   this,std::placeholders::_1));
	registerHandler<AddCameraComponent		> (std::bind (&TestSystem::handleAddCameraComponent,      this,std::placeholders::_1));
	registerHandler<AddGraphic3DComponent	> (std::bind (&TestSystem::handleAddGraphic3DComponent,   this,std::placeholders::_1));
	registerHandler<RemoveTransformComponent> (std::bind (&TestSystem::handleRemoveTransformComponent,this,std::placeholders::_1));
	registerHandler<RemoveGraphic3DComponent> (std::bind (&TestSystem::handleRemoveGraphic3DComponent,this,std::placeholders::_1));
	registerHandler<RemoveCameraComponent	> (std::bind (&TestSystem::handleRemoveCameraComponent,   this,std::placeholders::_1));

	subscribe (6);
	subscribe (2);
	subscribe (9);
	subscribe (0);

	(pOS->getOpenGLGraphicsSubSystem())->createWindow("A Better Name Pls",700,700);

	setthreadAfinity (true);
}

void TestSystem::release ( ) {

}

void TestSystem::threadInit ( ) {
	OutputDebugString ("thread init\n");
	m_pOS->getOpenGLGraphicsSubSystem ( )->initOpenGLContext (4,1);
	gl = &m_pOS->getOpenGLGraphicsSubSystem ( )->getGLInterface ( );
	
	if (gl==nullptr) throw int (25);
	Shader::setGL (*gl);
	ShaderProgram::setGL (*gl);
	Texture::setGL (*gl);
	Font::setGL (*gl);

	gl->ClearColor (0.0f,0.0f,0.0f,1.0f);
    gl->ClearDepth (1.0);
	gl->Enable (GL_DEPTH_TEST);
	gl->Enable (GL_CULL_FACE);
	gl->FrontFace (GL_CCW);

	loadShaders ( );
	initGeometry ( );

	m_Cameras.emplace_back ( );
	m_cam = m_Cameras.back ( );
	m_cam.SetFOV (90);
	m_cam.Update ( );
}

void TestSystem::handleWindowMessage (const WindowEvent event) {
	switch (event.event.type) {
		case WINDOWEVENTTYPE_CLOSE: {
			WindowCommand cmd;
			cmd.type = WINDOWCOMMAND_DESTROY;
			postMessage<WindowCommand> (cmd,10);
		} break;
		case WINDOWEVENTTYPE_DESTROY: {
			QuitMessage msg = { 0 };
			postMessage<QuitMessage> (msg,10);
		} break;
		case WINDOWEVENTTYPE_RESIZE: {
			if (gl) {
				gl->Viewport (0,0,event.resize.newWidth,event.resize.newHieght);
				float aspect = (float)event.resize.newWidth/event.resize.newHieght;
				if (m_Cameras.size ( )) {
					//m_cam.SetAspectRatio (aspect);
					//m_cam.Update ( );
				}
				Font::SetScreenDimentions (event.resize.newHieght,event.resize.newWidth);
			}
		}
	}
}

#include <chrono>
#include <sstream>

void TestSystem::render (const RenderMessage msg) {
	//Render the frame.
	using namespace std::chrono_literals;
	static std::chrono::high_resolution_clock clock;
	static auto now = clock.now ( );
	static auto prev = clock.now ( );
	static std::chrono::duration<double> acc = 0s;
	now = clock.now ( );
	std::chrono::duration<double> frame = now-prev;
	static std::stringstream stream;
	
	acc += frame;
	if (acc>=500ms) {
		stream.str ("");
		stream<<"Framerate: "<<1.0/frame.count ( )<<" FPS ";
		acc = 0s;
	}
	gl->Clear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	
	glm::mat4 proj = *m_cam.GetProjectionMatrix ( );
	glm::mat4 view = *m_cam.GetViewMatrix ( );

	gl->BindVertexArray (VAO);
	m_3DShader.useProgram ( );

	GLint wpos = gl->GetUniformLocation (m_3DShader.getProgramID(),"world");
	
	for (auto& graphic:m_graphic3Ds) {
		glm::mat4 WVP = proj*view*graphic.trans.transform;
		gl->UniformMatrix4fv (wpos,1,false,&WVP[0][0]);

		gl->DrawArrays (GL_TRIANGLES,0,3*2*6);
	}

	
	prev = now;
	m_font.print (stream.str(),0,0,12);
	//OutputDebugString (stream.str().c_str());

	m_pOS->getOpenGLGraphicsSubSystem ( )->swapBuffers ( );

}

void TestSystem::posUpdate (const PositionUpdate msg) {
	if (m_entities[msg.entity].hasCamera) {
		m_cam.SetPos (msg.x,msg.y,msg.z);
		m_cam.Update ( );
	}
	if (m_entities[msg.entity].hasGraphic3D) {
		m_graphic3Ds[m_entities[msg.entity].Graphic3DIndex].trans.pos = glm::vec3 (msg.x,msg.y,msg.z);
		m_graphic3Ds[m_entities[msg.entity].Graphic3DIndex].trans.transform = glm::translate (m_graphic3Ds[m_entities[msg.entity].Graphic3DIndex].trans.pos);
	}
}

void TestSystem::rotUpdate (const RotationUpdate msg) {
	glm::mat4 rotation = glm::mat4 (1.0f);

	rotation *= glm::rotate (msg.y,glm::vec3 (0,1,0));
	rotation *= glm::rotate (msg.x,glm::vec3 (1,0,0));
	rotation *= glm::rotate (msg.z,glm::vec3 (0,0,1));

	if (m_entities[msg.entity].hasCamera) {
		glm::vec3 forward = glm::vec3 (rotation*glm::vec4 (0,0,1,0));
		glm::vec3 look = *m_cam.GetPos ( )+forward;
		m_cam.SetLookAt (look.x,look.y,look.z);
		m_cam.Update ( );
	}

	if (m_entities[msg.entity].hasGraphic3D) {
		m_graphic3Ds[m_entities[msg.entity].Graphic3DIndex].trans.rot = glm::vec3 (msg.x,msg.y,msg.z);
		m_graphic3Ds[m_entities[msg.entity].Graphic3DIndex].trans.transform = glm::translate(m_graphic3Ds[m_entities[msg.entity].Graphic3DIndex].trans.pos)*rotation;
	}
}

unsigned int TestSystem::loadShaders ( ) {
	Shader vert,frag;
	vert.loadShader ("..\\Data\\3dShader.vert",GL_VERTEX_SHADER);
	frag.loadShader ("..\\Data\\3dshader.frag",GL_FRAGMENT_SHADER);
	vert.gpu_Upload ( );
	frag.gpu_Upload ( );
	m_3DShader.createProgram ( );
	m_3DShader.addShaderToProgram (&vert);
	m_3DShader.addShaderToProgram (&frag);
	m_3DShader.linkProgram ( );
	m_3DShader.gpuUpload ( );
	//m_3DShader.useProgram ( );

	Shader fontvert,fontfrag;
	fontvert.loadShader ("..\\Data\\2DFontShader.vert",GL_VERTEX_SHADER);
	fontfrag.loadShader ("..\\Data\\2DFontShader.frag",GL_FRAGMENT_SHADER);
	fontvert.gpu_Upload ( );
	fontfrag.gpu_Upload ( );
	m_FontShader.createProgram ( );
	m_FontShader.addShaderToProgram (&fontvert);
	m_FontShader.addShaderToProgram (&fontfrag);
	m_FontShader.linkProgram ( );
	m_FontShader.gpuUpload ( );

	m_font.SetShaderProgram (&m_FontShader);
	m_font.LoadFont ("..\\Data\\AliquamREG.ttf",24);
	Font::SetScreenDimentions (700,700);
	return 0; 
}

void TestSystem::initGeometry ( ) {
	unsigned int VBO[2];

	float cube[] = {
		-0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
		0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
		0.5f,-0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
		0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f
	};

	float colorscube[] = {
		1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f,

		0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f,

		0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f,

		1.0f,1.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f,0.0f,

		0.0f,1.0f,1.0f, 0.0f,1.0f,1.0f, 0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f, 0.0f,1.0f,1.0f, 0.0f,1.0f,1.0f,

		1.0f,0.0f,1.0f, 1.0f,0.0f,1.0f, 1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f, 1.0f,0.0f,1.0f, 1.0f,0.0f,1.0f
	};

	gl->GenVertexArrays (1,&VAO);
	gl->GenBuffers (2,VBO);

	gl->BindVertexArray (VAO);

	gl->BindBuffer (GL_ARRAY_BUFFER,VBO[0]);
	gl->BufferData (GL_ARRAY_BUFFER,108*sizeof (float),&cube[0],GL_STATIC_DRAW);
	gl->EnableVertexAttribArray (0);
	gl->VertexAttribPointer (0,3,GL_FLOAT,GL_FALSE,0,0);

	gl->BindBuffer (GL_ARRAY_BUFFER,VBO[1]);
	gl->BufferData (GL_ARRAY_BUFFER,108*sizeof (float),&colorscube[0],GL_STATIC_DRAW);
	gl->EnableVertexAttribArray (1);
	gl->VertexAttribPointer (1,3,GL_FLOAT,GL_FALSE,0,0);
}

void TestSystem::handleCreateEntity				(const CreateEntity				msg){
	Entity& entity = m_entities[msg.id];
	entity = { 0 };
	entity.id = msg.id;
}
void TestSystem::handleDeleteEntity				(const DeleteEntity				msg){
	int entityId = msg.id;
	auto& entity = m_entities[entityId];
	if (entity.hasGraphic3D) {
		m_graphic3Ds[entity.Graphic3DIndex] = m_graphic3Ds.back ( );
		int lastentity = m_graphic3Ds[entity.Graphic3DIndex].entityId;
		m_entities[lastentity].Graphic3DIndex = entity.Graphic3DIndex;
		m_graphic3Ds.pop_back ( );
	}
	m_entities.erase (entityId);
	//if its the camera, we need to handle that casse too.
}

void TestSystem::handleAddTransformComponent	(const AddTransformComponent	msg){
	m_entities[msg.entityId].hasTransform = true;
}

void TestSystem::handleAddCameraComponent		(const AddCameraComponent		msg){
	m_entities[msg.entityId].hasCamera = true;
	m_entities[msg.entityId].CameraIndex = m_Cameras.size ( );
	m_Cameras.emplace_back ( );
	m_cam = m_Cameras.back ( );
}
void TestSystem::handleAddGraphic3DComponent	(const AddGraphic3DComponent	msg){
	m_entities[msg.EntityId].hasGraphic3D = true;
	m_entities[msg.EntityId].Graphic3DIndex = m_graphic3Ds.size ( );
	m_graphic3Ds.emplace_back ( );
	m_graphic3Ds.back ( ).entityId = msg.EntityId;
}
void TestSystem::handleRemoveTransformComponent	(const RemoveTransformComponent	msg){
	int entityId = msg.entityId;
	auto& entity = m_entities[entityId];
	entity.hasTransform = false;
}
void TestSystem::handleRemoveGraphic3DComponent	(const RemoveGraphic3DComponent	msg){
	int entityId = msg.entityId;
	auto& entity = m_entities[entityId];
	if (entity.hasGraphic3D) {
		m_graphic3Ds[entity.Graphic3DIndex] = m_graphic3Ds.back ( );
		int lastentity = m_graphic3Ds[entity.Graphic3DIndex].entityId;
		m_entities[lastentity].Graphic3DIndex = entity.Graphic3DIndex;
		m_graphic3Ds.pop_back ( );
	}
}
void TestSystem::handleRemoveCameraComponent	(const RemoveCameraComponent	msg){
	//nah, we wont.
}

System * getSystemInstance ( ) {
	return new TestSystem;
}
