#pragma once

#include <vector>

#include "..\..\Vertex.h"
#include "..\..\Renderer\IndexedVAO.h"

struct RenderableTag {};

struct Renderable
{
	IndexedVAO VAO = IndexedVAO();

	void SetVAO(const std::vector<Vertex>& connectivityData, const std::vector<uint32_t>& indices);

	operator uint32_t& () { return VAO; }
	operator const uint32_t& () const { return VAO; }
};

struct Object3D : Renderable
{
	IndexedVAO NormalVAO = IndexedVAO();

	void SetNVAO(const std::vector<Vertex>& connectivityData);
};
