#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "Entity.h"
#include "Light.h"
#include "Camera.h"

enum DrawingMode {
	ISOLATED,
	LIT,
	DRAWING_MODE_SIZE
};

class TriangleMesh final : public Entity
{
public:
	explicit TriangleMesh(Material* material = nullptr, const std::vector<Light*>& lights = std::vector<Light*>(), Camera* camera = nullptr)
	{
		m_Material = material;
		m_Lights = lights;
		m_Camera = camera;
		m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByVariousLights.frag");
	}

	// TODO: Fix this
	void SetAsAARectangle()
	{
		m_VerticesN = 4;
		m_TrianglesN = 2;
		m_Indices.reserve(m_TrianglesN * 3);
		m_ConnectivityData.reserve(12 * m_VerticesN);
		
		auto posBL = glm::vec3(-0.5f, -0.5f, 0.0f);
		auto posTL = glm::vec3(-0.5f, 0.5f, 0.0f);
		auto posTR = glm::vec3(0.5f, 0.5f, 0.0f);
		auto posBR = glm::vec3(0.5f, -0.5f, 0.0f);
		auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

		auto colorBL = glm::vec3(1.0f, 0.0f, 0.0f); // Red
		auto colorTL = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
		auto colorTR = glm::vec3(0.0f, 0.0f, 1.0f); // Blue
		auto colorBR = glm::vec3(0.0f, 1.0f, 0.0f); // Green

		auto texCoordBL = glm::vec3(0.0f, 0.0f, 0.0f);
		auto texCoordTL = glm::vec3(0.0f, 1.0f, 0.0f);
		auto texCoordTR = glm::vec3(1.0f, 1.0f, 0.0f);
		auto texCoordBR = glm::vec3(1.0f, 0.0f, 0.0f);

		m_ConnectivityData.emplace_back(posBL, colorBL, normal, texCoordBL);
		m_ConnectivityData.emplace_back(posTL, colorTL, normal, texCoordTL);
		m_ConnectivityData.emplace_back(posTR, colorTR, normal, texCoordTR);
		m_ConnectivityData.emplace_back(posBR, colorBR, normal, texCoordBR);

		// Indexing
		// --------
		IndicesPush3I(0, 1, 2);
		IndicesPush3I(0, 2, 3);

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(Vertex), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Normals
		// ------------------------------
		std::vector<float> normalData;

		for (const auto& vertex : m_ConnectivityData)
		{
			normalData.emplace_back(vertex.Position.x);
			normalData.emplace_back(vertex.Position.y);
			normalData.emplace_back(vertex.Position.z);

			normalData.emplace_back(vertex.Position.x + vertex.Normal.x);
			normalData.emplace_back(vertex.Position.y + vertex.Normal.y);
			normalData.emplace_back(vertex.Position.z + vertex.Normal.z);
		}

		glGenVertexArrays(1, &m_NVAO);
		glGenBuffers(1, &m_NVBO);
		glBindVertexArray(m_NVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_NVBO);
		glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(float), normalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	void SetAsAACube() {
		constexpr unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
		m_TrianglesN = 12;
		m_VerticesN = 3 * m_TrianglesN;
		m_ConnectivityData.reserve(sizeof(Vertex) * 12 * m_VerticesN);
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

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> colors;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> texCoords;

		positions.emplace_back(-0.5f, -0.5f, 0.5f); // BLF
		positions.emplace_back(-0.5f, 0.5f, 0.5f); // TLF
		positions.emplace_back(0.5f, 0.5f, 0.5f); // TRF
		positions.emplace_back(0.5f, -0.5f, 0.5f); // BRF
		positions.emplace_back(-0.5f, -0.5f, -0.5f); // BLB
		positions.emplace_back(-0.5f, 0.5f, -0.5f); // TLB
		positions.emplace_back(0.5f, 0.5f, -0.5f); // TRB
		positions.emplace_back(0.5f, -0.5f, -0.5f);  // BRB

		colors.emplace_back(1.0f, 0.0f, 0.0f); // BLF - Red
		colors.emplace_back(1.0f, 1.0f, 0.0f); // TLF - Yellow
		colors.emplace_back(0.0f, 0.0f, 1.0f); // TRF - Blue
		colors.emplace_back(0.0f, 1.0f, 0.0f); // BRF - Green
		colors.emplace_back(1.0f, 0.0f, 0.0f); // BLB - Red
		colors.emplace_back(1.0f, 1.0f, 0.0f); // TLB - Yellow
		colors.emplace_back(0.0f, 0.0f, 1.0f); // TRB - Blue
		colors.emplace_back(0.0f, 1.0f, 0.0f); // BRB - Green

		normals.emplace_back(0.0f, 0.0f, 1.0f); // POS Z
		normals.emplace_back(1.0f, 0.0f, 0.0f); // POS X
		normals.emplace_back(0.0f, 0.0f, -1.0f); // NEG Z
		normals.emplace_back(-1.0f, 0.0f, 0.0f); // NEG X
		normals.emplace_back(0.0f, -1.0f, 0.0f); // NEG Y
		normals.emplace_back(0.0f, 1.0f, 0.0f); // POS Y

		texCoords.emplace_back(-1.0f, -1.0f, 1.0f); // BLF
		texCoords.emplace_back(-1.0f, 1.0f, 1.0f); // TLF
		texCoords.emplace_back(1.0f, 1.0f, 1.0f); // TRF
		texCoords.emplace_back(1.0f, -1.0f, 1.0f); // BRF
		texCoords.emplace_back(-1.0f, -1.0f, -1.0f); // BLB
		texCoords.emplace_back(-1.0f, 1.0f, -1.0f); // TLB
		texCoords.emplace_back(1.0f, 1.0f, -1.0f); // TRB
		texCoords.emplace_back(1.0f, -1.0f, -1.0f); // BRB

		for (int i = 0; i < m_VerticesN; i++) {
			const unsigned int corner = m_Indices[i];
			m_ConnectivityData.emplace_back(positions[corner], colors[corner], normals[i / 6], texCoords[corner]);
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
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(Vertex), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		constexpr int stride = 12 * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Normals
		// ------------------
		std::vector<float> normalData;

		for (const auto& vertex : m_ConnectivityData)
		{
			normalData.emplace_back(vertex.Position.x);
			normalData.emplace_back(vertex.Position.y);
			normalData.emplace_back(vertex.Position.z);

			normalData.emplace_back(vertex.Position.x + vertex.Normal.x);
			normalData.emplace_back(vertex.Position.y + vertex.Normal.y);
			normalData.emplace_back(vertex.Position.z + vertex.Normal.z);
		}

		glGenVertexArrays(1, &m_NVAO);
		glGenBuffers(1, &m_NVBO);
		glBindVertexArray(m_NVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_NVBO);
		glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(float), normalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void SetMaterial(Material* material)
	{
		m_Material = material;
	}

	void SetLights(const std::vector<Light*>& lights)
	{
		m_Lights = lights;
	}

	void SetCamera(Camera* camera)
	{
		m_Camera = camera;
	}

	void Draw() override
	{
		const Shader shader = m_Shaders[m_DrawingMode];
		shader.Use();
		shader.SetMat4("model", m_Model);
		shader.SetMat4("view", m_View);
		shader.SetMat4("proj", m_Proj);

		switch (m_DrawingMode) {
		case ISOLATED:
			break;
		case LIT:
			shader.SetMat4("modelInv", glm::inverse(m_Model));

			m_Material->SendToShader(shader);

			for (const Light* light : m_Lights)
			{
				light->SendToShader(shader);
			}
			m_Camera->SendToShader(shader);

			break;
		default:
			break;
		}

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		CheckForErrors("ERROR::TM::DRAW: ");
	}

public:
	Material* m_Material;
	std::vector<Light*> m_Lights;
	Camera* m_Camera;

	int m_DrawingMode = ISOLATED;
};
