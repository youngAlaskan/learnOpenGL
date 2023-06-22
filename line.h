#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "shader.h"

/* Largely based on the work of jackw11111
* in his response to the following question:
* https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
*/

class Line
{
public:
	Line() {}

	Line(glm::vec3 start, glm::vec3 end)
	{
		m_Start = start;
		m_End = end;
		m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
		m_Model = glm::mat4(1.0f);

		m_ShaderProgram = Shader("position.vert", "uniformColor.frag");

		vertices = {
			start.x, start.y, start.z,
			end.x, end.y, end.z
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void setModel(glm::mat4 model) { m_Model = model; }

	void setView(glm::mat4 view) { m_View = view; }

	void setProj(glm::mat4 proj) { m_Proj = proj; }

	void setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 proj) 
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}

	void setColor(glm::vec3 color) { m_Color = color; }

	void draw(glm::mat4 model, glm::mat4 view, glm::mat4 proj, glm::vec3 color)
	{
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", model);
		m_ShaderProgram.setMat4("view", view);
		m_ShaderProgram.setMat4("proj", proj);

		m_ShaderProgram.setVec3("color", color);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void draw()
	{
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", m_Model);
		m_ShaderProgram.setMat4("view", m_View);
		m_ShaderProgram.setMat4("proj", m_Proj);

		m_ShaderProgram.setVec3("color", m_Color);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	~Line()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(m_ShaderProgram.ID);
	}

public:
	Shader m_ShaderProgram;
	unsigned int VAO, VBO;
	std::vector<float> vertices;
	glm::vec3 m_Start;
	glm::vec3 m_End;
	glm::mat4 m_Model, m_View, m_Proj;
	glm::vec3 m_Color;
};