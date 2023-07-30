#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include "stb_image.h"

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

	virtual void Use(int index) const = 0;

	bool operator==(const Texture& other) const
	{
		return m_ID == other.m_ID;
	}

	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }

	virtual ~Texture() { glDeleteTextures(1, &m_ID); }

public:
	GLuint m_ID = 0;
};

class Tex2D final : public Texture
{
public:
	explicit Tex2D(const glm::vec4 color, std::string tag = std::string())
		: m_Tag(std::move(tag))
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

	explicit Tex2D(const std::string& filepath, std::string tag = std::string())
		: m_Tag(std::move(tag)), m_Path(filepath)
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
		int width, height, channelsN;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channelsN, 0);
		if (data)
		{
			const GLint format = channelsN == 1 ? GL_RED : (channelsN == 4 ? GL_RGBA : GL_RGB);

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

	void SetTag(std::string tag) { m_Tag = std::move(tag); }

	void Use(const int index = 0) const override
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

public:
	std::string m_Tag = std::string();
	std::string m_Path = std::string();
};

class TexCube final : public Texture
{
public:
	explicit TexCube(const std::string& filepath)
	{
		m_Paths.emplace_back(filepath);

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, channelsN;
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channelsN, 0);
		if (data)
		{
			const GLint format = channelsN == 1 ? GL_RED : (channelsN == 4 ? GL_RGBA : GL_RGB);

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

	explicit TexCube(const std::vector<std::string>& filepaths)
		: m_Paths(filepaths)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

		int width, height, channelsN;
		for (unsigned int i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load(filepaths[i].c_str(), &width, &height, &channelsN, 0);
			if (data)
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGBA,
					width,
					height,
					0,
					channelsN == 1 ? GL_RED : (channelsN == 4 ? GL_RGBA : GL_RGB),
					GL_UNSIGNED_BYTE,
					data
				);
			else
				std::cout << "Failed to load cube map texture" << std::endl;

			stbi_image_free(data);
		}

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	void Use(const int index = 0) const override
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
	}

public:
	std::vector<std::string> m_Paths = std::vector<std::string>();
};

class TexColorBuffer final : public Texture
{
public:
	explicit TexColorBuffer() = default;

	explicit TexColorBuffer(const unsigned int width, const unsigned int height)
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}

	void Use(const int index = 0) const override
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}
};
