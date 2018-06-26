#include "Shader.h"

GL Shader::gl;

Shader::Shader () {
	m_bLoaded	= false;
	m_iType		= 0;
	m_uiShader  = 0;
}

void Shader::gpu_Upload ( ) {
	if (m_bLoaded) {
		const char** program_code = (new const char*[m_sLines.size ( )]);
		for (unsigned int i = 0; i<m_sLines.size ( ); i++)
			program_code[i] = const_cast<char*>(m_sLines[i].c_str ( ));
		m_uiShader = gl.CreateShader (m_iType);

		gl.ShaderSource ((GLuint)m_uiShader,m_sLines.size ( ),program_code,NULL);
		gl.CompileShader (m_uiShader);

		delete[] program_code;

		int iCompilationStatus;
		gl.GetShaderiv (m_uiShader,GL_COMPILE_STATUS,&iCompilationStatus);

		if (iCompilationStatus==GL_FALSE) {
			GLsizei len;
			gl.GetShaderiv (getID ( ),GL_INFO_LOG_LENGTH,&len);

			GLchar* log = new GLchar[len+1];
			gl.GetShaderInfoLog (getID ( ),len,&len,log);
			delete[] log;
			//TODO: Do I need to Delete the shader?
			return;
		}
	}
}

bool Shader::loadShader (const char* file,int type) {
	FILE* fp = nullptr;
	fopen_s (&fp,file,"rt");
	if (!fp) return false;

	// Get all lines from a file
	char sLine[255];
	while (fgets (sLine,255,fp))m_sLines.push_back (sLine);
	fclose (fp);
	
	m_iType = type;
	m_bLoaded = true;

	return true;
}

void Shader::deleteShader ( ) {
	if (!isLoaded ( )) return;
	m_bLoaded = false;
	gl.DeleteShader (m_uiShader);
}