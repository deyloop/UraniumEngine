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
	registerHandler<TickMessage> (std::bind (&TestSystem::render,this,std::placeholders::_1));
	registerHandler<UserInputEvent> (std::bind (&TestSystem::input,this,std::placeholders::_1));
	subscribe (2);
	subscribe (0);
	subscribe (5);

	(pOS->getOpenGLGraphicsSubSystem())->createWindow("kuchbhi",500,500);

	setthreadAfinity (true);
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

	//m_pOS->getOpenGLGraphicsSubSystem ( )->swapBuffers ( );
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
			}
		}
	}
}
#include <chrono>
#include <sstream>

void TestSystem::render (const TickMessage msg) {
	//Render the frame.
	static std::chrono::high_resolution_clock clock;
	static auto now = clock.now ( );
	static auto prev = clock.now ( );
	
	now = clock.now ( );
	std::chrono::duration<double> frame = now-prev;
	
	//m_cam.SetPos (0.0f,0.0f,sin (msg.tick_number/1000.0f)*5);

	m_cam.Update ( );

	gl->Clear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glm::mat4 proj = *m_cam.GetProjectionMatrix ( );
	glm::mat4 view = *m_cam.GetViewMatrix ( );
	glm::mat4 world = glm::mat4 (1);
	world = glm::rotate (world,(float)msg.tick_number/20.0f,glm::vec3 (1,0,0));
	world = glm::rotate (world,(float)msg.tick_number/30.0f,glm::vec3 (0,1,0));
	glm::mat4 WVP = proj*view*world;

	GLint wpos = gl->GetUniformLocation (program,"world");
	gl->UniformMatrix4fv (wpos,1,false,&WVP[0][0]);

	gl->DrawArrays (GL_TRIANGLES,0,3*2*6);

	world = glm::mat4 (1);
	world = glm::translate (world,glm::vec3 (2,1,0));
	WVP = proj*view*world;

	gl->UniformMatrix4fv (wpos,1,false,&WVP[0][0]);
	gl->DrawArrays (GL_TRIANGLES,0,3*2*6);
	
	m_pOS->getOpenGLGraphicsSubSystem ( )->swapBuffers ( );
	
	prev = now;
	std::stringstream stream;
	stream<<"Framerate: "<<1.0/frame.count ( ) <<"s \n";
	//OutputDebugString (stream.str().c_str());
}

void TestSystem::input (const UserInputEvent event) {
	if (event.event=="move-forward") {
		m_cam.SetPos (m_cam.GetPos ( )->x,m_cam.GetPos ( )->y,m_cam.GetPos ( )->z+0.1);
	} else if (event.event=="move-back") {
		m_cam.SetPos (m_cam.GetPos ( )->x,m_cam.GetPos ( )->y,m_cam.GetPos ( )->z-0.1);
	} else if (event.event=="move-left") {
		m_cam.SetPos (m_cam.GetPos ( )->x-0.05,m_cam.GetPos ( )->y,m_cam.GetPos ( )->z);
	} else if (event.event=="move-right") {
		m_cam.SetPos (m_cam.GetPos ( )->x+0.05,m_cam.GetPos ( )->y,m_cam.GetPos ( )->z);
	}
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
