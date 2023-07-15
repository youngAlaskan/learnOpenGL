#pragma once

class Light
{
public:
	Light() = default;

	explicit Light(const glm::vec3 color)
	{
		m_Color = color;
	}

	void SetColor(const glm::vec3 color)
	{
		m_Color = color;
	}

	virtual void SendToShader(const Shader& shader) const {}

public:
	glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_KA = 1.0f, m_KD = 1.0f, m_KS = 1.0f;
};

class PointLight : public Light
{
public:
	PointLight() = default;

	explicit PointLight(const float distance) : m_Pos(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		m_Distance = distance;
		m_Linear = 4.48882f / distance;
		m_Quadratic = 75.5817f / (distance * distance);
	}

	void SetPos(const glm::vec3 pos)
	{
		m_Pos = pos;
	}

	void SendToShader(const Shader& shader) const override
	{
		const std::string prefix = "pointLights[" + std::to_string(m_Index) + "].";
		shader.SetVec3(prefix + "position", m_Pos);
		shader.SetVec3(prefix + "color", m_Color);

		shader.SetFloat(prefix + "kA", m_KA);
		shader.SetFloat(prefix + "kD", m_KD);
		shader.SetFloat(prefix + "kS", m_KS);

		shader.SetFloat(prefix + "constant", m_Constant);
		shader.SetFloat(prefix + "linear", m_Linear);
		shader.SetFloat(prefix + "quadratic", m_Quadratic);
	}

public:
	glm::vec3 m_Pos = glm::vec3(0.0f);
	float m_Distance = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
	int m_Index = 0;
};

class DirectionalLight final : public Light
{
public:
	DirectionalLight() = default;

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec3("dirLight.direction", m_Direction);
		shader.SetVec3("dirLight.color", m_Color);

		shader.SetFloat("dirLight.kA", m_KA);
		shader.SetFloat("dirLight.kD", m_KD);
		shader.SetFloat("dirLight.kS", m_KS);
	}

public:
	glm::vec3 m_Direction = glm::vec3(0.0f);
};

class SpotLight final : public PointLight
{
public:
	SpotLight() = default;

	explicit SpotLight(const float theta)
	: m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
	  m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f)))) {}

	void SetCutOff(const float thetaDegrees)
	{
		m_InnerCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 0.875f, 0.0f)));
		m_OuterCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 1.125f, 0.0f)));
	}

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec3("spotLight.position", m_Pos);
		shader.SetVec3("spotLight.direction", m_Direction);
		shader.SetVec3("spotLight.color", m_Color);

		shader.SetFloat("spotLight.kA", m_KA);
		shader.SetFloat("spotLight.kD", m_KD);
		shader.SetFloat("spotLight.kS", m_KS);

		shader.SetFloat("spotLight.constant", m_Constant);
		shader.SetFloat("spotLight.linear", m_Linear);
		shader.SetFloat("spotLight.quadratic", m_Quadratic);

		shader.SetFloat("spotLight.innerCutOff", m_InnerCutOff);
		shader.SetFloat("spotLight.outerCutOff", m_OuterCutOff);
	}

public:
	glm::vec3 m_Direction = glm::vec3(0.0f);
	float m_InnerCutOff = 0.0f, m_OuterCutOff = 0.0f;
};
