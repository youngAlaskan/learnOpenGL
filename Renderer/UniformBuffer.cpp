﻿#include "UniformBuffer.h"

void UniformBuffer::SetData(const GLsizeiptr size, const void* data, const GLenum usage) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SetSubData(const GLintptr offset, const GLsizeiptr size, const GLvoid* data) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::BindDataRange(const GLuint index, const GLintptr offset, const GLsizeiptr size)
{
	m_Index = index;
	glBindBufferRange(GL_UNIFORM_BUFFER, index, m_ID, offset, size);
}

void UniformBuffer::BindData(const GLuint index)
{
	m_Index = index;
	glBindBufferBase(GL_UNIFORM_BUFFER, index, m_ID);
}
