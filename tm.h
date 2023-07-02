#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "entity.h"
#include "light.h"
#include "camera.h"

enum DrawingMode {
	ISOLATED,
	LIT_BY_POINT,
	LIT_BY_DIRECTIONAL,
	LIT_BY_SPOTLIGHT,
	DRAWING_MODE_SIZE
};

class TriangleMesh final : public Entity
{
public:
	explicit TriangleMesh(Material* material = nullptr, Light* light = nullptr, Camera* camera = nullptr)
	{
		m_Material = material;
		m_Light = light;
		m_Camera = camera;
		m_Shaders.reserve(DRAWING_MODE_SIZE);
		m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByPoint.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByDirectional.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitBySpotlight.frag");
	}

	TriangleMesh(const glm::vec3* vertices, const glm::vec3* colors, const glm::vec3* normals,
		const glm::vec3* texCoords, const int verticesN, const int trianglesN) : m_Material(nullptr), m_Light(nullptr), m_Camera(nullptr)
	{
		m_TrianglesN = trianglesN;
		m_VerticesN = verticesN;

		for (int i = 0; i < verticesN; i++)
		{
			// vertices
			if (vertices)
			{
				m_ConnectivityData.push_back(vertices[i].x);
				m_ConnectivityData.push_back(vertices[i].y);
				m_ConnectivityData.push_back(vertices[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}

			// Colors
			if (colors)
			{
				m_ConnectivityData.push_back(colors[i].x);
				m_ConnectivityData.push_back(colors[i].y);
				m_ConnectivityData.push_back(colors[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}

			// Normals
			if (normals)
			{
				m_ConnectivityData.push_back(normals[i].x);
				m_ConnectivityData.push_back(normals[i].y);
				m_ConnectivityData.push_back(normals[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}

			// texture Coordinates
			if (texCoords)
			{
				m_ConnectivityData.push_back(texCoords[i].x);
				m_ConnectivityData.push_back(texCoords[i].y);
				m_ConnectivityData.push_back(texCoords[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}
		}
	}

	// TODO: Fix this
	void SetAsAARectangle(int aaPlane, float texCoordScaleFactor = 1.0f) {
		if (aaPlane > 2 || aaPlane < 0) return;

		m_VerticesN = 4;
		m_TrianglesN = 2;
		m_Indices.reserve(m_TrianglesN * 3);
		m_ConnectivityData.reserve(12 * m_VerticesN);

		glm::vec3 posBL;
		glm::vec3 posTL;
		glm::vec3 posTR;
		glm::vec3 posBR;
		glm::vec3 normal;

		if (aaPlane == XY)
		{
			posBL = glm::vec3(-0.5f, -0.5f, 0.0f);
			posTL = glm::vec3(-0.5f,  0.5f, 0.0f);
			posTR = glm::vec3( 0.5f,  0.5f, 0.0f);
			posBR = glm::vec3( 0.5f, -0.5f, 0.0f);
			normal = glm::vec3(0.0f, 0.0f, 1.0f);
		}
		else if (aaPlane == XZ)
		{
			posBL = glm::vec3(-0.5f, 0.0f,  0.5f);
			posTL = glm::vec3(-0.5f, 0.0f, -0.5f);
			posTR = glm::vec3( 0.5f, 0.0f, -0.5f);
			posBR = glm::vec3( 0.5f, 0.0f,  0.5f);
			normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else
		{
			posBL = glm::vec3(0.0f, -0.5f,  0.5f);
			posTL = glm::vec3(0.0f,  0.5f,  0.5f);
			posTR = glm::vec3(0.5f,  0.5f, -0.5f);
			posBR = glm::vec3(0.5f, -0.5f, -0.5f);
			normal = glm::vec3(1.0f, 0.0f, 0.0f);
		}

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

		// Bottom Right
		// ------------
		DataPush3F(m_ConnectivityData, posBR);
		DataPush3F(m_ConnectivityData, colorBR);
		DataPush3F(m_ConnectivityData, normal);
		DataPush3F(m_ConnectivityData, texCoordBR);

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
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(float), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), static_cast<void*>(nullptr));
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
	
	void SetAsAACube() {
#if 0
		5-----6
	  / |   / |
	1---+-2   |
	|   4-+---7
	| /   | /
	0-----3
#endif
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
		m_Positions.emplace_back(0.5f,  0.5f,  0.5f); // TRF
		m_Positions.emplace_back(0.5f, -0.5f,  0.5f); // BRF
		m_Positions.emplace_back(-0.5f, -0.5f, -0.5f); // BLB
		m_Positions.emplace_back(-0.5f,  0.5f, -0.5f); // TLB
		m_Positions.emplace_back(0.5f,  0.5f, -0.5f); // TRB
		m_Positions.emplace_back(0.5f, -0.5f, -0.5f);  // BRB

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

		glGenVertexArrays(1, &m_NVAO);
		glGenBuffers(1, &m_NVBO);
		glBindVertexArray(m_NVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_NVBO);
		glBufferData(GL_ARRAY_BUFFER, m_NormalData.size() * sizeof(float), m_NormalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void SetMaterial(Material* material)
	{
		m_Material = material;
	}

	void SetLight(Light* light)
	{
		m_Light = light;
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
		case LIT_BY_POINT:
		case LIT_BY_DIRECTIONAL:
		case LIT_BY_SPOTLIGHT:
			shader.SetMat4("modelInv", glm::inverse(m_Model));

			m_Material->SendToShader(shader);
			m_Light->SendToShader(shader);
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
	Light* m_Light;
	Camera* m_Camera;

	int m_DrawingMode = ISOLATED;
};
