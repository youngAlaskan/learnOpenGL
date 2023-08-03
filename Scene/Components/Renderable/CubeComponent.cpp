#include "CubeComponent.h"

CubeComponent::CubeComponent()
{
	constexpr unsigned int vertexCount = 24, indexCount = 36;
	VAO.IndexCount = indexCount;
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

	SetVAO(connectivityData, indices);
	SetNVAO(connectivityData);
}
