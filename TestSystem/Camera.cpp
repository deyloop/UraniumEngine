#include "Camera.h"
#include <glm/glm/gtx/transform.hpp>

Camera::Camera ( ) {
	m_fov		= 45.0f;
	m_aspect	= 1.0f;
	m_nearPlane = 0.01f;
	m_farPlane	= 10000.0f;

	m_pos		= glm::vec3 (0.0f,2,10);
	m_lookAt	= glm::vec3 (0.0f);

	m_view = glm::lookAt (m_pos,m_lookAt,glm::vec3 (0.0f,1.0f,0.0f));
	CreateProjectionMatrix (m_fov,m_aspect,m_nearPlane,m_farPlane);
}

void Camera::CreateProjectionMatrix (float fov,float aspect,float nearPlane,float farPlane) {
	m_projection = glm::perspective (fov,aspect,nearPlane,farPlane);
}

void Camera::Update ( ) {
	m_view = glm::lookAt (m_pos,m_lookAt,glm::vec3 (0.0f,1.0f,0.0f));
	CreateProjectionMatrix (m_fov,m_aspect,m_nearPlane,m_farPlane);
}

void Camera::SetLookAt (float x,float y,float z) {
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
}

void Camera::SetPos (float x,float y,float z) {
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}
