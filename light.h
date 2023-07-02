#pragma once

#include "utils.h"

#include "entity.h"

class Light : public Entity
{
public:
	Light()
	{
		m_Shaders.emplace_back("position.vert", "uniformColor.frag");
	}

	explicit Light(const glm::vec3 color)
	{
		m_Color = color;
	}

	void SetColor(const glm::vec3 color)
	{
		m_Color = color;
	}

	void Draw() override {}

	virtual void SendToShader(const Shader& shader) const {}

public:
	glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_KA = 1.0f, m_KD = 1.0f, m_KS = 1.0f;
};

class PointLight : public Light
{
public:
	PointLight() = default;

	explicit PointLight(const float distance) : m_Pos(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		m_Distance = distance;
		m_Linear = 4.48882f / distance;
		m_Quadratic = 75.5817f / (distance * distance);
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

	void SetPos(const glm::vec3 pos)
	{
		m_Pos = pos;
	}

	void Draw() override
	{
		const Shader shader = m_Shaders.at(0);
		shader.Use();
		shader.SetMat4("model", m_Model);
		shader.SetMat4("view", m_View);
		shader.SetMat4("proj", m_Proj);

		shader.SetVec3("color", m_Color);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		CheckForErrors("ERROR::POINT_LIGHT::DRAW: ");
	}

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec3("light.position", m_Pos);
		shader.SetVec3("light.color", m_Color);

		shader.SetFloat("light.kA", m_KA);
		shader.SetFloat("light.kD", m_KD);
		shader.SetFloat("light.kS", m_KS);

		shader.SetFloat("light.constant", m_Constant);
		shader.SetFloat("light.linear", m_Linear);
		shader.SetFloat("light.quadratic", m_Quadratic);
	}

public:
	glm::vec3 m_Pos = glm::vec3(0.0f);
	float m_Distance = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
};

class DirectionalLight final : public Light
{
public:
	DirectionalLight() = default;

	void SetAsAARectangle(int aaPlane, float texCoordScaleFactor = 1.0f) {
		if (aaPlane > 2 || aaPlane < 0) return;

		m_VerticesN = 6;
		m_TrianglesN = 2;
		m_ConnectivityData.reserve(12 * m_VerticesN);
		m_Normals.reserve(3 * 2 * m_VerticesN);

		glm::vec3 posBL;
		glm::vec3 posTL;
		glm::vec3 posTR;
		glm::vec3 posBR;
		glm::vec3 normal;

		if (aaPlane == XY)
		{
			posBL = glm::vec3(-0.5f, -0.5f, 0.0f);
			posTL = glm::vec3(-0.5f, 0.5f, 0.0f);
			posTR = glm::vec3(0.5f, 0.5f, 0.0f);
			posBR = glm::vec3(0.5f, -0.5f, 0.0f);
			normal = glm::vec3(0.0f, 0.0f, 1.0f);
		}
		else if (aaPlane == XZ)
		{
			posBL = glm::vec3(-0.5f, 0.0f, 0.5f);
			posTL = glm::vec3(-0.5f, 0.0f, -0.5f);
			posTR = glm::vec3(0.5f, 0.0f, -0.5f);
			posBR = glm::vec3(0.5f, 0.0f, 0.5f);
			normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else
		{
			posBL = glm::vec3(0.0f, -0.5f, 0.5f);
			posTL = glm::vec3(0.0f, 0.5f, 0.5f);
			posTR = glm::vec3(0.5f, 0.5f, -0.5f);
			posBR = glm::vec3(0.5f, -0.5f, -0.5f);
			normal = glm::vec3(1.0f, 0.0f, 0.0f);
		}

		m_Normals.emplace_back(normal);

		auto colorBL = glm::vec3(1.0f, 0.0f, 0.0f); // Red
		auto colorTL = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
		auto colorTR = glm::vec3(0.0f, 0.0f, 1.0f); // Blue
		auto colorBR = glm::vec3(0.0f, 1.0f, 0.0f); // Green

		auto texCoordBL = glm::vec3(0.0f, 0.0f, 0.0f) * texCoordScaleFactor;
		auto texCoordTL = glm::vec3(0.0f, 1.0f, 0.0f) * texCoordScaleFactor;
		auto texCoordTR = glm::vec3(1.0f, 1.0f, 0.0f) * texCoordScaleFactor;
		auto texCoordBR = glm::vec3(1.0f, 0.0f, 0.0f) * texCoordScaleFactor;

		// Bottom Left
		// -----------
		DataPush3F(m_ConnectivityData, posBL);
		DataPush3F(m_ConnectivityData, colorBL);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordBL);

		// Top Left
		// --------
		DataPush3F(m_ConnectivityData, posTL);
		DataPush3F(m_ConnectivityData, colorTL);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordTL);

		// Top Right
		// ---------
		DataPush3F(m_ConnectivityData, posTR);
		DataPush3F(m_ConnectivityData, colorTR);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordTR);

		// Bottom Left
		// -----------
		DataPush3F(m_ConnectivityData, posBL);
		DataPush3F(m_ConnectivityData, colorBL);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordBL);

		// Top Right
		// ---------
		DataPush3F(m_ConnectivityData, posTR);
		DataPush3F(m_ConnectivityData, colorTR);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordTR);

		// Bottom Right
		// ------------
		DataPush3F(m_ConnectivityData, posBR);
		DataPush3F(m_ConnectivityData, colorBR);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordBR);

		// Buffers
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(float), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Normals
		DataPush3F(m_NormalData, posBL);
		DataPush3F(m_NormalData, posBL + normal);

		DataPush3F(m_NormalData, posTL);
		DataPush3F(m_NormalData, posTL + normal);

		DataPush3F(m_NormalData, posTR);
		DataPush3F(m_NormalData, posTR + normal);

		DataPush3F(m_NormalData, posBR);
		DataPush3F(m_NormalData, posBR + normal);

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

	void Draw() override
	{
		const Shader shader = m_Shaders.at(0);
		shader.Use();
		shader.SetMat4("model", m_Model);
		shader.SetMat4("view", m_View);
		shader.SetMat4("proj", m_Proj);

		shader.SetVec3("color", m_Color);

		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, m_VerticesN);
		glBindVertexArray(0);

		CheckForErrors("ERROR::DIRECTIONAL_LIGHT::DRAW: ");
	}

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec3("light.direction", m_Direction);
		shader.SetVec3("light.color", m_Color);

		shader.SetFloat("light.kA", m_KA);
		shader.SetFloat("light.kD", m_KD);
		shader.SetFloat("light.kS", m_KS);
	}

public:
	glm::vec3 m_Direction = glm::vec3(0.0f);
};

class SpotLight final : public PointLight
{
public:
	SpotLight() = default;

	explicit SpotLight(const float theta) : m_Theta(theta) {}

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec3("light.position", m_Pos);
		shader.SetVec3("light.direction", m_Direction);
		shader.SetVec3("light.color", m_Color);

		shader.SetFloat("light.kA", m_KA);
		shader.SetFloat("light.kD", m_KD);
		shader.SetFloat("light.kS", m_KS);

		shader.SetFloat("light.constant", m_Constant);
		shader.SetFloat("light.linear", m_Linear);
		shader.SetFloat("light.quadratic", m_Quadratic);

		shader.SetFloat("light.cutoff", glm::cos(glm::radians(m_Theta)));
	}

	void Draw() override {}

	void DrawNormals(const glm::vec3& color) const override {}

	~SpotLight() override
	{
		while (!m_Shaders.empty())
		{
			glDeleteProgram(m_Shaders.back().m_ID);
			m_Shaders.pop_back();
		}
	}

public:
	glm::vec3 m_Direction = glm::vec3(0.0f);
	float m_Theta = 0.0f;
};
