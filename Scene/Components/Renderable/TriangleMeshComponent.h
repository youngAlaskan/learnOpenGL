#pragma once

#include <vector>

#include "..\..\Vertex.h"

#include "Renderable.h"

struct TriangleMeshComponent : Object3D
{
	TriangleMeshComponent(const std::vector<Vertex>& connectivityData, const std::vector<unsigned int>& indices);
};
