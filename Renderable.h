#pragma once

#include "Shader.h"

class Renderable
{
public:
	Renderable() = default;

	virtual void Draw() const;

	void SetModel(const glm::mat4& model) { m_Model = model; }
	void SetView(const glm::mat4& view) { m_View = view; }
	void SetProj(const glm::mat4& proj) { m_Proj = proj; }
	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}

	virtual ~Renderable() = default;

protected:
	unsigned int m_VAO = 0, m_VBO = 0;
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);
};
