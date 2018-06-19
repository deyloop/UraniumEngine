#pragma once
#include <glm/glm/glm.hpp>

class Transform {
public:
	Transform ( );
	~Transform ( );

	void reset ( );
	void translateAbs (float x,float y,float z);
	void translateRel (float x,float y,float z);
	void rotateAbs (float x,float y,float z);
	void rotateRel (float x,float y,float z);
	void scaleAbs (float x,float y,float z);
	void scaleRel (float x,float y,float z);

	glm::mat4& getTransform ( );
	glm::vec3& getPosition ( ) { return m_position; }

	float getXPosition ( ) { return m_transX; }
	float getYPosition ( ) { return m_transY; }
	float getZPosition ( ) { return m_transZ; }
	float getXRotation ( ) { return m_rotationX; }
	float getYRotation ( ) { return m_rotationY; }
	float getZRotation ( ) { return m_rotationZ; }
	float getXScale ( ) { return m_scaleX; }
	float getYScale ( ) { return m_scaleY; }
	float getZScale ( ) { return m_scaleZ; }
	void setXPosition (float x) { translateAbs (x,m_transY,m_transZ); }
	void setYPosition (float y) { translateAbs (m_transX,y,m_transZ); }
	void setZPosition (float z) { translateAbs (m_transX,m_transY,z); }
	void setXRotation (float x) { rotateAbs (x,m_rotationY,m_rotationZ); }
	void setYRotation (float y) { rotateAbs (m_rotationX,y,m_rotationZ); }
	void setZRotation (float z) { rotateAbs (m_rotationX,m_rotationY,z); }
	void setXScale (float x) { scaleAbs (x,m_scaleY,m_scaleZ); }
	void setYScale (float y) { scaleAbs (m_scaleX,y,m_scaleZ); }
	void setZScale (float z) { scaleAbs (m_scaleX,m_scaleY,z); }
	void setLookAt (float x,float y,float z);

	const glm::vec3 GetUp ( )		{ return glm::normalize (glm::vec3 (m_rotation*glm::vec4 (0,1,0,1))); }
	const glm::vec3 GetDown ( )		{ return glm::normalize (glm::vec3 (m_rotation*glm::vec4 (0,-1,0,1))); }
	const glm::vec3 GetLeft ( )		{ return glm::normalize (glm::vec3 (m_rotation*glm::vec4 (1,0,0,1))); }
	const glm::vec3 GetRight ( )	{ return glm::normalize (glm::vec3 (m_rotation*glm::vec4 (-1,0,0,1))); }
	const glm::vec3 GetForward ( )	{ return glm::normalize (glm::vec3 (m_rotation*glm::vec4 (0,0,1,1))); }
	const glm::vec3 GetBackWard ( ) { return glm::normalize (glm::vec3 (m_rotation*glm::vec4 (0,0,-1,1))); }
private:
	glm::mat4 m_localTransform,m_rotation,m_translation,m_scale;
	glm::vec3 m_position;

	float	m_rotationX,	m_rotationY,	m_rotationZ,
			m_scaleX,		m_scaleY,		m_scaleZ,
			m_transX,		m_transY,		m_transZ;

};

