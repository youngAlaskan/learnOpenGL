#include "Renderable.h"

#include "glad\glad.h"

Renderable::Renderable()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
}

bool Renderable::operator==(const Renderable& other) const
{
	return m_VAO == other.m_VAO &&
		m_VBO == other.m_VBO &&
		m_EBO == other.m_EBO &&
		m_Type == other.m_Type;
}

Renderable::~Renderable()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}
