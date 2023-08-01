#pragma once

#include <vector>

#include "Renderable.h"

#include "..\Vertex.h"

// Abstract Class for Triangle Meshes
class AbstractTriangleMesh : public Renderable
{
public:
	std::vector<Vertex> m_ConnectivityData = std::vector<Vertex>();
	std::vector<unsigned int> m_Indices = std::vector<unsigned int>();
	unsigned int m_TriangleCount = 0, m_VertexCount = 0;

	DrawingMode m_DrawingMode = DrawingMode::ISOLATED;

protected:
	// Sets VAO attribute pointers using connectivity data and indices
	void SetVAOData() const;
};

class TriangleMesh final : public AbstractTriangleMesh
{
public:
	TriangleMesh();
	explicit TriangleMesh(std::shared_ptr<MaterialComponent> material, DrawingMode drawingMode = DrawingMode::ISOLATED);
	TriangleMesh(std::vector<Vertex> connectivityData, std::vector<unsigned int> indices, std::shared_ptr<MaterialComponent> material, DrawingMode drawingMode = DrawingMode::ISOLATED);

	// Set Mesh as Axis-Aligned Square
	void SetAsAAPlane();
	// Set Mesh as Axis-Aligned Cube with an optional solid color
	void SetAsAACube();

	~TriangleMesh() override;

public:
	std::shared_ptr<MaterialComponent> m_Material;
	unsigned int m_NVAO = 0, m_NVBO = 0;
	float m_RefractiveIndex = 0.0f;

protected:
	// Sets NVAO using connectivity data
	void SetNVAOData() const;
};

class CubemapMesh final : public AbstractTriangleMesh
{
public:
	CubemapMesh();
	explicit CubemapMesh(const std::string& filepath);
	explicit CubemapMesh(const std::vector<std::string>& filepaths);

public:
	std::shared_ptr<TexCube> m_Texture;

private:
	void SetUp();
};

class ScreenMesh final : public AbstractTriangleMesh
{
public:
	explicit ScreenMesh(std::shared_ptr<TexColorBuffer> texture = std::make_shared<TexColorBuffer>());

public:
	std::shared_ptr<TexColorBuffer> m_Texture;
};
