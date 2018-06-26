#pragma once

#include <OpenGL.h>
#include <FreeImage.h>
#include <string>

enum ETextureFiltering {
	TEXTURE_FILTER_MAG_NEAREST = 0,		// Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR,		// Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST,			// Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR,		// Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP,	// Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR,		// Bilinear criterion for minification on two closest mipmaps, then averaged
};

class Texture {
public:
	Texture ();
	~Texture ( ) { release ( ); }

	void release ( ) { };

	void createFromData (unsigned char* data,int width,int height,int BPP,GLenum format,bool generateMipMap = false);
	bool setTexture2D (const char* filename,bool generateMipMaps = false);
	void bindTexture (int textureUnit = 0);

	void load ( );
	void unLoad ( );
	void gpuUpload ( );

	void setSamplerParameter (unsigned parameter,unsigned value);
	void setFiltering (int magnification,int minification);

	int getMinificationFilter ( ) { return m_minification; }
	int getMagnificationFilter ( ) { return m_magnification; }
	int getWidth ( ) { return m_width; }
	int getHeight ( ) { return m_height; }
	int getBPP ( ) { return m_bits_per_pixel; }

	void releaseTexture ( );

	static void setGL (GL _gl) { gl = _gl; }
private:
	int m_width,m_height,m_bits_per_pixel;
	unsigned m_uiTexture;
	unsigned m_uiSampler;
	bool m_generate_mip_maps;
	bool m_mip_maps_generated;
	bool m_loaded;

	int m_minification,m_magnification;

	std::string m_filename;
	FIBITMAP*   m_bitMap;
	static GL gl;
};