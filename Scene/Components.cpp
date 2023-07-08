#include "Components.h"

#include <glad/glad.h>

MeshComponent::MeshComponent(const std::vector<Vertex>& connectivityData, const std::vector<unsigned int>& indices)
	: VertexCount(static_cast<int>(connectivityData.size()))
{
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, connectivityData.size() * sizeof(Vertex), connectivityData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	constexpr int stride = 12 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Unbinds
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Material2DComponent::SendToShader(const Shader shader) const
{
	DiffuseMap->Use();
	SpecularMap->Use();
	EmissiveMap->Use();

	shader.SetInt("material.diffuse", *DiffuseMap);
	shader.SetInt("material.specular", *SpecularMap);
	shader.SetInt("material.emissive", *EmissiveMap);
	shader.SetFloat("material.shininess", Shininess);
}

void MaterialCubeComponent::SendToShader(const Shader shader) const
{
	DiffuseMap->Use();
	SpecularMap->Use();
	EmissiveMap->Use();

	shader.SetInt("material.diffuse", *DiffuseMap);
	shader.SetInt("material.specular", *SpecularMap);
	shader.SetInt("material.emissive", *EmissiveMap);
	shader.SetFloat("material.shininess", Shininess);
}

void PointLightComponent::SendToShader(const Shader shader) const
{
	const std::string prefix = "pointLights[" + std::to_string(Index) + "].";
	shader.SetVec3(prefix + "position", Position);
	shader.SetVec3(prefix + "color", Color);

	shader.SetFloat(prefix + "kA", KA);
	shader.SetFloat(prefix + "kD", KD);
	shader.SetFloat(prefix + "kS", KS);

	shader.SetFloat(prefix + "constant", Constant);
	shader.SetFloat(prefix + "linear", Linear);
	shader.SetFloat(prefix + "quadratic", Quadratic);
}

void DirectionalLightComponent::SendToShader(const Shader shader) const
{
	shader.SetVec3("dirLight.direction", Direction);
	shader.SetVec3("dirLight.color", Color);

	shader.SetFloat("dirLight.kA", KA);
	shader.SetFloat("dirLight.kD", KD);
	shader.SetFloat("dirLight.kS", KS);
}

void SpotLightComponent::SendToShader(const Shader shader) const
{
	shader.SetVec3("spotLight.position", Position);
	shader.SetVec3("spotLight.direction", Direction);
	shader.SetVec3("spotLight.color", Color);

	shader.SetFloat("spotLight.kA", KA);
	shader.SetFloat("spotLight.kD", KD);
	shader.SetFloat("spotLight.kS", KS);

	shader.SetFloat("spotLight.constant", Constant);
	shader.SetFloat("spotLight.linear", Linear);
	shader.SetFloat("spotLight.quadratic", Quadratic);

	shader.SetFloat("spotLight.innerCutOff", InnerCutOff);
	shader.SetFloat("spotLight.outerCutOff", OuterCutOff);
}



