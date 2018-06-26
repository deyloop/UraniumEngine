#include "Texture.h"

GL Texture::gl;

Texture::Texture () {
	m_width					= m_height		= m_bits_per_pixel = 0;
	m_uiTexture				= m_uiSampler	= 0;
	m_mip_maps_generated	= false;
	m_minification			= 0;
	m_magnification			= 0;
	m_filename				= "no_tex.png";
	m_loaded				= false;
	m_generate_mip_maps		= false;
}

void Texture::createFromData (
	unsigned char * data,
	int width,int height,
	int BPP,GLenum format,
	bool generateMipMaps
) {
	// Generate an OpenGL texture ID for this texture
	//m_filename = "";
	m_mip_maps_generated	= generateMipMaps;
	m_width					= width;
	m_height				= height;
	m_bits_per_pixel		= BPP;

	gl.GenTextures (1,(GLuint*)&m_uiTexture);
	gl.BindTexture (GL_TEXTURE_2D,m_uiTexture);

	format = m_bits_per_pixel==32 ? GL_BGRA : m_bits_per_pixel==24 ? GL_BGR : m_bits_per_pixel==8 ? GL_LUMINANCE : 0;
	int internalFormat = m_bits_per_pixel==32 ? GL_RGBA : m_bits_per_pixel==24 ? GL_RGB : GL_DEPTH_COMPONENT;


	gl.TexImage2D (
		GL_TEXTURE_2D,
		0,
		internalFormat,
		m_width,
		m_height,
		0,
		internalFormat,
		GL_UNSIGNED_BYTE,
		data
	);

	if (m_generate_mip_maps) gl.GenerateMipmap (GL_TEXTURE_2D);

	gl.GenSamplers (1,&m_uiSampler);

	m_loaded = true;

}

void Texture::gpuUpload ( ) {
	if (m_loaded) {
		BYTE* Dataptr = FreeImage_GetBits (m_bitMap);

		//Genarate OpenGL texture ID for this texture.
		gl.GenTextures (1,(GLuint*)&m_uiTexture);
		gl.BindTexture (GL_TEXTURE_2D,m_uiTexture);

		int format = m_bits_per_pixel==32 ? GL_BGRA : m_bits_per_pixel==24 ? GL_BGR : m_bits_per_pixel==8 ? GL_LUMINANCE : 0;
		int internalFormat = m_bits_per_pixel==32 ? GL_RGBA : m_bits_per_pixel==24 ? GL_RGB : GL_DEPTH_COMPONENT;

		createFromData (Dataptr,m_width,m_height,m_bits_per_pixel,internalFormat);
		FreeImage_Unload (m_bitMap);

		m_mip_maps_generated = m_generate_mip_maps;
	}
}

void Texture::load ( ) {
	if (!m_loaded) {	//we want to load only once.
		FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
		m_bitMap = 0;

		image_format = FreeImage_GetFileType (m_filename.c_str ( ),0);
		if (image_format==FIF_UNKNOWN) {
			//try to get the format from file extention.
			image_format = FreeImage_GetFIFFromFilename (m_filename.c_str ( ));

			if (image_format==FIF_UNKNOWN) {
				//still unkown, can't load file.
				return;
			}
		}

		if (FreeImage_FIFSupportsReading (image_format)) {
			m_bitMap = FreeImage_Load (image_format,m_filename.c_str ( ));
			if (!m_bitMap) {
				return;
			}
		}

		//set the properties.
		m_width = FreeImage_GetWidth (m_bitMap);
		m_height = FreeImage_GetHeight (m_bitMap);
		m_bits_per_pixel = FreeImage_GetBPP (m_bitMap);

		m_loaded = true;
	}
}

void Texture::unLoad ( ) {
	if (m_loaded) {
		releaseTexture ( );
	}
}

bool Texture::setTexture2D (const char* filename,bool generateMipMaps) {

	m_filename = filename;
	m_generate_mip_maps = generateMipMaps;

	return true;
}

void Texture::setFiltering (int magnification,int minification) {
	// Set magnification filter
	if (magnification==TEXTURE_FILTER_MAG_NEAREST)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	else if (magnification==TEXTURE_FILTER_MAG_BILINEAR)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Set minification filter
	if (minification==TEXTURE_FILTER_MIN_NEAREST)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	else if (minification==TEXTURE_FILTER_MIN_BILINEAR)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	else if (minification==TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
	else if (minification==TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	else if (minification==TEXTURE_FILTER_MIN_TRILINEAR)
		gl.SamplerParameteri (m_uiSampler,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	m_minification = minification;
	m_magnification = magnification;
}

void Texture::setSamplerParameter (unsigned parameter,unsigned value) {
	gl.SamplerParameteri (m_uiSampler,parameter,value);
}

void Texture::bindTexture (int iTextureUnit) {
	gl.ActiveTexture (GL_TEXTURE0+iTextureUnit);
	gl.BindTexture (GL_TEXTURE_2D,m_uiTexture);
	gl.BindSampler (iTextureUnit,m_uiSampler);
}

void Texture::releaseTexture ( ) {
	gl.DeleteSamplers (1,&m_uiSampler);
	gl.DeleteTextures (1,&m_uiTexture);
}