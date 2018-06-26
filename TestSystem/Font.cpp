#include "Font.h"
#include "ShaderProgram.h"

#include <freetype/freetype.h>
#include <cmath>

int Font::screenhieght = 0;
int Font::screenwidth = 0;
glm::mat4 Font::proj = glm::mat4 (1.0f);
GL Font::gl;

Font::Font () {
	bLoaded = false;
	m_ppShaderProgram = nullptr;
}

Font::~Font ( ) { };

/*--------------------------------------------------------------------------
Name:	createChar
Params:	iIndex - character index in Unicode.
Result:	Creates one single character (its
texture).
/*-------------------------------------------------------------------------*/

inline int next_p2 (int n) { int res = 1; while (res < n)res <<= 1; return res; }

void Font::createChar (int iIndex) {

	FT_Load_Glyph (m_ftFace,FT_Get_Char_Index (m_ftFace,iIndex),FT_LOAD_DEFAULT);

	FT_Render_Glyph (m_ftFace->glyph,FT_RENDER_MODE_NORMAL);
	FT_Bitmap* pBitmap = &m_ftFace->glyph->bitmap;

	int iW = pBitmap->width,iH = pBitmap->rows;
	int iTW = next_p2 (iW),iTH = next_p2 (iH);

	BYTE* bData = new BYTE[iTW*iTH];
	// Copy glyph data and add dark pixels elsewhere
	for (int ch = 0; ch<iTH; ++ch) {
		for (int cw = 0; cw<iTW; ++cw) {
			bData[ch*iTW+cw] = (ch>=iH||cw>=iW) ? 0 : pBitmap->buffer[(iH-ch-1)*iW+cw];
		}
	}
	// And create a texture from it

	tCharTextures[iIndex].createFromData (bData,iTW,iTH,8,GL_DEPTH_COMPONENT,false);
	tCharTextures[iIndex].setFiltering (TEXTURE_FILTER_MAG_BILINEAR,TEXTURE_FILTER_MIN_BILINEAR);

	tCharTextures[iIndex].setSamplerParameter (GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	tCharTextures[iIndex].setSamplerParameter (GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	// Calculate glyph data
	iAdvX[iIndex] = m_ftFace->glyph->advance.x>>6;
	iBearingX[iIndex] = m_ftFace->glyph->metrics.horiBearingX>>6;
	iCharWidth[iIndex] = m_ftFace->glyph->metrics.width>>6;

	iAdvY[iIndex] = (m_ftFace->glyph->metrics.height-m_ftFace->glyph->metrics.horiBearingY)>>6;
	iBearingY[iIndex] = m_ftFace->glyph->metrics.horiBearingY>>6;
	iCharHeight[iIndex] = m_ftFace->glyph->metrics.height>>6;

	iNewLine = std::fmax (iNewLine,int (m_ftFace->glyph->metrics.height>>6));

	// Rendering data, texture coordinates are always the same, so now we waste a little memory
	glm::vec2 vQuad[] =
	{
		glm::vec2 (0.0f, float (-iAdvY[iIndex]+iTH)),
		glm::vec2 (0.0f, float (-iAdvY[iIndex])),
		glm::vec2 (float (iTW), float (-iAdvY[iIndex]+iTH)),
		glm::vec2 (float (iTW), float (-iAdvY[iIndex]))
	};
	glm::vec2 vTexQuad[] = { glm::vec2 (0.0f, 1.0f), glm::vec2 (0.0f, 0.0f), glm::vec2 (1.0f, 1.0f), glm::vec2 (1.0f, 0.0f) };

	// Add this char to VBO
	for (int i = 0; i<4; ++i) {
		vboData.insert (vboData.end ( ),(unsigned char*)&vQuad[i],(unsigned char*)&vQuad[i]+sizeof (glm::vec2));
		vboData.insert (vboData.end ( ),(unsigned char*)&vTexQuad[i],(unsigned char*)&vTexQuad[i]+sizeof (glm::vec2));
	}
	delete[] bData;


}

/*-------------------------------------------------------------------------
Name:	loadFont
Params:	sFile - path to font file
iPXSize - desired font pixel size
Result:	Loads whole font.
/*-----------------------------------------------------------------------*/

bool Font::LoadFont (std::string File,int PixelSize) {
	FT_Error bError = FT_Init_FreeType (&m_ftLib);

	bError = FT_New_Face (m_ftLib,File.c_str ( ),0,&m_ftFace);
	if (bError)
		return false;

	FT_Set_Pixel_Sizes (m_ftFace,0,PixelSize);
	iLoadedPixelSize = PixelSize;

	gl.GenVertexArrays (1,&m_uiVAO);

	gl.BindVertexArray (m_uiVAO);
	gl.GenBuffers (1,&m_uiVBO);

	gl.BindBuffer (GL_ARRAY_BUFFER,m_uiVBO);

	for (int i = 0; i < 128; ++i)createChar (i);
	bLoaded = true;


	FT_Done_Face (m_ftFace);
	FT_Done_FreeType (m_ftLib);

	/* progID = m_ppShaderProgram->GetProgramID( );
	int vertexpos_loc = glGetAttribLocation(progID, "inPosition");
	int texcoord_loc = glGetAttribLocation(progID, "inTexCoord");
	*/
	gl.BufferData (GL_ARRAY_BUFFER,vboData.size ( )*sizeof (unsigned char),&vboData[0],GL_STATIC_DRAW);
	vboData.clear ( );
	gl.EnableVertexAttribArray (0);
	gl.VertexAttribPointer (0,2,GL_FLOAT,GL_FALSE,sizeof (glm::vec2)*2,0);
	gl.EnableVertexAttribArray (1);
	gl.VertexAttribPointer (1,2,GL_FLOAT,GL_FALSE,sizeof (glm::vec2)*2,(void*)(sizeof (glm::vec2)));

	return true;
}

/*--------------------------------------------------------------------------
Name:	loadSystemFont
Params:	sName - system font name
iPXSize - desired font pixel size
Result:	Loads system font (from system Fonts
directory).
/*-------------------------------------------------------------------------*/
bool Font::LoadSystemFont (std::string sName,int iPXSize) {
	char buf[512];
	//SystemComponent::GetInstance ( )->GetSystemDirectory_ (buf,512);
	std::string sPath = buf;
	sPath += "\\Fonts\\";
	sPath += sName;
	return LoadFont (sPath,iPXSize);
}

/*---------------------------------------------------------------------------
Name:	print
Params:	sText - text to print
x, y - 2D position
iPXSize - printed text size
Result:	Prints text at specified position
with specified pixel size.
/*-------------------------------------------------------------------------*/
void Font::print (std::string sText,float x,float y,float iPXSize) {

	if (!bLoaded)return;
	if (m_ppShaderProgram&&m_ppShaderProgram->isLinked ( )) {
		GLuint progID = m_ppShaderProgram->getProgramID ( );
		m_ppShaderProgram->useProgram ( );
		gl.BindVertexArray (m_uiVAO);
		gl.Uniform1i (gl.GetUniformLocation (progID,"gSampler"),0);

		gl.Disable (GL_DEPTH_TEST);
		gl.Enable (GL_BLEND);
		gl.BlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);



		if (iPXSize==-1)iPXSize = iLoadedPixelSize;
		float fScale = float (iPXSize)/float (iLoadedPixelSize);
		float iCurX = x,iCurY = y;

		for (unsigned int i = 0; i < sText.size ( ); ++i) {
			if (sText[i]=='\n') {
				iCurX = x;
				iCurY -= iNewLine*iPXSize/iLoadedPixelSize;
				continue;
			}
			int iIndex = int (sText[i]);
			iCurX += iBearingX[iIndex]*iPXSize/iLoadedPixelSize;
			if (sText[i]!=' ') {
				tCharTextures[iIndex].bindTexture ( );
				glm::mat4 mModelView = glm::translate (glm::mat4 (1.0f),glm::vec3 (float (iCurX),float (iCurY),0.0f));
				mModelView = glm::scale (mModelView,glm::vec3 (fScale));
				mModelView = proj*mModelView;
				gl.UniformMatrix4fv (gl.GetUniformLocation (progID,"gMVP"),1,GL_FALSE,&mModelView[0][0]);
				// Draw character
				gl.DrawArrays (GL_TRIANGLE_STRIP,iIndex*4,4);
			}

			iCurX += (iAdvX[iIndex]-iBearingX[iIndex])*iPXSize/iLoadedPixelSize;
		}
		gl.Disable (GL_BLEND);
		gl.Enable (GL_DEPTH_TEST);
	}
}

/*--------------------------------------------------------------------------
Name:		printFormatted
Params:	x, y - 2D position
iPXSize - printed text size
sText - text to print
Result:	Prints formatted text at specified position
with specified pixel size.
/*-------------------------------------------------------------------------*/

void Font::printFormatted (float x,float y,float iPXSize,char* sText,...) {
	char buf[512];
	va_list ap;
	va_start (ap,sText);
	vsprintf_s<512> (buf,sText,ap);
	va_end (ap);
	print (buf,x,y,iPXSize);
}

/*--------------------------------------------------------------------------

Name:		releaseFont

Params:	none

Result:	Deletes all font textures.

/*--------------------------------------------------------------------------*/
void Font::releaseFont() {
	for (int i = 0; i<128; ++i)tCharTextures[i].releaseTexture ( );
	gl.DeleteBuffers (1,&m_uiVBO);
	vboData.clear ( );
	gl.DeleteVertexArrays (1,&m_uiVAO);
}

/*-----------------------------------------------------------------------------

Name:	setShaderProgram

Params:	a_shShaderProgram - shader program

Result:	Sets shader program that font uses.

/*-------------------------------------------------------------------------*/

void Font::SetShaderProgram (ShaderProgram* a_shShaderProgram) {
	m_ppShaderProgram = a_shShaderProgram;
}