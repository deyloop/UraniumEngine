#pragma once
#include <glm/glm/glm.hpp>

struct TransformUpdate{
	int entity;
	glm::mat4 transform;
};