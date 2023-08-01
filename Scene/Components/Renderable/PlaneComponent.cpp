#include "PlaneComponent.h"

#include <vector>

#include "..\..\Vertex.h"

PlaneComponent::PlaneComponent()
{
	constexpr unsigned int vertexCount = 24;
	auto connectivityData = std::vector<Vertex>(vertexCount);

	auto posBL = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	auto posTL = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
	auto posTR = glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
	auto posBR = glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);

	auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

	auto texCoordBL = glm::vec2(0.0f, 0.0f);
	auto texCoordTL = glm::vec2(0.0f, 1.0f);
	auto texCoordTR = glm::vec2(1.0f, 1.0f);
	auto texCoordBR = glm::vec2(1.0f, 0.0f);

	connectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	connectivityData.emplace_back(posTR, normal, texCoordTR); // TR
	connectivityData.emplace_back(posTL, normal, texCoordTL); // TL
	connectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	connectivityData.emplace_back(posBR, normal, texCoordBR); // BR
	connectivityData.emplace_back(posTR, normal, texCoordTR); // TR

	// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(connectivityData.size() * sizeof(Vertex)), connectivityData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Position))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoord))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(2);

	std::vector<float> normalData;

	for (const auto& vertex : connectivityData)
	{
		normalData.emplace_back(vertex.Position.x);
		normalData.emplace_back(vertex.Position.y);
		normalData.emplace_back(vertex.Position.z);

		normalData.emplace_back(vertex.Position.x + vertex.Normal.x * 0.5f);
		normalData.emplace_back(vertex.Position.y + vertex.Normal.y * 0.5f);
		normalData.emplace_back(vertex.Position.z + vertex.Normal.z * 0.5f);
	}

	glBindVertexArray(NVAO);

	glBindBuffer(GL_ARRAY_BUFFER, NVBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(normalData.size() * sizeof(float)), normalData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

