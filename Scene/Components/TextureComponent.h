#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <string>
#include <vector>

class TextureComponent
{
public:
	TextureComponent()
		{ glGenTextures(1, &m_ID); }
	explicit TextureComponent(const int ID)
		{ m_ID = ID; }
	virtual void Use(int index) const = 0;
	bool operator==(const TextureComponent& other) const
		{ return m_ID == other.m_ID; }
	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }
	virtual ~TextureComponent()
		{ glDeleteTextures(1, &m_ID); }

public:
	GLuint m_ID = 0;
};

class Tex2DComponent final : public TextureComponent
{
public:
	explicit Tex2DComponent(glm::vec4 color, std::string tag = std::string());
	explicit Tex2DComponent(const std::string& filepath, std::string tag = std::string());
	static void SetWrap(GLint sWrap, const GLint tWrap);
	void SetTag(std::string tag)
		{ m_Tag = std::move(tag); }
	void Use(int index = 0) const override;

public:
	std::string m_Tag = std::string();
	std::string m_Path = std::string();
};

class TexCubeComponent final : public TextureComponent
{
public:
	explicit TexCubeComponent(const std::string& filepath);
	explicit TexCubeComponent(const std::vector<std::string>& filepaths);
	explicit TexCubeComponent(glm::vec4 color);
	static void SetWrap(GLint sWrap, GLint tWrap, GLint rWrap);
	void Use(int index = 0) const override;

public:
	std::vector<std::string> m_Paths = std::vector<std::string>();
};

class TexColorBufferComponent final : public TextureComponent
{
public:
	explicit TexColorBufferComponent() = default;
	explicit TexColorBufferComponent(unsigned int width, unsigned int height);
	void Use(int index = 0) const override;
};
