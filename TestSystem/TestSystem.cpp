#include "TestSystem.h"
#include <OSMessages.h>

#include <Windows.h>
#include <OSGLGraphicsSubSystem.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

const char* vertexshadercode[] = {
	"#version 410\n",
	"uniform mat4 world;\n",
	"in vec3 inPos;\n",
	"in vec3 inColor;\n",

	"smooth out vec3 outColor;\n",
	"void main(){\n",
	"	gl_Position = world*vec4(inPos,1.0f);\n",
	"	outColor = inColor;\n",
	"}\n"
};

const char* fragmentshadercode[] = {
	"#version 410\n",
	"in vec3 outColor;\n",
	"out vec4 color;\n",
	"void main(){\n",
	"	color = vec4(outColor,1.0f);\n",
	"}\n"
};

TestSystem::TestSystem ( ) {
	gl = nullptr;
}

void TestSystem::init (OSFramework* pOS ) {
	m_pOS = pOS;
	m_pOS->initSubSystem (SUBSYSTEM_OPENGL_GRAPHICS);
	registerHandler<WindowEvent> (std::bind (&TestSystem::handleWindowMessage,this,std::placeholders::_1));
	registerHandler<RenderMessage> (std::bind (&TestSystem::render,this,std::placeholders::_1));
	registerHandler<PositionUpdate> (std::bind (&TestSystem::posUpdate,this,std::placeholders::_1));
	registerHandler<RotationUpdate> (std::bind (&TestSystem::rotUpdate,this,std::placeholders::_1));

	subscribe (2);
	subscribe (0);
	subscribe (9);

	(pOS->getOpenGLGraphicsSubSystem())->createWindow("kuchbhi",1000,1000);

	setthreadAfinity (true);
	m_cam.SetFOV (90);
}

void TestSystem::release ( ) {

}

void TestSystem::threadInit ( ) {
	OutputDebugString ("thread init\n");
	m_pOS->getOpenGLGraphicsSubSystem ( )->initOpenGLContext (4,1);
	gl = &m_pOS->getOpenGLGraphicsSubSystem ( )->getGLInterface ( );
	
	gl->ClearColor (0.0f,0.0f,0.0f,1.0f);
	gl->ClearDepth (1.0);
	gl->Enable (GL_DEPTH_TEST);
	gl->Enable (GL_CULL_FACE);
	gl->FrontFace (GL_CCW);

	program = loadShaders ( );
	initGeometry ( );

	gl->UseProgram (program);

	for (int i = 0; i<16; i++){
		for (int j = 0; j<16; j++) {
			Graphic3D g;
			g.worldTransform = glm::translate (glm::vec3 (i,0,j));
			m_entities.push_back ( g );
		}
	}

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
				m_cam.SetAspectRatio (aspect);
				m_cam.Update ( );
			}
		}
	}
}

#include <chrono>
#include <sstream>

void TestSystem::render (const RenderMessage msg) {
	//Render the frame.
	static std::chrono::high_resolution_clock clock;
	static auto now = clock.now ( );
	static auto prev = clock.now ( );
	
	now = clock.now ( );
	std::chrono::duration<double> frame = now-prev;

	gl->Clear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glm::mat4 proj = *m_cam.GetProjectionMatrix ( );
	glm::mat4 view = *m_cam.GetViewMatrix ( );

	GLint wpos = gl->GetUniformLocation (program,"world");
	
	for (auto& graphic:m_entities) {
		glm::mat4 WVP = proj*view*graphic.worldTransform;
		gl->UniformMatrix4fv (wpos,1,false,&WVP[0][0]);

		gl->DrawArrays (GL_TRIANGLES,0,3*2*6);
	}

	m_pOS->getOpenGLGraphicsSubSystem ( )->swapBuffers ( );
	
	prev = now;
	std::stringstream stream;
	stream<<"Framerate: "<<1.0/frame.count ( ) <<"s \n";
	OutputDebugString (stream.str().c_str());
}

void TestSystem::posUpdate (const PositionUpdate msg) {
	m_cam.SetPos (msg.x,msg.y,msg.z);
	m_cam.Update ( );
}

void TestSystem::rotUpdate (const RotationUpdate msg) {
	glm::mat4 rotation = glm::mat4 (1.0f);

	rotation *= glm::rotate (msg.y,glm::vec3 (0,1,0));
	rotation *= glm::rotate (msg.x,glm::vec3 (1,0,0));
	rotation *= glm::rotate (msg.z,glm::vec3 (0,0,1));

	glm::vec3 forward = glm::vec3 (rotation*glm::vec4 (0,0,1,0));
	glm::vec3 look = *m_cam.GetPos ( )+forward;
	m_cam.SetLookAt (look.x,look.y,look.z);
	m_cam.Update ( );
}

unsigned int TestSystem::loadShaders ( ) {
	unsigned int vshader,fshader,program;
	vshader = gl->CreateShader (GL_VERTEX_SHADER);
	fshader = gl->CreateShader (GL_FRAGMENT_SHADER);
	program = gl->CreateProgram ( );

	gl->ShaderSource ((GLuint)vshader,9,vertexshadercode,NULL);
	gl->ShaderSource ((GLuint)fshader,6,fragmentshadercode,NULL);

	gl->CompileShader (vshader);
	int iCompilationStatus;
	gl->GetShaderiv (vshader,GL_COMPILE_STATUS,&iCompilationStatus);

	if (iCompilationStatus==GL_FALSE) {
		GLsizei len;
		gl->GetShaderiv (vshader,GL_INFO_LOG_LENGTH,&len);

		GLchar* log = new GLchar[len+1];
		gl->GetShaderInfoLog (vshader,len,&len,log);
		MessageBox (NULL,log,"Shader could not compile.",NULL);
		delete[] log;
		//TODO: Do I need to Delete the shader?
	}
	gl->CompileShader (fshader);
	gl->GetShaderiv (fshader,GL_COMPILE_STATUS,&iCompilationStatus);

	if (iCompilationStatus==GL_FALSE) {
		GLsizei len;
		gl->GetShaderiv (fshader,GL_INFO_LOG_LENGTH,&len);

		GLchar* log = new GLchar[len+1];
		gl->GetShaderInfoLog (fshader,len,&len,log);
		MessageBox (NULL,log,"Shader could not compile.",NULL);
		delete[] log;
		//TODO: Do I need to Delete the shader?
	}
	gl->AttachShader (program,vshader);
	gl->AttachShader (program,fshader);
	gl->LinkProgram (program);

	gl->DeleteShader (fshader);
	gl->DeleteShader (vshader);

	return program;
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

System * getSystemInstance ( ) {
	return new TestSystem;
}
