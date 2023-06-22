#pragma once

#include "shader.h"
#include "material.h"

class Light
{
public:
	Light() {}

	void SetAsAACube()
	{
#if 0
		5 ----- 6
	  / |     / |
	1 --+-- 2   |
	|   4 --+-- 7
	| /     | /
	0 ----- 3
#endif
		unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
		unsigned int index = 0;
		m_TrianglesN = 12;
		m_VerticesN = 3 * m_TrianglesN;
		m_ConnectivityData.reserve(12 * m_VerticesN);
		m_Indices.reserve(m_VerticesN);

		// Front
		indicesPush3i(BLF, TLF, TRF);
		indicesPush3i(BLF, TRF, BRF);

		// Right
		indicesPush3i(BRF, TRF, TRB);
		indicesPush3i(BRF, TRB, BRB);

		// Back
		indicesPush3i(BRB, TRB, TLB);
		indicesPush3i(BRB, TLB, BLB);

		// Left
		indicesPush3i(BLB, TLB, TLF);
		indicesPush3i(BLB, TLF, BLF);

		// Bottom
		indicesPush3i(BRF, BRB, BLB);
		indicesPush3i(BRF, BLB, BLF);

		// Top
		indicesPush3i(TRF, TRB, TLB);
		indicesPush3i(TRF, TLB, TLF);

		m_Positions.push_back(glm::vec3(-0.5f, -0.5f, 0.5f)); // BLF
		m_Positions.push_back(glm::vec3(-0.5f, 0.5f, 0.5f)); // TLF
		m_Positions.push_back(glm::vec3(0.5f, 0.5f, 0.5f)); // TRF
		m_Positions.push_back(glm::vec3(0.5f, -0.5f, 0.5f)); // BRF
		m_Positions.push_back(glm::vec3(-0.5f, -0.5f, -0.5f)); // BLB
		m_Positions.push_back(glm::vec3(-0.5f, 0.5f, -0.5f)); // TLB
		m_Positions.push_back(glm::vec3(0.5f, 0.5f, -0.5f)); // TRB
		m_Positions.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // BRB

		m_Colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // BLF - Red
		m_Colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // TLF - Yellow
		m_Colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // TRF - Blue
		m_Colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // BRF - Green
		m_Colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // BLB - Red
		m_Colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // TLB - Yellow
		m_Colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // TRB - Blue
		m_Colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // BRB - Green

		m_Normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // POS Z
		m_Normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // POS X
		m_Normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f)); // NEG Z
		m_Normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f)); // NEG X
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f)); // NEG Y
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // POS Y

		m_TexCoords.push_back(glm::vec3(-1.0f, -1.0f, 1.0f)); // BLF
		m_TexCoords.push_back(glm::vec3(-1.0f, 1.0f, 1.0f)); // TLF
		m_TexCoords.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // TRF
		m_TexCoords.push_back(glm::vec3(1.0f, -1.0f, 1.0f)); // BRF
		m_TexCoords.push_back(glm::vec3(-1.0f, -1.0f, -1.0f)); // BLB
		m_TexCoords.push_back(glm::vec3(-1.0f, 1.0f, -1.0f)); // TLB
		m_TexCoords.push_back(glm::vec3(1.0f, 1.0f, -1.0f)); // TRB
		m_TexCoords.push_back(glm::vec3(1.0f, -1.0f, -1.0f)); // BRB

		for (unsigned int i = 0; i < m_VerticesN; i++) {
			unsigned int corner = m_Indices[i];
			connectivityPush3f(m_Positions[corner]);
			connectivityPush3f(m_Colors[corner]);
			connectivityPush3f(m_Normals[i / 6]);
			connectivityPush3f(m_TexCoords[corner]);

			normalPush3f(m_Positions[corner]);
			normalPush3f(m_Positions[corner] + m_Normals[i / 6]);
		}

		for (unsigned int i = 0; i < m_VerticesN; i++) {
			m_Indices[i] = i;
		}

		m_ShaderProgram = Shader("position.vert", "uniformColor.frag");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(float), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		int stride = (int)m_ConnectivityData.size() / (int)m_Indices.size() * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glGenVertexArrays(1, &nVAO);
		glGenBuffers(1, &nVBO);
		glBindVertexArray(nVAO);

		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glBufferData(GL_ARRAY_BUFFER, m_NormalData.size() * sizeof(float), m_NormalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void setColor(glm::vec3 color)
	{
		m_Color = color;
	}

	void setPos(glm::vec3 pos)
	{
		m_Pos = pos;
	}

	void setModel(glm::mat4 model)
	{
		m_Model = model;
	}

	void setView(glm::mat4 view)
	{
		m_View = view;
	}

	void setProj(glm::mat4 proj)
	{
		m_Proj = proj;
	}

	void setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}

	void draw()
	{
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", m_Model);
		m_ShaderProgram.setMat4("view", m_View);
		m_ShaderProgram.setMat4("proj", m_Proj);

		m_ShaderProgram.setVec3("color", m_Color);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void drawNormals(glm::vec3 color)
	{
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", m_Model);
		m_ShaderProgram.setMat4("view", m_View);
		m_ShaderProgram.setMat4("proj", m_Proj);

		m_ShaderProgram.setVec3("color", color);

		glBindVertexArray(nVAO);
		glDrawArrays(GL_LINES, 0, 72);
		glBindVertexArray(0);
	}

	void sendToShader(Shader& shader)
	{
		shader.setVec3("light.position", m_Pos);

		shader.setFloat("light.kA", m_KA);
		shader.setFloat("light.kD", m_KD);
		shader.setFloat("light.kS", m_KS);
	}

	void print(unsigned int start = 0, unsigned int end = 0) {
		std::cout << "Number of Triangles = " << m_TrianglesN << std::endl;

		for (unsigned int i = start; i < 36 * m_TrianglesN && i < 36 * end; i++)
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

	~Light()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &nVAO);
		glDeleteBuffers(1, &nVBO);
		glDeleteBuffers(1, &nEBO);
		glDeleteProgram(m_ShaderProgram.ID);
	}

public:
	glm::vec3 m_Color;
	glm::vec3 m_Pos;
	float m_KA = 1.0f, m_KD = 1.0f, m_KS = 1.0f;

	// 'inherited' TriangleMesh members
	std::vector<float> m_ConnectivityData, m_NormalData;
	std::vector<unsigned int> m_Indices;
	std::vector<glm::vec3> m_Positions, m_Colors, m_Normals, m_TexCoords;
	unsigned int m_TrianglesN, m_VerticesN;
	Material m_Material;
	Shader m_ShaderProgram;
	unsigned int VAO, VBO, EBO;
	unsigned int nVAO, nVBO, nEBO;

private:
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);

private:
	void connectivityPush3f(glm::vec3 v)
	{
		m_ConnectivityData.push_back(v.x);
		m_ConnectivityData.push_back(v.y);
		m_ConnectivityData.push_back(v.z);
	}

	void normalPush3f(glm::vec3 v)
	{
		m_NormalData.push_back(v.x);
		m_NormalData.push_back(v.y);
		m_NormalData.push_back(v.z);
	}

	void indicesPush3i(unsigned int x, unsigned int y, unsigned int z)
	{
		m_Indices.push_back(x);
		m_Indices.push_back(y);
		m_Indices.push_back(z);
	}
};
