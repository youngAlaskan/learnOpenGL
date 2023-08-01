#pragma once

#include <glad/glad.h>

class Renderbuffer
{
public:
	Renderbuffer() { glGenRenderbuffers(1, &m_ID); }

	Renderbuffer(const unsigned int width, const unsigned int height)
		: m_Width(width), m_Height(height) {
		glGenRenderbuffers(1, &m_ID);
	}

	Renderbuffer(const GLenum internalFormat, const unsigned int width, const unsigned int height)
		: m_Width(width), m_Height(height)
	{
		glGenRenderbuffers(1, &m_ID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_ID);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_Width, m_Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }

public:
	unsigned int m_ID = 0;
	unsigned int m_Width = 0, m_Height = 0;
};
