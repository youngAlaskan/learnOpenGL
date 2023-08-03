#include "Line.h"

Line::Line(glm::vec3 start, glm::vec3 end, glm::vec4 color)
	: m_Start(start), m_End(end), m_Color(color)
{
	m_Vertices = {
		start.x, start.y, start.z,
		end.x, end.y, end.z
	};

	glBindVertexArray(VAO);

	unsigned int VBO = 0;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);  // NOLINT(bugprone-sizeof-container)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
