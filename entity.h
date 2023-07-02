#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "utils.h"

#include "shader.h"

enum Planes {
	XY,
	XZ,
	YZ
};

class Entity
{
public:
	Entity() = default;

	virtual void Draw() {}

	void SetModel(const glm::mat4& model)
	{
		m_Model = model;
	}

	void SetView(const glm::mat4& view)
	{
		m_View = view;
	}

	void SetProj(const glm::mat4& proj)
	{
		m_Proj = proj;
	}

	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}

	virtual void DrawNormals(const glm::vec3& color) const
	{
		m_ShaderProgramNormals.Use();
		m_ShaderProgramNormals.SetMat4("model", m_Model);
		m_ShaderProgramNormals.SetMat4("view", m_View);
		m_ShaderProgramNormals.SetMat4("proj", m_Proj);

		m_ShaderProgramNormals.SetVec3("color", color);

		glBindVertexArray(m_NVAO);
		glDrawArrays(GL_LINES, 0, 72);
		glBindVertexArray(0);

		CheckForErrors("ERROR::DRAW_NORMALS: ");
	}

	void PrintConnectivity(const int start = 0, const int end = 0) const {
		std::cout << "Number of Triangles = " << m_TrianglesN << std::endl;

		for (int i = start; i < 36 * m_TrianglesN && i < 36 * end; i++)
		{
			if (i == start)
			{
				std::cout << "Triangle: " << start << std::endl << '<';
			}
			else if (i % 36 == 0)
			{
				std::cout << '>' << std::endl << "Triangle: " << i / 36 << std::endl << '<';
			}
			else if (i % 12 == 0)
			{
				std::cout << '>' << std::endl << '<';
			}
			else if (i % 12 == 3 || i % 12 == 6 || i % 12 == 9)
			{
				std::cout << ">, <";
			}
			else
			{
				std::cout << ", ";
			}

			if (m_ConnectivityData[i] >= 0.0f)
				std::cout << ' ';
			std::cout << m_ConnectivityData[i];
		}

		std::cout << '>' << std::endl;
	}

	bool operator==(const Entity& other) const
	{
		return m_ConnectivityData == other.m_ConnectivityData &&
			m_NormalData == other.m_NormalData &&
			m_Indices == other.m_Indices &&
			m_Positions == other.m_Positions &&
			m_Colors == other.m_Colors &&
			m_Normals == other.m_Normals &&
			m_TexCoords == other.m_TexCoords &&
			m_Shaders == other.m_Shaders &&
			m_TrianglesN == other.m_TrianglesN &&
			m_VerticesN == other.m_VerticesN &&
			m_ShaderProgramNormals == other.m_ShaderProgramNormals &&
			m_VAO == other.m_VAO &&
			m_VBO == other.m_VBO &&
			m_EBO == other.m_EBO &&
			m_NVAO == other.m_NVAO &&
			m_NVBO == other.m_NVBO &&
			m_NEBO == other.m_NEBO &&
			m_Model == other.m_Model &&
			m_View == other.m_View &&
			m_Proj == other.m_Proj;
	}

	virtual ~Entity()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		glDeleteVertexArrays(1, &m_NVAO);
		glDeleteBuffers(1, &m_NVBO);
		glDeleteBuffers(1, &m_NEBO);

		while (!m_Shaders.empty())
		{
			glDeleteProgram(m_Shaders.back().m_ID);
			m_Shaders.pop_back();
		}
	}

protected:
	std::vector<float> m_ConnectivityData, m_NormalData;
	std::vector<unsigned int> m_Indices;
	std::vector<glm::vec3> m_Positions, m_Colors, m_Normals, m_TexCoords;
	std::vector<Shader> m_Shaders;
	int m_TrianglesN = 0, m_VerticesN = 0;
	Shader m_ShaderProgramNormals = Shader("position.vert", "uniformColor.frag");
	unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;
	unsigned int m_NVAO = 0, m_NVBO = 0, m_NEBO = 0;
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);

protected:
	static void DataPush3F(std::vector<float>& data, const glm::vec3 v)
	{
		data.push_back(v.x);
		data.push_back(v.y);
		data.push_back(v.z);
	}

	void IndicesPush3I(const unsigned int x, const unsigned int y, const unsigned int z)
	{
		m_Indices.push_back(x);
		m_Indices.push_back(y);
		m_Indices.push_back(z);
	}
};
