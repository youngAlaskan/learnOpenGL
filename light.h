#pragma once

#include "glm/glm.hpp"
#include "Shader.h"
#include "utils.h"

struct PointLight
{
public:
	PointLight() = default;

	PointLight(const PointLight& pointLight) = default;

	explicit PointLight(const float distance)
		: m_Pos(glm::vec3(0.0f)), m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance)) {}

	void SetPos(const glm::vec3 pos) { m_Pos = pos; }

	void SendToShader(const Shader& shader) const;

	bool operator==(const PointLight& other) const;

	~PointLight() = default;

public:
	glm::vec3 m_Pos = glm::vec3(0.0f);
	float m_Distance = 0.0f;
	glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_KA = 0.0f, m_KD = 0.0f, m_KS = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
	int m_Index = 0;
};

struct DirectionalLight
{
public:
	DirectionalLight() = default;

	DirectionalLight(const DirectionalLight& directionalLight) = default;

	void SendToShader(const Shader& shader) const;

	bool operator==(const DirectionalLight& other) const;

	~DirectionalLight() = default;

public:
	glm::vec3 m_Direction = glm::vec3(0.0f);
	glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_KA = 0.0f, m_KD = 0.0f, m_KS = 0.0f;
};

struct SpotLight
{
public:
	SpotLight() = default;

	SpotLight(const SpotLight& spotLight) = default;

	explicit SpotLight(const float theta)
	: m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
	m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f)))) {}

	void SetCutOff(const float thetaDegrees)
	{
		m_InnerCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 0.875f, 0.0f)));
		m_OuterCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 1.125f, 0.0f)));
	}

	void SendToShader(const Shader& shader) const;

	bool operator==(const SpotLight& other) const;

	~SpotLight() = default;

public:
	glm::vec3 m_Pos = glm::vec3(0.0f);
	glm::vec3 m_Direction = glm::vec3(0.0f);
	glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_KA = 0.0f, m_KD = 0.0f, m_KS = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
	float m_InnerCutOff = 0.0f, m_OuterCutOff = 0.0f;
};
