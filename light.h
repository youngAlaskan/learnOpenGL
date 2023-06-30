#pragma once

#include "utils.h"

#include "entity.h"

class Light final : public Entity
{
public:
	Light() : m_Color(glm::vec3(1.0f, 1.0f, 1.0f)), m_Pos(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		m_Shaders.emplace_back("position.vert", "uniformColor.frag");
	}

	void SetAsAACube(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
	{
#if 0
		5 ----- 6
	  / |     / |
	1 --+-- 2   |
	|   4 --+-- 7
	| /     | /
	0 ----- 3
#endif
		m_Pos = position;
		constexpr unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
		m_TrianglesN = 12;
		m_VerticesN = 3 * m_TrianglesN;
		m_ConnectivityData.reserve(12 * m_VerticesN);
		m_Indices.reserve(m_VerticesN);

		// Front
		IndicesPush3I(BLF, TLF, TRF);
		IndicesPush3I(BLF, TRF, BRF);

		// Right
		IndicesPush3I(BRF, TRF, TRB);
		IndicesPush3I(BRF, TRB, BRB);

		// Back
		IndicesPush3I(BRB, TRB, TLB);
		IndicesPush3I(BRB, TLB, BLB);

		// Left
		IndicesPush3I(BLB, TLB, TLF);
		IndicesPush3I(BLB, TLF, BLF);

		// Bottom
		IndicesPush3I(BRF, BRB, BLB);
		IndicesPush3I(BRF, BLB, BLF);

		// Top
		IndicesPush3I(TRF, TRB, TLB);
		IndicesPush3I(TRF, TLB, TLF);

		m_Positions.emplace_back(-0.5f, -0.5f,  0.5f); // BLF
		m_Positions.emplace_back(-0.5f,  0.5f,  0.5f); // TLF
		m_Positions.emplace_back( 0.5f,  0.5f,  0.5f); // TRF
		m_Positions.emplace_back( 0.5f, -0.5f,  0.5f); // BRF
		m_Positions.emplace_back(-0.5f, -0.5f, -0.5f); // BLB
		m_Positions.emplace_back(-0.5f,  0.5f, -0.5f); // TLB
		m_Positions.emplace_back( 0.5f,  0.5f, -0.5f); // TRB
		m_Positions.emplace_back( 0.5f, -0.5f, -0.5f); // BRB

		for (glm::vec3& pos : m_Positions)
		{
			pos += position;
		}

		m_Colors.emplace_back(1.0f, 0.0f, 0.0f); // BLF - Red
		m_Colors.emplace_back(1.0f, 1.0f, 0.0f); // TLF - Yellow
		m_Colors.emplace_back(0.0f, 0.0f, 1.0f); // TRF - Blue
		m_Colors.emplace_back(0.0f, 1.0f, 0.0f); // BRF - Green
		m_Colors.emplace_back(1.0f, 0.0f, 0.0f); // BLB - Red
		m_Colors.emplace_back(1.0f, 1.0f, 0.0f); // TLB - Yellow
		m_Colors.emplace_back(0.0f, 0.0f, 1.0f); // TRB - Blue
		m_Colors.emplace_back(0.0f, 1.0f, 0.0f); // BRB - Green

		m_Normals.emplace_back(0.0f,  0.0f,  1.0f); // POS Z
		m_Normals.emplace_back(1.0f,  0.0f,  0.0f); // POS X
		m_Normals.emplace_back(0.0f,  0.0f, -1.0f); // NEG Z
		m_Normals.emplace_back(-1.0f,  0.0f,  0.0f); // NEG X
		m_Normals.emplace_back(0.0f, -1.0f,  0.0f); // NEG Y
		m_Normals.emplace_back(0.0f,  1.0f,  0.0f); // POS Y

		m_TexCoords.emplace_back(-1.0f, -1.0f,  1.0f); // BLF
		m_TexCoords.emplace_back(-1.0f,  1.0f,  1.0f); // TLF
		m_TexCoords.emplace_back(1.0f,  1.0f,  1.0f); // TRF
		m_TexCoords.emplace_back(1.0f, -1.0f,  1.0f); // BRF
		m_TexCoords.emplace_back(-1.0f, -1.0f, -1.0f); // BLB
		m_TexCoords.emplace_back(-1.0f,  1.0f, -1.0f); // TLB
		m_TexCoords.emplace_back(1.0f,  1.0f, -1.0f); // TRB
		m_TexCoords.emplace_back(1.0f, -1.0f, -1.0f); // BRB

		for (int i = 0; i < m_VerticesN; i++) {
			const unsigned int corner = m_Indices[i];
			DataPush3F(m_ConnectivityData, m_Positions[corner]);
			DataPush3F(m_ConnectivityData, m_Colors[corner]);
			DataPush3F(m_ConnectivityData, m_Normals[i / 6]);
			DataPush3F(m_ConnectivityData, m_TexCoords[corner]);

			DataPush3F(m_NormalData, m_Positions[corner]);
			DataPush3F(m_NormalData, m_Positions[corner] + m_Normals[i / 6]);
		}

		for (int i = 0; i < m_VerticesN; i++) {
			m_Indices[i] = i;
		}

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(float), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		const int stride = static_cast<int>(m_ConnectivityData.size()) / static_cast<int>(m_Indices.size()) * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Normals
		// ------------------------------

		glGenVertexArrays(1, &m_NVAO);
		glGenBuffers(1, &m_NVBO);
		glBindVertexArray(m_NVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_NVBO);
		glBufferData(GL_ARRAY_BUFFER, m_NormalData.size() * sizeof(float), m_NormalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void SetColor(const glm::vec3 color)
	{
		m_Color = color;
	}

	void SetPos(const glm::vec3 pos)
	{
		m_Pos = pos;
	}

	void Draw() override
	{
		Shader shader = m_Shaders.at(0);
		shader.Use();
		shader.SetMat4("model", m_Model);
		shader.SetMat4("view", m_View);
		shader.SetMat4("proj", m_Proj);

		shader.SetVec3("color", m_Color);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		CheckForErrors("ERROR::LIGHT::DRAW: ");
	}

	void SendToShader(const Shader& shader) const
	{
		shader.SetVec3("light.position", m_Pos);
		shader.SetVec3("light.color", m_Color);

		shader.SetFloat("light.kA", m_KA);
		shader.SetFloat("light.kD", m_KD);
		shader.SetFloat("light.kS", m_KS);
	}

public:
	glm::vec3 m_Color;
	glm::vec3 m_Pos;
	float m_KA = 1.0f, m_KD = 1.0f, m_KS = 1.0f;
};
