#pragma once
#include <vector>
#include <string>
#include <OpenGL.h>

class Shader {
public:
	Shader ();
	~Shader ( ) { deleteShader ( ); }

	bool loadShader (const char* file,int type);
	void gpu_Upload ( );
	void deleteShader ( );

	bool isLoaded	( )	{ return m_bLoaded; }
	unsigned getID	( )	{ return m_uiShader; }

	static void setGL (GL _gl) { gl = _gl; }
private:
	unsigned	m_uiShader;			// ID of shader
	int			m_iType;			// GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool		m_bLoaded;			// Whether shader was loaded and compiled
	std::vector<std::string> m_sLines;
	static GL gl;
};