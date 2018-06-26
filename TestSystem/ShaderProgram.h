#pragma once
#include <vector>;
#include <OpenGL.h>

class Shader;

class ShaderProgram{
public:
	void createProgram ( );
	void deleteProgram ( );

	bool addShaderToProgram (Shader* shShader);
	bool linkProgram ( );
	void gpuUpload ( );

	void useProgram ( );

	const unsigned getProgramID ( ) const { return m_uiProgram; }
	const bool isLinked ( ) const;
	ShaderProgram ();

	static void setGL (GL _gl) { gl = _gl; }
private:
	unsigned m_uiProgram; // ID of program
	static unsigned currentProgramInUse;
	std::vector<Shader*> m_shaders;
	static GL gl;
};