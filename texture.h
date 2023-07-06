#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

class Texture
{
public:
	Texture() = default;
	explicit Texture(const int ID) : m_ID(ID) {}
	void Use() const
	{
		glActiveTexture(GL_TEXTURE0 + m_ID);
		glBindTexture(m_Type, m_ID);
	}
	bool operator==(const Texture& other) const { return m_ID == other.m_ID; }
	~Texture() { glDeleteTextures(1, &m_ID); }

public:
	unsigned int m_ID = 0;
	GLenum m_Type = GL_TEXTURE_2D;
};

class Tex2D final : public Texture
{
public:
	explicit Tex2D(const glm::vec3& color);
	explicit Tex2D(const char* filepath);
	static void SetWrap(const GLint sWrap, const GLint tWrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
	}
};

class TexCube final : public Texture
{
public:
	explicit TexCube(const char* filepath);
	explicit TexCube(const char** filepaths);
	explicit TexCube(const glm::vec3& color);
	static void SetWrap(const GLint sWrap, const GLint tWrap, const GLint rWrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, rWrap);
	}
};
