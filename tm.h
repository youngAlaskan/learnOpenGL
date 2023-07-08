#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "Vertex.h"

#include "Light.h"
#include "Camera.h"

enum DrawingMode {
	ISOLATED,
	LIT,
	DRAWING_MODE_SIZE
};

class TriangleMesh
{
public:
	TriangleMesh()
	{
		m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByVariousLights.frag");
	}

	TriangleMesh(std::shared_ptr<Material> material, std::vector<std::shared_ptr<PointLight>> lights, std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<SpotLight> spotLight, std::shared_ptr<Camera> camera)
		: m_Material(std::move(material)), m_PointLights(std::move(lights)), m_DirectionalLight(std::move(directionalLight)), m_SpotLight(std::move(spotLight)), m_Camera(std::move(camera))
	{
		m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByVariousLights.frag");
	}

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

	void SetAsAACube(glm::vec3 color)
	{
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

		colors.emplace_back(color); // BLF
		colors.emplace_back(color); // TLF
		colors.emplace_back(color); // TRF
		colors.emplace_back(color); // BRF
		colors.emplace_back(color); // BLB
		colors.emplace_back(color); // TLB
		colors.emplace_back(color); // TRB
		colors.emplace_back(color); // BRB

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

	void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }
	void SetPointLights(const std::vector<std::shared_ptr<PointLight>>& pointLights) { m_PointLights = pointLights; }
	void SetCamera(const std::shared_ptr<Camera>& camera) { m_Camera = camera; }

	void Draw() const
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

			if (m_Material)
				m_Material->SendToShader(shader);

			for (const auto& pointLight : m_PointLights)
			{
				pointLight->SendToShader(shader);
			}

			if (m_DirectionalLight)
				m_DirectionalLight->SendToShader(shader);

			if (m_SpotLight)
				m_SpotLight->SendToShader(shader);

			if (m_Camera)
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

	~TriangleMesh()
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

public:
	std::shared_ptr<Material> m_Material;
	std::vector<std::shared_ptr<PointLight>> m_PointLights;
	std::shared_ptr<DirectionalLight> m_DirectionalLight;
	std::shared_ptr<SpotLight> m_SpotLight;
	std::shared_ptr<Camera> m_Camera;

	int m_DrawingMode = ISOLATED;

private:
	void IndicesPush3I(const unsigned int x, const unsigned int y, const unsigned int z)
	{
		m_Indices.push_back(x);
		m_Indices.push_back(y);
		m_Indices.push_back(z);
	}

private:
	std::vector<Vertex> m_ConnectivityData;
	std::vector<unsigned int> m_Indices;
	std::vector<Shader> m_Shaders;
	int m_TrianglesN = 0, m_VerticesN = 0;
	Shader m_ShaderProgramNormals = Shader("position.vert", "uniformColor.frag");
	unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;
	unsigned int m_NVAO = 0, m_NVBO = 0, m_NEBO = 0;
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);
};
