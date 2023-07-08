#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Shader.h"

/* Largely based on the work of jackw11111
* in his response to the following question:
* https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
*/

class Line
{
public:
	Line() = default;
	Line(glm::vec3 start, glm::vec3 end);
	void SetColor(const glm::vec3 color) { m_Color = color; }
	void SetModel(const glm::mat4& model) { m_Model = model; }
	void SetView(const glm::mat4& view) { m_View = view; }
	void SetProj(const glm::mat4& proj) { m_Proj = proj; }
	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}
	void Draw() const;
	bool operator==(const Line& other) const;
	~Line()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteProgram(m_ShaderProgram.m_ID);
	}

public:
	Shader m_ShaderProgram = Shader();
	unsigned int m_VAO = 0, m_VBO = 0;
	std::vector<float> m_Vertices = std::vector<float>();
	glm::vec3 m_Start = glm::vec3(0.0f);
	glm::vec3 m_End = glm::vec3(0.0f);
	glm::vec3 m_Color = glm::vec3(0.0f);
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);
};