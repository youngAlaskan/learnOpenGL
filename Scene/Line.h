#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <vector>

#include "Components\Renderable\Renderable.h"

/* Largely based on the work of jackw11111
* in his response to the following question:
* https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
*/

class Line final : public Renderable
{
public:
	Line(glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

public:
	std::vector<float> m_Vertices;
	glm::vec3 m_Start = glm::vec3(0.0);
	glm::vec3 m_End = glm::vec3(0.0);
	glm::vec4 m_Color = glm::vec4(1.0, 0.0, 1.0, 0.0);
};
