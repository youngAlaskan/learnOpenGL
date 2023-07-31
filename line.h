#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <vector>

#include "Drawable.h"

/* Largely based on the work of jackw11111
* in his response to the following question:
* https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
*/

class Line final : public Drawable
{
public:
	Line() { m_Type = DrawableType::LINE; }

	Line(glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
		: m_Start(start), m_End(end), m_Color(color)
	{
		m_Type = DrawableType::LINE;

		m_Vertices = {
			start.x, start.y, start.z,
			end.x, end.y, end.z
		};

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);  // NOLINT(bugprone-sizeof-container)

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

public:
	std::vector<float> m_Vertices;
	glm::vec3 m_Start = glm::vec3(0.0);
	glm::vec3 m_End = glm::vec3(0.0);
	glm::vec4 m_Color = glm::vec4(1.0, 0.0, 1.0, 0.0);
};
