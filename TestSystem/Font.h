#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <string>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "Texture.h"

class ShaderProgram;

class Font {
public:
	Font ();
	~Font ( );

	bool LoadFont (std::string file,int pixelSize);
	bool LoadSystemFont (std::string name,int pxelSize);

	const int GetTextWidth (std::string Text,int pixelSize);

	void print (std::string sText,float x,float y,float iPXSize = -1);
	void printFormatted (float x,float y,float iPXSize,char* sText,...);

	void releaseFont ( );

	void SetShaderProgram (ShaderProgram* shaderProgram);
	static void SetScreenDimentions (int h,int w) {
		screenhieght = h;
		screenwidth = w;
		proj = glm::ortho (0.0f,float (screenwidth),0.0f,float (screenhieght));
	}
	static void setGL (GL _gl) { gl = _gl; }
private:

	void createChar (int iIndex);

	Texture tCharTextures[256];
	float iAdvX[256],iAdvY[256];
	float iBearingX[256],iBearingY[256];
	int iCharWidth[256],iCharHeight[256];
	int iLoadedPixelSize,iNewLine;

	bool bLoaded;

	unsigned m_uiVAO;
	//For buffer data.
	unsigned m_uiVBO;
	std::vector<unsigned char> vboData;

	FT_Library	m_ftLib;
	FT_Face		m_ftFace;
	ShaderProgram* m_ppShaderProgram;

	static int screenwidth;
	static int screenhieght;
	static glm::mat4 proj;
	static GL gl;
};