#include "tm.h"

TriangleMesh::TriangleMesh(const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<PointLight>>& pointLights, const std::shared_ptr<DirectionalLight>& directionalLight, const std::shared_ptr<SpotLight>& spotLight, const std::shared_ptr<Camera>& camera)
{
	m_Material = material;
	m_PointLights = pointLights;
	m_DirectionalLight = directionalLight;
	m_SpotLight = spotLight;
	m_Camera = camera;
	m_Shaders.emplace_back("positionColorNormalTex.vert", "variableColor.frag");            // ISOLATED
	m_Shaders.emplace_back("positionColorNormalTex.vert", "objectLitByVariousLights.frag"); // LIT
	m_Shaders.emplace_back("position.vert", "uniformColor.frag");                           // LIGHT
	m_Shaders.emplace_back("position.vert", "uniformColor.frag");                           // NORMALS
}

void TriangleMesh::SetAsAARectangle(int aaPlane, float texCoordScaleFactor)
{
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
	m_ConnectivityData.emplace_back(posBL, colorBL, normal, texCoordBL);

	// Top Left
	// --------
	m_ConnectivityData.emplace_back(posTL, colorTL, normal, texCoordTL);

	// Top Right
	// ---------
	m_ConnectivityData.emplace_back(posTR, colorTR, normal, texCoordTR);

	// Bottom Right
	// ------------
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
		normalData.emplace_back(vertex.m_Position.x);
		normalData.emplace_back(vertex.m_Position.y);
		normalData.emplace_back(vertex.m_Position.z);

		normalData.emplace_back(vertex.m_Position.x + vertex.m_Normal.x);
		normalData.emplace_back(vertex.m_Position.y + vertex.m_Normal.y);
		normalData.emplace_back(vertex.m_Position.z + vertex.m_Normal.z);
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

void TriangleMesh::SetAsAACube()
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

	// Normals
	// ------------------
	std::vector<float> normalData;

	for (const auto& vertex : m_ConnectivityData)
	{
		normalData.emplace_back(vertex.m_Position.x);
		normalData.emplace_back(vertex.m_Position.y);
		normalData.emplace_back(vertex.m_Position.z);

		normalData.emplace_back(vertex.m_Position.x + vertex.m_Normal.x);
		normalData.emplace_back(vertex.m_Position.y + vertex.m_Normal.y);
		normalData.emplace_back(vertex.m_Position.z + vertex.m_Normal.z);
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

void TriangleMesh::SetAsAACube(glm::vec3 color)
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

	// Normals
	// ------------------
	std::vector<float> normalData;

	for (const auto& vertex : m_ConnectivityData)
	{
		normalData.emplace_back(vertex.m_Position.x);
		normalData.emplace_back(vertex.m_Position.y);
		normalData.emplace_back(vertex.m_Position.z);

		normalData.emplace_back(vertex.m_Position.x + vertex.m_Normal.x);
		normalData.emplace_back(vertex.m_Position.y + vertex.m_Normal.y);
		normalData.emplace_back(vertex.m_Position.z + vertex.m_Normal.z);
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

void TriangleMesh::Draw() const
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
	case LIGHT:
		shader.SetVec3("color", glm::vec3(1.0f));
		break;
	default:
		break;
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	CheckForErrors("ERROR::TM::DRAW: ");
}

void TriangleMesh::DrawNormals(const glm::vec3& color) const
{
	m_Shaders[NORMALS].Use();
	m_Shaders[NORMALS].SetMat4("model", m_Model);
	m_Shaders[NORMALS].SetMat4("view", m_View);
	m_Shaders[NORMALS].SetMat4("proj", m_Proj);

	m_Shaders[NORMALS].SetVec3("color", color);

	glBindVertexArray(m_NVAO);
	glDrawArrays(GL_LINES, 0, 72);
	glBindVertexArray(0);

	CheckForErrors("ERROR::DRAW_NORMALS: ");
}

bool TriangleMesh::operator==(const TriangleMesh& other) const
{
	return m_ConnectivityData == other.m_ConnectivityData &&
		m_Indices == other.m_Indices &&
		m_Shaders == other.m_Shaders &&
		m_TrianglesN == other.m_TrianglesN &&
		m_VerticesN == other.m_VerticesN &&
		m_ShaderProgramNormals == other.m_ShaderProgramNormals &&
		m_VAO == other.m_VAO &&
		m_VBO == other.m_VBO &&
		m_EBO == other.m_EBO &&
		m_NVAO == other.m_NVAO &&
		m_NVBO == other.m_NVBO &&
		m_NEBO == other.m_NEBO &&
		m_Model == other.m_Model &&
		m_View == other.m_View &&
		m_Proj == other.m_Proj;
}

TriangleMesh::~TriangleMesh()
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
