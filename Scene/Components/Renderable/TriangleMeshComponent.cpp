#include "TriangleMeshComponent.h"

TriangleMeshComponent::TriangleMeshComponent(const std::vector<Vertex>& connectivityData, const std::vector<unsigned>& indices)
{
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
