#pragma once

#include <glm\glm.hpp>

struct Vertex
{
	glm::vec4 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;

	Vertex() = default;

	Vertex(const glm::vec4& pos, const glm::vec3& norm, const glm::vec2& texCoord)
		: Position(pos), Normal(norm), TexCoord(texCoord) {}
	bool operator==(const Vertex& other) const
	{
		return Position == other.Position &&
			Normal == other.Normal &&
			TexCoord == other.TexCoord;
	}

	~Vertex() = default;
};
