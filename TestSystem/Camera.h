#pragma once

#include <glm/glm/glm.hpp>
class Camera {
public:
	Camera ( );

	void CreateProjectionMatrix (float fov,float aspect,float nearPlane,float farPlane);

	void Update ( );

	void SetLookAt (float x,float y,float z);
	void SetFOV (float fov) { CreateProjectionMatrix (fov,m_aspect,m_nearPlane,m_farPlane); }
	void SetAspectRatio (float aspect) { CreateProjectionMatrix (m_fov,aspect,m_nearPlane,m_farPlane); }
	void SetNearPlane (float plane) { CreateProjectionMatrix (m_fov,m_aspect,plane,m_farPlane); }
	void SetFarPlane (float plane) { CreateProjectionMatrix (m_fov,m_aspect,m_nearPlane,plane); }
	void SetPos (float x,float y,float z);

	glm::mat4* GetViewMatrix ( ) { return &m_view; }
	glm::mat4* GetProjectionMatrix ( ) { return &m_projection; }
	glm::vec3* GetLookAt ( ) { return &m_lookAt; }
	float GetFOV ( ) { return m_fov; }
	float GetAspectRatio ( ) { return m_aspect; }
	float GetNearPlane ( ) { return m_nearPlane; }
	float GetFarPlane ( ) { return m_farPlane; }
	
private:


	glm::mat4   m_view;
	glm::mat4	m_projection;

	glm::vec3	m_lookAt;
	glm::vec3   m_pos;

	float       m_fov;
	float       m_aspect;
	float       m_nearPlane;
	float       m_farPlane;
};