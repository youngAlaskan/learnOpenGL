#include "Light.h"

void PointLight::SendToShader(const Shader& shader) const
{
	const std::string prefix = "pointLights[" + std::to_string(Index) + "].";
	shader.SetVec3(prefix + "position", Pos);
	shader.SetVec3(prefix + "color", Color);

	shader.SetFloat(prefix + "kA", KA);
	shader.SetFloat(prefix + "kD", KD);
	shader.SetFloat(prefix + "kS", KS);

	shader.SetFloat(prefix + "constant", Constant);
	shader.SetFloat(prefix + "linear", Linear);
	shader.SetFloat(prefix + "quadratic", Quadratic);
}

bool PointLight::operator==(const PointLight& other) const
{
	return Pos == other.Pos &&
		Distance == other.Distance &&
		Color == other.Color &&
		KA == other.KA &&
		KD == other.KD &&
		KS == other.KS &&
		Constant == other.Constant &&
		Linear == other.Linear &&
		Quadratic == other.Quadratic &&
		Index == other.Index;
}


void DirectionalLight::SendToShader(const Shader& shader) const
{
	shader.SetVec3("dirLight.direction", Direction);
	shader.SetVec3("dirLight.color", m_Color);

	shader.SetFloat("dirLight.kA", m_KA);
	shader.SetFloat("dirLight.kD", m_KD);
	shader.SetFloat("dirLight.kS", m_KS);
}

bool DirectionalLight::operator==(const DirectionalLight& other) const
{
	return Direction == other.Direction &&
		m_Color == other.m_Color &&
		m_KA == other.m_KA &&
		m_KD == other.m_KD &&
		m_KS == other.m_KS;
}


void SpotLight::SendToShader(const Shader& shader) const
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

bool SpotLight::operator==(const SpotLight& other) const
{
	return m_Pos == other.m_Pos &&
		m_Direction == other.m_Direction &&
		m_Color == other.m_Color &&
		m_KA == other.m_KA &&
		m_KD == other.m_KD &&
		m_KS == other.m_KS &&
		m_Constant == other.m_Constant &&
		m_Linear == other.m_Linear &&
		m_Quadratic == other.m_Quadratic &&
		m_InnerCutOff == other.m_InnerCutOff &&
		m_OuterCutOff == other.m_OuterCutOff;
}

