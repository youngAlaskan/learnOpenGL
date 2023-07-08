#include "Line.h"

#include "utils.h"

Line::Line(glm::vec3 start, glm::vec3 end)
{
	m_Start = start;
	m_End = end;
	m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Model = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);
	m_Proj = glm::mat4(1.0f);

	m_ShaderProgram = Shader("position.vert", "uniformColor.frag");

	m_Vertices = {
		start.x, start.y, start.z,
		end.x, end.y, end.z
	};

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Line::Draw() const
{
	m_ShaderProgram.Use();
	m_ShaderProgram.SetMat4("model", m_Model);
	m_ShaderProgram.SetMat4("view", m_View);
	m_ShaderProgram.SetMat4("proj", m_Proj);

	m_ShaderProgram.SetVec3("color", m_Color);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	CheckForErrors("ERROR while drawing Line: ");
}

bool Line::operator==(const Line& other) const
{
	return m_ShaderProgram == other.m_ShaderProgram &&
		m_VAO == other.m_VAO &&
		m_VBO == other.m_VBO &&
		m_Vertices == other.m_Vertices &&
		m_Start == other.m_Start &&
		m_End == other.m_End &&
		m_Model == other.m_Model &&
		m_View == other.m_View &&
		m_Proj == other.m_Proj &&
		m_Color == other.m_Color;
}

