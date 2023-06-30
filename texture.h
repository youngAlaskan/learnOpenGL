#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <iostream>

class Texture
{
public:
	Texture() = default;

	explicit Texture(const int ID)
	{
		m_ID = ID;
	}

	void Use() const
	{
		glActiveTexture(GL_TEXTURE0 + m_ID);
		glBindTexture(m_Type, m_ID);
	}

	bool operator==(const Texture& other) const
	{
		return m_ID == other.m_ID;
	}

	virtual ~Texture() {
		glDeleteTextures(1, &m_ID);
	}

public:
	unsigned int m_ID = 0;
	GLenum m_Type = GL_TEXTURE_2D;
};

class Tex2D final : public Texture
{
public:
	explicit Tex2D(const glm::vec3 color)
	{
		m_Type = GL_TEXTURE_2D;

		constexpr int width = 2, height = 2;
		// load and create a texture 
		// -------------------------
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const auto data = new unsigned char[width * height * 3];

		for (int uv = 0; uv < width * height * 3; uv += 3)
		{
			data[uv + 0] = static_cast<unsigned char>(color.r);
			data[uv + 1] = static_cast<unsigned char>(color.g);
			data[uv + 2] = static_cast<unsigned char>(color.b);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	explicit Tex2D(const char* filepath)
	{
		m_Type = GL_TEXTURE_2D;

		// load and create a texture 
		// -------------------------
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// load image, create texture and generate mipmaps
		int width, height, nrChannels;

		unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
		if (data)
		{
			GLint format = GL_RGB;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 4)
				format = GL_RGBA;


			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	static void SetWrap(const GLint sWrap, const GLint tWrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
	}
};

class TexCube final : public Texture
{
public:
	explicit TexCube(const char* filepath) {
		m_Type = GL_TEXTURE_CUBE_MAP;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, channelsN;
		unsigned char* data = stbi_load(filepath, &width, &height, &channelsN, 0);
		if (data)
		{
			GLint format = GL_RGB;
			if (channelsN == 1)
				format = GL_RED;
			else if (channelsN == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	explicit TexCube(const char** filepaths) {
		m_Type = GL_TEXTURE_CUBE_MAP;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, channelsN;
		for (unsigned int i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load(filepaths[i], &width, &height, &channelsN, 0);
			if (data) {
				GLint format = GL_RGB;
				if (channelsN == 1)
					format = GL_RED;
				else if (channelsN == 4)
					format = GL_RGBA;
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
			}
			else
			{
				std::cout << "Failed to load cube map texture" << std::endl;
			}

			stbi_image_free(data);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	static void SetWrap(const GLint sWrap, const GLint tWrap, const GLint rWrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, rWrap);
	}
};
