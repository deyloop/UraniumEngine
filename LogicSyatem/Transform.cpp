#include "Transform.h"
#include <glm/glm/gtx/transform.hpp>


Transform::Transform ( ) {
	reset ( );
}


Transform::~Transform ( ) { }


void Transform::reset ( ) {
	m_localTransform = m_translation = m_rotation = m_scale = glm::mat4 (1.0f);

	m_position = glm::vec3 (0.0f,0.0f,0.0f);

	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
	m_scaleX = m_scaleY = m_scaleZ = 1.0f;
	m_transX = m_transY = m_transZ = 0.0f;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Absolute translation
Parameters:
[in] x - X position
[in] y - Y position
[in] z - Z position
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Transform::translateAbs (float x,float y,float z) {
	m_transX = x;
	m_transY = y;
	m_transZ = z;

	m_translation = glm::mat4 (1.0f);
	m_translation = glm::translate (m_translation,glm::vec3 (x,y,z));

	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Relative translation
Parameters:
[in] x - X amount
[in] y - Y amount
[in] z - Z amount
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Transform::translateRel (float x,float y,float z) {
	m_transX += x;
	m_transY += y;
	m_transZ += z;

	m_translation = glm::translate (m_translation,glm::vec3 (x,y,z));

	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Absolute rotation
Parameters:
[in] x - X radians
[in] y - Y radians
[in] z - Z radians
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Transform::rotateAbs (float x,float y,float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	m_rotation = glm::mat4 (1.0f);

	m_rotation *= glm::rotate (y,glm::vec3 (0.0f,1.0f,0.0f));
	m_rotation *= glm::rotate (x,glm::vec3 (1.0f,0.0f,0.0f));
	m_rotation *= glm::rotate (z,glm::vec3 (0.0f,0.0f,1.0f));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Relative rotation
Parameters:
[in] x - X radians
[in] y - Y radians
[in] z - Z radians
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Transform::rotateRel (float x,float y,float z) {
	m_rotationX += x;
	m_rotationY += y;
	m_rotationZ += z;

	rotateAbs (m_rotationX,m_rotationY,m_rotationZ);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Absolute scale.
Parameters:
[in] x - X size
[in] y - Y size
[in] z - Z size
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Transform::scaleAbs (float x,float y,float z) {
	m_scaleX = x;
	m_scaleY = y;
	m_scaleZ = z;

	m_scale = glm::mat4 (1.0f);

	m_scale = glm::scale (m_scale,glm::vec3 (x,y,z));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Relative scale.
Parameters:
[in] x - X size
[in] y - Y size
[in] z - Z size
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Transform::scaleRel (float x,float y,float z) {
	m_scaleX += x;
	m_scaleY += y;
	m_scaleZ += z;
	m_scale = glm::scale (m_scale,glm::vec3 (x,y,z));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Get the transformation matrix
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
glm::mat4& Transform::getTransform ( ) {
	m_localTransform = m_translation*m_rotation*m_scale;
	return m_localTransform;
}



void Transform::setLookAt (float x,float y,float z) {

	glm::vec3 lookPoint = glm::vec3 (x,y,z);

	glm::vec3 lookDir = glm::normalize (lookPoint-m_position);

	float lookLengthOnXZ = sqrtf (lookDir.z*lookDir.z+lookDir.x*lookDir.x);
	m_rotationX = -glm::degrees (atan2f (lookDir.y,lookLengthOnXZ));
	m_rotationY = glm::degrees (atan2f (lookDir.x,lookDir.z));

	rotateAbs (m_rotationX,m_rotationY,m_rotationZ);
}