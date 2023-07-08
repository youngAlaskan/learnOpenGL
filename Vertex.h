#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec3 Normal;
	glm::vec3 TexCoord;

	Vertex() = default;

	Vertex(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& norm, const glm::vec3& texCoord)
		: Position(pos), Color(col), Normal(norm), TexCoord(texCoord) {}
	bool operator==(const Vertex& other) const
	{
		return Position == other.Position &&
			Color == other.Color &&
			Normal == other.Normal &&
			TexCoord == other.TexCoord;
	}
	~Vertex() = default;
};
