#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "Renderable.h"

#include "Light.h"
#include "Camera.h"
#include "Material.h"

enum Planes {
	XY,
	XZ,
	YZ
};

enum DrawingMode {
	ISOLATED,
	LIT,
	NORMALS,
	DRAWING_MODE_SIZE
};

struct Vertex
{
public:
	Vertex() = default;

	Vertex(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& norm, const glm::vec3& texCoord)
		: m_Position(pos), m_Color(col), m_Normal(norm), m_TexCoord(texCoord) {}

	~Vertex() = default;
public:
	glm::vec3 m_Position;
	glm::vec3 m_Color;
	glm::vec3 m_Normal;
	glm::vec3 m_TexCoord;
};

class TriangleMesh final : public Renderable
{
public:
	TriangleMesh() = default;
	explicit TriangleMesh(const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<PointLight>>& pointLights, const std::shared_ptr<DirectionalLight>& directionalLight, const std::shared_ptr<SpotLight>& spotLight, const std::shared_ptr<Camera>& camera);
	void SetAsAARectangle(int aaPlane, float texCoordScaleFactor = 1.0f);
	void SetAsAACube();
	void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }
	void SetPointLights(const std::vector<std::shared_ptr<PointLight>>& pointLights) { m_PointLights = pointLights; }
	void SetCamera(const std::shared_ptr<Camera>& camera) { m_Camera = camera; }
	void Draw() const override;
	void DrawNormals(const glm::vec3& color) const;
	bool operator==(const TriangleMesh& other) const;
	~TriangleMesh() override;

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
	std::vector<float> m_NormalData;
	std::vector<unsigned int> m_Indices;
	std::vector<Shader> m_Shaders = std::vector<Shader>();
	int m_TrianglesN = 0, m_VerticesN = 0;
	unsigned int m_EBO = 0;
	unsigned int m_NVAO = 0, m_NVBO = 0, m_NEBO = 0;
	Shader m_ShaderProgramNormals = Shader("position.vert", "uniformColor.frag");
};
