#pragma once

class Light
{
public:
	Light() = default;

	void SetColor(const glm::vec4 color)
	{
		m_Color = color;
	}

	void SetCoeffs(const float kA, const float kD, const float kS)
	{
		m_KA = kA;
		m_KD = kD;
		m_KS = kS;
	}

	virtual void SendToShader(const Shader& shader) const {}

public:
	glm::vec4 m_Color = glm::vec4(1.0f);
	float m_KA = 1.0f, m_KD = 1.0f, m_KS = 1.0f;
};

class PointLight : public Light
{
public:
	PointLight() { SetIndex(); }

	explicit PointLight(const glm::vec4 position) : m_Pos(position) { SetIndex(); }

	explicit PointLight(const float distance)
		: m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance))
	{
		SetIndex();
	}

	explicit PointLight(const float distance, const float kA, const float kD, const float kS)
		: m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance))
	{
		SetCoeffs(kA, kD, kS);
		SetIndex();
	}

	explicit PointLight(const glm::vec4 position, const float distance, const float kA, const float kD, const float kS)
		: m_Pos(position), m_Distance(distance), m_Linear(4.48882f / distance), m_Quadratic(75.5817f / (distance * distance))
	{
		SetCoeffs(kA, kD, kS);
		SetIndex();
	}

	void SetPos(const glm::vec4 pos)
	{
		m_Pos = pos;
	}

	static int GetCount() { return s_Count; }

	void SendToShader(const Shader& shader) const override
	{
		const std::string prefix = "pointLights[" + std::to_string(m_Index) + "].";
		shader.SetVec4(prefix + "position", m_Pos);
		shader.SetVec4(prefix + "color", m_Color);

		shader.SetFloat(prefix + "kA", m_KA);
		shader.SetFloat(prefix + "kD", m_KD);
		shader.SetFloat(prefix + "kS", m_KS);

		shader.SetFloat(prefix + "constant", m_Constant);
		shader.SetFloat(prefix + "linear", m_Linear);
		shader.SetFloat(prefix + "quadratic", m_Quadratic);
	}

	virtual ~PointLight()
	{
		s_Count--;
	}

public:
	glm::vec4 m_Pos = glm::vec4(0.0f);
	float m_Distance = 0.0f;
	float m_Constant = 1.0f, m_Linear = 0.0f, m_Quadratic = 0.0f;
	int m_Index = 0;

private:
	void SetIndex()
	{
		m_Index = s_Count++;
	}

private:
	inline static int s_Count = 0;
};

class DirectionalLight final : public Light
{
public:
	DirectionalLight() = default;

	explicit DirectionalLight(const glm::vec3 direction) : m_Direction(direction) {}

	explicit DirectionalLight(const glm::vec3 direction, const float kA, const float kD, const float kS)
		: m_Direction(direction)
	{
		SetCoeffs(kA, kD, kS);
	}

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec3("dirLight.direction", m_Direction);
		shader.SetVec4("dirLight.color", m_Color);

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

	explicit SpotLight(const float theta, const float kA, const float kD, const float kS)
		: m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
		  m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f))))
	{
		SetCoeffs(kA, kD, kS);
	}

	explicit SpotLight(const glm::vec3 direction, const float theta, const float kA, const float kD, const float kS)
		: m_Direction(direction),
	      m_InnerCutOff(glm::cos(glm::radians(std::max(theta * 0.875f, 0.0f)))),
		  m_OuterCutOff(glm::cos(glm::radians(std::max(theta * 1.125f, 0.0f))))
	{
		SetCoeffs(kA, kD, kS);
	}

	void SetCutOff(const float thetaDegrees)
	{
		m_InnerCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 0.875f, 0.0f)));
		m_OuterCutOff = glm::cos(glm::radians(std::max(thetaDegrees * 1.125f, 0.0f)));
	}

	void SendToShader(const Shader& shader) const override
	{
		shader.SetVec4("spotLight.position", m_Pos);
		shader.SetVec3("spotLight.direction", m_Direction);
		shader.SetVec4("spotLight.color", m_Color);

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
