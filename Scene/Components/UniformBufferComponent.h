#pragma once

#include "glad\glad.h"

class UniformBufferComponent
{
public:
	explicit UniformBufferComponent()
		{ glGenBuffers(1, &m_ID); }

	// Sets the buffer data to the data provided. Can be used to allocate space
	void SetData(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW) const;
	// Sets a range of buffer data to the given data
	void SetSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data) const;
	// Binds a range of the buffer to a specific index (binding point)
	void BindDataRange(GLuint index, GLintptr offset, GLsizeiptr size);
	// Binds the entire buffer to a specific index (binding point)
	void BindData(GLuint index);

public:
	unsigned int m_ID = 0; // Buffer ID
	unsigned int m_Index = 0; // Binding Point
};
