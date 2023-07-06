#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "utils.h"

#include "Renderable.h"

#include "Shader.h"

/* Largely based on the work of jackw11111
* in his response to the following question:
* https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
*/

class Line final : public Renderable
{
public:
	Line() = default;
	Line(glm::vec3 start, glm::vec3 end);
	void SetColor(const glm::vec3 color) { m_Color = color; }
	void Draw() const override;
	bool operator==(const Line& other) const;
	~Line() override
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteProgram(m_ShaderProgram.m_ID);
	}

public:
	Shader m_ShaderProgram = Shader();
	std::vector<float> m_Vertices = std::vector<float>();
	glm::vec3 m_Start = glm::vec3(0.0f);
	glm::vec3 m_End = glm::vec3(0.0f);
	glm::vec3 m_Color = glm::vec3(0.0f);
};