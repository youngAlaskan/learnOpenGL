#pragma once

#include <glad\glad.h>

#include "IndexedVAO.h"

class Renderer
{
public:
	Renderer() = default;

	static void RenderIndexed(const IndexedVAO& indexedVAO)
	{
		glBindVertexArray(indexedVAO);
		glDrawElements(GL_TRIANGLES, static_cast<int>(indexedVAO.IndexCount), GL_UNSIGNED_INT, nullptr);
	}

	static void RenderLine(const uint32_t& VAO)
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);
	}
};
