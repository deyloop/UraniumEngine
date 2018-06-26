#include "ShaderProgram.h"
#include "Shader.h"

unsigned ShaderProgram::currentProgramInUse = 0;
GL ShaderProgram::gl;

const bool ShaderProgram::isLinked () const {
	GLint isLinked = GL_FALSE;
	gl.GetProgramiv (m_uiProgram,GL_LINK_STATUS,(int*)&isLinked);
	return isLinked;
}

ShaderProgram::ShaderProgram () {
	m_uiProgram = 0;
}

void ShaderProgram::createProgram ( ) {
	m_uiProgram = gl.CreateProgram ( );
}

void ShaderProgram::deleteProgram ( ) {
	if (!isLinked ( )) return;
	gl.DeleteProgram (m_uiProgram);
}

bool ShaderProgram::addShaderToProgram (Shader* shader) {
	m_shaders.push_back (shader);
	return true;
}

bool ShaderProgram::linkProgram ( ) {
	for (auto shader:m_shaders)
		gl.AttachShader (m_uiProgram,shader->getID());

	gl.LinkProgram (m_uiProgram);
	return isLinked ( );
}

void ShaderProgram::gpuUpload ( ) {
	createProgram ( );
	if (!linkProgram ( )) {
		int len;
		gl.GetProgramiv (m_uiProgram,GL_INFO_LOG_LENGTH,&len);

		char* log = new char[len+1];
		gl.GetProgramInfoLog (m_uiProgram,len,&len,log);
		delete[] log;
		//TODO: Do I need to delete the program?
		return;
	}
}

void ShaderProgram::useProgram ( ) {
	if (isLinked ( )) {
		//if (m_uiProgram!=currentProgramInUse) {
			//currentProgramInUse = m_uiProgram;
			gl.UseProgram (m_uiProgram);
		//}
	}
}