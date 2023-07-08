#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Vertex.h"

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
	LIGHT,
	NORMALS,
	DRAWING_MODE_SIZE
};

class TriangleMesh
{
public:
	TriangleMesh()
	{
		m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");            // ISOLATED
		m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByVariousLights.frag"); // LIT
		m_Shaders.emplace_back("position.vert", "uniformColor.frag");                           // LIGHT
		m_Shaders.emplace_back("position.vert", "uniformColor.frag");                           // NORMALS
	}
	explicit TriangleMesh(const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<PointLight>>& pointLights, const std::shared_ptr<DirectionalLight>& directionalLight, const std::shared_ptr<SpotLight>& spotLight, const std::shared_ptr<Camera>& camera);
	void SetAsAARectangle(int aaPlane, float texCoordScaleFactor = 1.0f);
	void SetAsAACube();
	void SetAsAACube(glm::vec3 color);
	void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }
	void SetPointLights(const std::vector<std::shared_ptr<PointLight>>& pointLights) { m_PointLights = pointLights; }
	void SetCamera(const std::shared_ptr<Camera>& camera) { m_Camera = camera; }
	void SetModel(const glm::mat4& model) { m_Model = model; }
	void SetView(const glm::mat4& view) { m_View = view; }
	void SetProj(const glm::mat4& proj) { m_Proj = proj; }
	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}
	void Draw() const;
	void DrawNormals(const glm::vec3& color) const;
	bool operator==(const TriangleMesh& other) const;
	~TriangleMesh();

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
	std::vector<Shader> m_Shaders = std::vector<Shader>();
	int m_TrianglesN = 0, m_VerticesN = 0;
	unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;
	unsigned int m_NVAO = 0, m_NVBO = 0, m_NEBO = 0;
	Shader m_ShaderProgramNormals = Shader("position.vert", "uniformColor.frag");
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);
};
