#include "Light.h"

#include <algorithm>

void Light::SetCoeffs(const float kA, const float kD, const float kS)
{
	m_KA = kA;
	m_KD = kD;
	m_KS = kS;
}

PointLight::PointLight(const float distance)
	: m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance))
{
	SetIndex();
}

PointLight::PointLight(const float distance, const float kA, const float kD, const float kS)
	: m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance))
{
	SetCoeffs(kA, kD, kS);
	SetIndex();
}

PointLight::PointLight(const glm::vec4 position, const float distance, const float kA, const float kD, const float kS)
	: m_Pos(position), m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance))
{
	SetCoeffs(kA, kD, kS);
	SetIndex();
}

SpotLight::SpotLight(const float theta)
		: m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
		m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f))))
{
	SetIndex();
}

SpotLight::SpotLight(const float theta, const float kA, const float kD, const float kS)
	: m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
	m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f))))
{
	SetCoeffs(kA, kD, kS);
	SetIndex();
}

SpotLight::SpotLight(const glm::vec3 direction, const float theta, const float kA, const float kD, const float kS)
	: m_Direction(direction),
	m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
	m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f))))
{
	SetCoeffs(kA, kD, kS);
	SetIndex();
}

void SpotLight::SetCutOff(const float thetaDegrees)
{
	m_InnerCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 0.875f, 0.0f)));
	m_OuterCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 1.125f, 0.0f)));
}

void SpotLight::Update(const glm::vec4& position, const glm::vec3& direction)
{
	m_Pos = position;
	m_Direction = direction;
}
