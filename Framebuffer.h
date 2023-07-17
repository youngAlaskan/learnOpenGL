#pragma once

#include <glad/glad.h>

#include "Texture.h"
#include "Renderbuffer.h"

class Framebuffer
{
public:
	Framebuffer() { glGenFramebuffers(1, &m_ID); }

	Framebuffer(TexColorBuffer colorBuffer, Renderbuffer renderbuffer)
	{
		glGenFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

		CheckFrameBufferStatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Use() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	}

	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }

private:
	static void CheckFrameBufferStatus()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "ERROR::FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
	}

private:
	unsigned int m_ID = 0;
};
