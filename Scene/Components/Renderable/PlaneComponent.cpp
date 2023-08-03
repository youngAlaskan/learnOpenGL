#include "PlaneComponent.h"

PlaneComponent::PlaneComponent()
{
	constexpr unsigned int vertexCount = 6, indexCount = 6;
	VAO.IndexCount = indexCount;
	auto connectivityData = std::vector<Vertex>(vertexCount);
	std::vector<uint32_t> indices = {
		0, 2, 1,
		0, 3, 2
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

	connectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	connectivityData.emplace_back(posTR, normal, texCoordTR); // TR
	connectivityData.emplace_back(posTL, normal, texCoordTL); // TL
	connectivityData.emplace_back(posBL, normal, texCoordBL); // BL
	connectivityData.emplace_back(posBR, normal, texCoordBR); // BR
	connectivityData.emplace_back(posTR, normal, texCoordTR); // TR

	SetVAO(connectivityData, indices);
	SetNVAO(connectivityData);
}

