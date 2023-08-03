#pragma once

#include <glad\glad.h>

struct IndexedVAO
{
	uint32_t* VAO = nullptr;
	uint32_t IndexCount = 0;

	IndexedVAO()
	{
		glGenVertexArrays(1, VAO);
	}

	~IndexedVAO()
	{
		glDeleteVertexArrays(1, VAO);
		delete VAO;
	}

	operator uint32_t& () { return *VAO; }
	operator const uint32_t& () const { return *VAO; }
	operator uint32_t* () { return VAO; }
	operator const uint32_t* () const { return VAO; }
};
