#pragma once

#include <algorithm>
#include <glm\glm.hpp>

class LightComponent
{
public:
	LightComponent() = default;
	void SetColor(const glm::vec4 color)
		{ m_Color = color; }
	void SetCoeffs(const float kA, const float kD, const float kS);

public:
	glm::vec4 m_Color = glm::vec4(1.0f);
	float m_KA = 1.0f, m_KD = 1.0f, m_KS = 1.0f;
};

class PointLightComponent final : public LightComponent
{
public:
	PointLightComponent() { SetIndex(); }
	explicit PointLightComponent(const glm::vec4 position)
		: m_Pos(position) { SetIndex(); }
	explicit PointLightComponent(float distance);
	explicit PointLightComponent(float distance, float kA, float kD, float kS);
	explicit PointLightComponent(glm::vec4 position, float distance, float kA, float kD, float kS);
	void SetPos(const glm::vec4 pos)
		{ m_Pos = pos; }
	static int GetCount()
		{ return s_Count; }
	~PointLightComponent()
		{ s_Count--; }

public:
	glm::vec4 m_Pos = glm::vec4(0.0f);
	float m_Distance = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
	int m_Index = 0;

private:
	void SetIndex()
		{ m_Index = s_Count++; }

private:
	inline static int s_Count = 0;
};

class DirectionalLightComponent final : public LightComponent
{
public:
	DirectionalLightComponent() { SetIndex(); }
	explicit DirectionalLightComponent(const glm::vec3 direction)
		: m_Direction(direction) { SetIndex(); }
	explicit DirectionalLightComponent(const glm::vec3 direction, const float kA, const float kD, const float kS)
		: m_Direction(direction)
	{
		SetCoeffs(kA, kD, kS);
		SetIndex();
	}
	static int GetCount()
	{
		return s_Count;
	}
	~DirectionalLightComponent()
	{
		s_Count--;
	}

public:
	glm::vec3 m_Direction = glm::vec3(0.0f);
	int m_Index = 0;

private:
	void SetIndex()
	{
		m_Index = s_Count++;
	}

private:
	inline static int s_Count = 0;
};

class SpotLightComponent final : public LightComponent
{
public:
	SpotLightComponent() { SetIndex(); }
	explicit SpotLightComponent(float theta);
	explicit SpotLightComponent(float theta, float kA, float kD, float kS);
	explicit SpotLightComponent(glm::vec3 direction, float theta, float kA, float kD, float kS);
	void SetCutOff(float thetaDegrees);
	void Update(const glm::vec4& position, const glm::vec3& direction);
	static int GetCount()
	{
		return s_Count;
	}
	~SpotLightComponent()
	{
		s_Count--;
	}

public:
	glm::vec4 m_Pos = glm::vec4(0.0f);
	float m_Distance = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
	glm::vec3 m_Direction = glm::vec3(0.0f);
	float m_InnerCutOff = 0.0f, m_OuterCutOff = 0.0f;
	int m_Index = 0;

private:
	void SetIndex()
	{
		m_Index = s_Count++;
	}

private:
	inline static int s_Count = 0;
};
