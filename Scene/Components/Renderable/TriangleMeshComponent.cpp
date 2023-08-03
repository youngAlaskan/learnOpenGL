#include "TriangleMeshComponent.h"

TriangleMeshComponent::TriangleMeshComponent(const std::vector<Vertex>& connectivityData, const std::vector<uint32_t>& indices)
{
	SetVAO(connectivityData, indices);
	SetNVAO(connectivityData);
}
