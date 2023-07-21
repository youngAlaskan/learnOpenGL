#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <iostream>
#include <string>

class Texture
{
public:
	Texture() { glGenTextures(1, &m_ID); }

	explicit Texture(const int ID)
	{
		m_ID = ID;
	}

	void SetTagDiffuse() { m_Tag = std::string("diffuse"); }
	void SetTagSpecular() { m_Tag = std::string("specular"); }
	void SetTagEmissive() { m_Tag = std::string("emissive"); }
	void SetTagCustom(const std::string& name) { m_Tag = name; }

	virtual void Use() const = 0;

	bool operator==(const Texture& other) const
	{
		return m_ID == other.m_ID &&
			m_Tag == other.m_Tag &&
			m_Path == other.m_Path;
	}

	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }

	~Texture() {
		glDeleteTextures(1, &m_ID);
	}

public:
	GLuint m_ID = 0;
	std::string m_Tag = std::string();
	std::string m_Path = std::string();
};

class Tex2D final : public Texture
{
public:
	explicit Tex2D(const glm::vec4 color)
	{
		// load and create a texture 
		// -------------------------
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const auto data = new unsigned char[4 * sizeof(unsigned char)];
		data[0] = static_cast<unsigned char>(color.r * 255.0f);
		data[1] = static_cast<unsigned char>(color.g * 255.0f);
		data[2] = static_cast<unsigned char>(color.b * 255.0f);
		data[3] = static_cast<unsigned char>(color.a * 255.0f);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		delete[] data;

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	explicit Tex2D(const char* filepath)
	{
		// load and create a texture 
		// -------------------------
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// load image, create texture and generate mipmaps
		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
		if (data)
		{
			GLint format = GL_RGB;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 4)
				format = GL_RGBA;


			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

	void Use() const override
	{
		glActiveTexture(GL_TEXTURE0 + m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}
};

class TexCube final : public Texture
{
public:
	explicit TexCube(const char* filepath)
	{
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

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0,
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

	explicit TexCube(const char** filepaths)
	{
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
					0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data
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

	explicit TexCube(const glm::vec4 color)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		const auto data = new unsigned char[4 * sizeof(unsigned char)];
		data[0] = static_cast<unsigned char>(color.r * 255.0f);
		data[1] = static_cast<unsigned char>(color.g * 255.0f);
		data[2] = static_cast<unsigned char>(color.b * 255.0f);
		data[3] = static_cast<unsigned char>(color.a * 255.0f);

		constexpr GLint format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 1, 1, 0,
			format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 1, 1, 0,
			format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 1, 1, 0,
			format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 1, 1, 0,
			format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 1, 1, 0,
			format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 1, 1, 0,
			format, GL_UNSIGNED_BYTE, data);

		delete[] data;
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	static void SetWrap(const GLint sWrap, const GLint tWrap, const GLint rWrap) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, tWrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, rWrap);
	}

	void Use() const override
	{
		glActiveTexture(GL_TEXTURE0 + m_ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
	}
};

class TexColorBuffer final : public Texture
{
public:
	explicit TexColorBuffer() = default;

	explicit TexColorBuffer(const unsigned int width, const unsigned int height)
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);
		glObjectLabel(GL_TEXTURE, m_ID, static_cast<GLsizei>(m_Path.size()), m_Path.c_str());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}

	void Use() const override
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}
};
