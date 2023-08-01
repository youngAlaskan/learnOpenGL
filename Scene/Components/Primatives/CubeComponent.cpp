#include "CubeComponent.h"

#include <vector>
#include <glad\glad.h>

#include "..\..\Vertex.h"

CubeComponent::CubeComponent()
{
	constexpr unsigned int vertexCount = 24, indexCount = 36;
	auto connectivityData = std::vector<Vertex>(vertexCount);
	auto indices = std::vector<unsigned int>(indexCount);

	indices = {
		0, 2, 1,
		0, 3, 2,

		4, 6, 5,
		4, 7, 6,

		8, 10, 9,
		8, 11, 10,

		12, 14, 13,
		12, 15, 14,

		16, 18, 17,
		16, 19, 18,

		20, 22, 21,
		20, 23, 22,
	};

	// +Z
	connectivityData.emplace_back(glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, -0.5f, 0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));

	// +X
	connectivityData.emplace_back(glm::vec4(0.5f, -0.5f, 0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));

	// -Z
	connectivityData.emplace_back(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));

	// -X
	connectivityData.emplace_back(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));

	// +Y
	connectivityData.emplace_back(glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));

	// -Y
	connectivityData.emplace_back(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	connectivityData.emplace_back(glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, -0.5f, 0.5f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	connectivityData.emplace_back(glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f));

	// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(connectivityData.size() * sizeof(Vertex)), connectivityData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

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
