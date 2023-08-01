#pragma once

#include <memory>
#include <glad\glad.h>
#include <glm\glm.hpp>

#include <string>
#include <vector>

class Texture
{
public:
	Texture()
		{ glGenTextures(1, &m_ID); }
	explicit Texture(const int ID)
		{ m_ID = ID; }
	virtual void Use(int index) const = 0;
	bool operator==(const Texture& other) const
		{ return m_ID == other.m_ID; }
	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }
	virtual ~Texture()
		{ glDeleteTextures(1, &m_ID); }

public:
	GLuint m_ID = 0;
};

class Tex2D final : public Texture
{
public:
	explicit Tex2D(glm::vec4 color, std::string tag = std::string());
	explicit Tex2D(const std::string& filepath, std::string tag = std::string());
	static void SetWrap(GLint sWrap, const GLint tWrap);
	void SetTag(std::string tag)
		{ m_Tag = std::move(tag); }
	void Use(int index = 0) const override;

public:
	std::string m_Tag = std::string();
	std::string m_Path = std::string();
};

struct Tex2DComponent
{
	std::shared_ptr<Tex2D> Texture;
};

class TexCube final : public Texture
{
public:
	explicit TexCube(const std::string& filepath);
	explicit TexCube(const std::vector<std::string>& filepaths);
	explicit TexCube(glm::vec4 color);
	static void SetWrap(GLint sWrap, GLint tWrap, GLint rWrap);
	void Use(int index = 0) const override;

public:
	std::vector<std::string> m_Paths = std::vector<std::string>();
};

struct TexCubeComponent
{
	std::shared_ptr<TexCube> Texture;
};

class TexColorBuffer final : public Texture
{
public:
	explicit TexColorBuffer() = default;
	explicit TexColorBuffer(unsigned int width, unsigned int height);
	void Use(int index = 0) const override;
};

struct TexColorBufferComponent
{
	std::shared_ptr<TexColorBufferComponent> Texture;
};
