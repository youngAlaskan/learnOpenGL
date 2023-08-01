#pragma once

#include <glad\glad.h>

struct Primitive
{
	unsigned int VAO = 0, VBO = 0, EBO = 0;

	Primitive()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
	}
	~Primitive()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	operator unsigned int& () { return VAO; }
	operator unsigned const int& () const { return VAO; }
};

struct Object3D : Primitive
{
	unsigned int NVAO = 0, NVBO = 0;

	Object3D()
	{
		glGenVertexArrays(1, &NVAO);
		glGenBuffers(1, &NVBO);
	}
	~Object3D()
	{
		glDeleteVertexArrays(1, &NVAO);
		glDeleteBuffers(1, &NVBO);
	}
};
