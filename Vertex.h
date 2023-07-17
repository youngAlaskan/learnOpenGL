#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec4 Position;
	glm::vec4 Color;
	glm::vec3 Normal;
	glm::vec2 TexCoord;

	Vertex() = default;

	Vertex(const glm::vec4& pos, const glm::vec4& col, const glm::vec3& norm, const glm::vec2& texCoord)
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
