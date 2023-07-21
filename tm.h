#pragma once

#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include <iostream>

#include "Vertex.h"

#include "Light.h"
#include "Camera.h"
#include "Material.h"

enum DrawingMode {
	ISOLATED,
	LIT_OBJECT,
	DRAWING_MODE_SIZE
};

class Drawable
{
public:
	Drawable()
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	}

	virtual void Draw() const = 0;

	virtual ~Drawable()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
	}

public:
	std::vector<Vertex> m_ConnectivityData = std::vector<Vertex>();
	std::vector<unsigned int> m_Indices = std::vector<unsigned int>();
	unsigned int m_TriangleCount = 0, m_VertexCount = 0;
	unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;

protected:
	// Push x, y, and z onto m_Indices
	void IndicesPush3I(const unsigned int x, const unsigned int y, const unsigned int z)
	{
		m_Indices.push_back(x);
		m_Indices.push_back(y);
		m_Indices.push_back(z);
	}

	// Sets VAO attribute pointers using connectivity data and indices
	void SetVAOData() const
	{
		if (m_ConnectivityData.empty()) return;

		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(Vertex), m_ConnectivityData.data(), GL_STATIC_DRAW);

		if (!m_Indices.empty())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);
		}

		// TODO: Update size parameters to be generalized
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
		glEnableVertexAttribArray(3);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

class TriangleMesh final : public Drawable
{
public:
	TriangleMesh()
	{
		SetUp();
	}

	TriangleMesh(std::shared_ptr<Material> material, std::vector<std::shared_ptr<PointLight>> lights, std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<SpotLight> spotLight, std::shared_ptr<Camera> camera)
		: m_Material(std::move(material)), m_PointLights(std::move(lights)), m_DirectionalLight(std::move(directionalLight)), m_SpotLight(std::move(spotLight)), m_Camera(std::move(camera))
	{
		SetUp();
	}

	TriangleMesh(std::vector<Vertex> connectivityData, std::vector<unsigned int> indices, std::shared_ptr<Material> material)
		: m_Material(std::move(material))
	{
		m_ConnectivityData = std::move(connectivityData);
		m_Indices = std::move(indices);
		SetUp();
		SetVAOData();
		SetNVAOData();
	}

	// Init shaders and generate VertexArrays and Buffers
	void SetUp()
	{
		m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByVariousLights.frag");

		glGenVertexArrays(1, &m_NVAO);
		glGenBuffers(1, &m_NVBO);
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

	void SetAsAASquare()
	{
		m_VertexCount = 6;
		m_TriangleCount = 2;
		m_Indices.reserve(sizeof(unsigned int) * m_TriangleCount * 3);
		m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);
		
		auto posBL = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		auto posTL = glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);
		auto posTR = glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
		auto posBR = glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);

		auto colorBL = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		auto colorTL = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		auto colorTR = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		auto colorBR = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green

		auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

		auto texCoordBL = glm::vec2(0.0f, 0.0f);
		auto texCoordTL = glm::vec2(0.0f, 1.0f);
		auto texCoordTR = glm::vec2(1.0f, 1.0f);
		auto texCoordBR = glm::vec2(1.0f, 0.0f);

		m_ConnectivityData.emplace_back(posBL, colorBL, normal, texCoordBL); // BL
		m_ConnectivityData.emplace_back(posTR, colorTR, normal, texCoordTR); // TR
		m_ConnectivityData.emplace_back(posTL, colorTL, normal, texCoordTL); // TL
		m_ConnectivityData.emplace_back(posBL, colorBL, normal, texCoordBL); // BL
		m_ConnectivityData.emplace_back(posBR, colorBR, normal, texCoordBR); // BR
		m_ConnectivityData.emplace_back(posTR, colorTR, normal, texCoordTR); // TR

		// Indexing
		// --------
		IndicesPush3I(0, 1, 2);
		IndicesPush3I(3, 4, 5);

		SetVAOData();
		SetNVAOData();
	}

	// Set Mesh as Axis-Aligned Cube with an optional solid color
	void SetAsAACube(glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
	{
		constexpr unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
		m_TriangleCount = 12;
		m_VertexCount = 3 * m_TriangleCount;
		m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);
		m_Indices.reserve(m_VertexCount);

		// Front
		IndicesPush3I(BLF, TRF, TLF);
		IndicesPush3I(BLF, BRF, TRF);

		// Right
		IndicesPush3I(BRF, TRB, TRF);
		IndicesPush3I(BRF, BRB, TRB);

		// Back
		IndicesPush3I(BRB, TLB, TRB);
		IndicesPush3I(BRB, BLB, TLB);

		// Left
		IndicesPush3I(BLB, TLF, TLB);
		IndicesPush3I(BLB, BLF, TLF);

		// Bottom
		IndicesPush3I(BLB, BRF, BLF);
		IndicesPush3I(BLB, BRB, BRF);

		// Top
		IndicesPush3I(TLF, TRB, TLB);
		IndicesPush3I(TLF, TRF, TRB);

		std::vector<glm::vec4> positions;
		std::vector<glm::vec4> colors;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;

		positions.emplace_back(-0.5f, -0.5f,  0.5f, 1.0f); // BLF
		positions.emplace_back(-0.5f,  0.5f,  0.5f, 1.0f); // TLF
		positions.emplace_back( 0.5f,  0.5f,  0.5f, 1.0f); // TRF
		positions.emplace_back( 0.5f, -0.5f,  0.5f, 1.0f); // BRF
		positions.emplace_back(-0.5f, -0.5f, -0.5f, 1.0f); // BLB
		positions.emplace_back(-0.5f,  0.5f, -0.5f, 1.0f); // TLB
		positions.emplace_back( 0.5f,  0.5f, -0.5f, 1.0f); // TRB
		positions.emplace_back( 0.5f, -0.5f, -0.5f, 1.0f); // BRB

		colors.emplace_back(color); // BLF
		colors.emplace_back(color); // TLF
		colors.emplace_back(color); // TRF
		colors.emplace_back(color); // BRF
		colors.emplace_back(color); // BLB
		colors.emplace_back(color); // TLB
		colors.emplace_back(color); // TRB
		colors.emplace_back(color); // BRB

		normals.emplace_back( 0.0f,  0.0f,  1.0f); // POS Z
		normals.emplace_back( 1.0f,  0.0f,  0.0f); // POS X
		normals.emplace_back( 0.0f,  0.0f, -1.0f); // NEG Z
		normals.emplace_back(-1.0f,  0.0f,  0.0f); // NEG X
		normals.emplace_back( 0.0f, -1.0f,  0.0f); // NEG Y
		normals.emplace_back( 0.0f,  1.0f,  0.0f); // POS Y

		texCoords.emplace_back(0.0f, 0.0f);
		texCoords.emplace_back(0.0f, 1.0f);
		texCoords.emplace_back(1.0f, 1.0f);
		texCoords.emplace_back(1.0f, 0.0f);

		const auto texCoordIndices = std::vector<int>{ 0, 2, 1, 0, 3, 2 };

		for (unsigned int i = 0; i < m_VertexCount; i++)
			m_ConnectivityData.emplace_back(positions[m_Indices[i]], colors[m_Indices[i]], normals[i / 6], texCoords[texCoordIndices[i % 6]]);

		for (unsigned int i = 0; i < m_VertexCount; i++)
			m_Indices[i] = i;

		SetVAOData();
		SetNVAOData();
	}

	void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }
	void SetPointLights(const std::vector<std::shared_ptr<PointLight>>& pointLights) { m_PointLights = pointLights; }
	void SetCamera(const std::shared_ptr<Camera>& camera) { m_Camera = camera; }
	void SetVertexCount(const unsigned int count) { m_VertexCount = count; }

	void Draw() const override
	{
		const Shader shader = m_Shaders[m_DrawingMode];
		shader.Use();
		shader.SetMat4("model", m_Model);
		shader.SetMat4("view", m_View);
		shader.SetMat4("proj", m_Proj);
		shader.SetMat4("modelInv", glm::inverse(m_Model));

		switch (m_DrawingMode)
		{
		case ISOLATED:
			break;
		case LIT_OBJECT:
			if (m_Material)
				m_Material->SendToShader(shader);

			shader.SetInt("pointLightCount", PointLight::GetCount());

			for (const auto& pointLight : m_PointLights)
				pointLight->SendToShader(shader);

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
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_VertexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void DrawNormals(const glm::vec4& color = glm::vec4(1.0, 0.0, 1.0, 1.0f)) const
	{
		m_ShaderProgramNormals.Use();
		m_ShaderProgramNormals.SetMat4("model", m_Model);
		m_ShaderProgramNormals.SetMat4("view", m_View);
		m_ShaderProgramNormals.SetMat4("proj", m_Proj);

		m_ShaderProgramNormals.SetVec4("color", color);

		glBindVertexArray(m_NVAO);
		glDrawArrays(GL_LINES, 0, m_VertexCount * 2);
		glBindVertexArray(0);
	}

	~TriangleMesh() override
	{
		glDeleteVertexArrays(1, &m_NVAO);
		glDeleteBuffers(1, &m_NVBO);
	}

public:
	std::shared_ptr<Material> m_Material;
	std::vector<std::shared_ptr<PointLight>> m_PointLights;
	std::shared_ptr<DirectionalLight> m_DirectionalLight;
	std::shared_ptr<SpotLight> m_SpotLight;
	std::shared_ptr<Camera> m_Camera;

	int m_DrawingMode = ISOLATED;

	std::vector<Shader> m_Shaders = std::vector<Shader>();
	Shader m_ShaderProgramNormals = Shader("position.vert", "uniformColor.frag");
	unsigned int m_NVAO = 0, m_NVBO = 0;
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);

private:
	// Sets NVAO using connectivity data
	void SetNVAOData() const
	{
		if (m_ConnectivityData.empty()) return;

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

		glBindVertexArray(m_NVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_NVBO);
		glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(float), normalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

class ScreenMesh final : public Drawable
{
public:
	explicit ScreenMesh(std::shared_ptr<TexColorBuffer> texture = std::make_shared<TexColorBuffer>())
		: m_Texture(std::move(texture))
	{
		m_VertexCount = 6;
		m_TriangleCount = 2;
		m_Indices.reserve(sizeof(unsigned int) * m_TriangleCount * 3);
		m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);

		auto posBL = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
		auto posTL = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
		auto posTR = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		auto posBR = glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);

		auto colorBL = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		auto colorTL = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		auto colorTR = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		auto colorBR = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green

		auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

		auto texCoordBL = glm::vec2(0.0f, 0.0f);
		auto texCoordTL = glm::vec2(0.0f, 1.0f);
		auto texCoordTR = glm::vec2(1.0f, 1.0f);
		auto texCoordBR = glm::vec2(1.0f, 0.0f);

		m_ConnectivityData.emplace_back(posBL, colorBL, normal, texCoordBL); // BL
		m_ConnectivityData.emplace_back(posTR, colorTR, normal, texCoordTR); // TR
		m_ConnectivityData.emplace_back(posTL, colorTL, normal, texCoordTL); // TL
		m_ConnectivityData.emplace_back(posBL, colorBL, normal, texCoordBL); // BL
		m_ConnectivityData.emplace_back(posBR, colorBR, normal, texCoordBR); // BR
		m_ConnectivityData.emplace_back(posTR, colorTR, normal, texCoordTR); // TR

		// Indexing
		// --------
		IndicesPush3I(0, 1, 2);
		IndicesPush3I(3, 4, 5);

		SetVAOData();

		// Set Shader
		m_ScreenShader.Use();
		m_ScreenShader.SetInt("tex", 0);
	}

	void SetTexture(std::shared_ptr<TexColorBuffer> texture) { m_Texture = std::move(texture); }
	std::shared_ptr<TexColorBuffer> GetTexture() { return m_Texture; }

	void UseShader() const
	{
		m_ScreenShader.Use();
		m_Texture->Use();
	}

	void Draw() const override
	{
		UseShader();

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<int>(m_VertexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

private:
	std::shared_ptr<TexColorBuffer> m_Texture;
	Shader m_ScreenShader = Shader("screen.vert", "texture2D.frag");
};
