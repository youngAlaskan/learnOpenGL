#include "TM.h"

void AbstractTriangleMesh::SetVAOData() const
{
	if (m_ConnectivityData.empty()) return;

	// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(m_ConnectivityData.size() * sizeof(Vertex)), m_ConnectivityData.data(), GL_STATIC_DRAW);

	if (!m_Indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(m_Indices.size() * sizeof(unsigned int)), m_Indices.data(), GL_STATIC_DRAW);
	}

	// TODO: Update size parameters to be generalized
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Position))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoord))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(2);

	// Unbinds
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TriangleMesh::TriangleMesh()
{
	m_Type = DrawableType::TRIANGLE_MESH;
	glGenVertexArrays(1, &m_NVAO);
	glGenBuffers(1, &m_NVBO);
}

TriangleMesh::TriangleMesh(std::shared_ptr<Material> material, const DrawingMode drawingMode)
	: m_Material(std::move(material))
{
	m_DrawingMode = drawingMode;
	m_Type = DrawableType::TRIANGLE_MESH;
	glGenVertexArrays(1, &m_NVAO);
	glGenBuffers(1, &m_NVBO);
}

TriangleMesh::TriangleMesh(std::vector<Vertex> connectivityData, std::vector<unsigned int> indices, std::shared_ptr<Material> material, const DrawingMode drawingMode)
	: m_Material(std::move(material))
{
	m_ConnectivityData = std::move(connectivityData);
	m_Indices = std::move(indices);
	m_DrawingMode = drawingMode;
	m_Type = DrawableType::TRIANGLE_MESH;
	glGenVertexArrays(1, &m_NVAO);
	glGenBuffers(1, &m_NVBO);
	SetVAOData();
	SetNVAOData();
}

// Set Mesh as Axis-Aligned Plane
void TriangleMesh::SetAsAAPlane()
{
	m_VertexCount = 6;
	m_TriangleCount = 2;
	m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);
	m_Indices = {
		0, 1, 2,
		3, 4, 5
	};

	auto posBL = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	auto posTL = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
	auto posTR = glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
	auto posBR = glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);

	auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

	auto texCoordBL = glm::vec2(0.0f, 0.0f);
	auto texCoordTL = glm::vec2(0.0f, 1.0f);
	auto texCoordTR = glm::vec2(1.0f, 1.0f);
	auto texCoordBR = glm::vec2(1.0f, 0.0f);

	m_ConnectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	m_ConnectivityData.emplace_back(posTR, normal, texCoordTR); // TR
	m_ConnectivityData.emplace_back(posTL, normal, texCoordTL); // TL
	m_ConnectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	m_ConnectivityData.emplace_back(posBR, normal, texCoordBR); // BR
	m_ConnectivityData.emplace_back(posTR, normal, texCoordTR); // TR

	SetVAOData();
	SetNVAOData();
}

// Set Mesh as Axis-Aligned Cube
void TriangleMesh::SetAsAACube()
{
	constexpr unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
	m_TriangleCount = 12;
	m_VertexCount = 3 * m_TriangleCount;
	m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);
	m_Indices = {
		// Front
		BLF, TRF, TLF,
		BLF, BRF, TRF,

		// Right
		BRF, TRB, TRF,
		BRF, BRB, TRB,

		// Back
		BRB, TLB, TRB,
		BRB, BLB, TLB,

		// Left
		BLB, TLF, TLB,
		BLB, BLF, TLF,

		// Bottom
		BLB, BRF, BLF,
		BLB, BRB, BRF,

		// Top
		TLF, TRB, TLB,
		TLF, TRF, TRB
	};

	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	positions.emplace_back(-0.5f, -0.5f, 0.5f, 1.0f); // BLF
	positions.emplace_back(-0.5f, 0.5f, 0.5f, 1.0f); // TLF
	positions.emplace_back(0.5f, 0.5f, 0.5f, 1.0f); // TRF
	positions.emplace_back(0.5f, -0.5f, 0.5f, 1.0f); // BRF
	positions.emplace_back(-0.5f, -0.5f, -0.5f, 1.0f); // BLB
	positions.emplace_back(-0.5f, 0.5f, -0.5f, 1.0f); // TLB
	positions.emplace_back(0.5f, 0.5f, -0.5f, 1.0f); // TRB
	positions.emplace_back(0.5f, -0.5f, -0.5f, 1.0f); // BRB

	normals.emplace_back(0.0f, 0.0f, 1.0f); // POS Z
	normals.emplace_back(1.0f, 0.0f, 0.0f); // POS X
	normals.emplace_back(0.0f, 0.0f, -1.0f); // NEG Z
	normals.emplace_back(-1.0f, 0.0f, 0.0f); // NEG X
	normals.emplace_back(0.0f, -1.0f, 0.0f); // NEG Y
	normals.emplace_back(0.0f, 1.0f, 0.0f); // POS Y

	texCoords.emplace_back(0.0f, 0.0f);
	texCoords.emplace_back(0.0f, 1.0f);
	texCoords.emplace_back(1.0f, 1.0f);
	texCoords.emplace_back(1.0f, 0.0f);

	const auto texCoordIndices = std::vector<int>{ 0, 2, 1, 0, 3, 2 };

	for (unsigned int i = 0; i < m_VertexCount; i++)
		m_ConnectivityData.emplace_back(positions[m_Indices[i]], normals[i / 6], texCoords[texCoordIndices[i % 6]]);

	for (unsigned int i = 0; i < m_VertexCount; i++)
		m_Indices[i] = i;

	SetVAOData();
	SetNVAOData();
}

TriangleMesh::~TriangleMesh()
{
	glDeleteVertexArrays(1, &m_NVAO);
	glDeleteBuffers(1, &m_NVBO);
}

// Sets NVAO using connectivity data
void TriangleMesh::SetNVAOData() const
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
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(normalData.size() * sizeof(float)), normalData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

CubemapMesh::CubemapMesh()
{
	SetUp();
}

CubemapMesh::CubemapMesh(const std::string& filepath)
{
	m_Texture = std::make_shared<TexCube>(filepath);
	SetUp();
}

CubemapMesh::CubemapMesh(const std::vector<std::string>& filepaths)
{
	m_Texture = std::make_shared<TexCube>(filepaths);
	SetUp();
}

void CubemapMesh::SetUp()
{
	m_Type = DrawableType::CUBE_MAP;
	constexpr unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
	m_TriangleCount = 12;
	m_VertexCount = 3 * m_TriangleCount;
	m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);
	m_Indices = {
		// Front
		BLF, TRF, TLF,
		BLF, BRF, TRF,

		// Right
		BRF, TRB, TRF,
		BRF, BRB, TRB,

		// Back
		BRB, TLB, TRB,
		BRB, BLB, TLB,

		// Left
		BLB, TLF, TLB,
		BLB, BLF, TLF,

		// Bottom
		BLB, BRF, BLF,
		BLB, BRB, BRF,

		// Top
		TLF, TRB, TLB,
		TLF, TRF, TRB
	};

	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	positions.emplace_back(-0.5f, -0.5f, 0.5f, 1.0f); // BLF
	positions.emplace_back(-0.5f, 0.5f, 0.5f, 1.0f); // TLF
	positions.emplace_back(0.5f, 0.5f, 0.5f, 1.0f); // TRF
	positions.emplace_back(0.5f, -0.5f, 0.5f, 1.0f); // BRF
	positions.emplace_back(-0.5f, -0.5f, -0.5f, 1.0f); // BLB
	positions.emplace_back(-0.5f, 0.5f, -0.5f, 1.0f); // TLB
	positions.emplace_back(0.5f, 0.5f, -0.5f, 1.0f); // TRB
	positions.emplace_back(0.5f, -0.5f, -0.5f, 1.0f); // BRB

	normals.emplace_back(0.0f, 0.0f, 1.0f); // POS Z
	normals.emplace_back(1.0f, 0.0f, 0.0f); // POS X
	normals.emplace_back(0.0f, 0.0f, -1.0f); // NEG Z
	normals.emplace_back(-1.0f, 0.0f, 0.0f); // NEG X
	normals.emplace_back(0.0f, -1.0f, 0.0f); // NEG Y
	normals.emplace_back(0.0f, 1.0f, 0.0f); // POS Y

	texCoords.emplace_back(0.0f, 0.0f);
	texCoords.emplace_back(0.0f, 1.0f);
	texCoords.emplace_back(1.0f, 1.0f);
	texCoords.emplace_back(1.0f, 0.0f);

	const auto texCoordIndices = std::vector<int>{ 0, 2, 1, 0, 3, 2 };

	for (unsigned int i = 0; i < m_VertexCount; i++)
		m_ConnectivityData.emplace_back(positions[m_Indices[i]], normals[i / 6], texCoords[texCoordIndices[i % 6]]);

	for (unsigned int i = 0; i < m_VertexCount; i++)
		m_Indices[i] = i;

	SetVAOData();
}

ScreenMesh::ScreenMesh(std::shared_ptr<TexColorBuffer> texture)
	: m_Texture(std::move(texture))
{
	m_Type = DrawableType::SCREEN;
	m_VertexCount = 6;
	m_TriangleCount = 2;
	m_ConnectivityData.reserve(sizeof(Vertex) * m_VertexCount);
	m_Indices = {
		0, 1, 2,
		3, 4, 5
	};

	auto posBL = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
	auto posTL = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
	auto posTR = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	auto posBR = glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);

	auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

	auto texCoordBL = glm::vec2(0.0f, 0.0f);
	auto texCoordTL = glm::vec2(0.0f, 1.0f);
	auto texCoordTR = glm::vec2(1.0f, 1.0f);
	auto texCoordBR = glm::vec2(1.0f, 0.0f);

	m_ConnectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	m_ConnectivityData.emplace_back(posTR, normal, texCoordTR); // TR
	m_ConnectivityData.emplace_back(posTL, normal, texCoordTL); // TL
	m_ConnectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	m_ConnectivityData.emplace_back(posBR, normal, texCoordBR); // BR
	m_ConnectivityData.emplace_back(posTR, normal, texCoordTR); // TR

	SetVAOData();
}
