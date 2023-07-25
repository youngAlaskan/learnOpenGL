#pragma once

#include <glm\glm.hpp>

struct Transform
{
	glm::mat4 Model      = glm::mat4(1.0f);
	glm::mat4 View       = glm::mat4(1.0f);
	glm::mat4 Projection = glm::mat4(1.0f);
};
