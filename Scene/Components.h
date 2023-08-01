#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "C:/Dev/LearnOpenGL/Vertex.h"
#include "C:/Dev/LearnOpenGL/Shader.h"

struct TransformComponent
{
	glm::mat4 Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	explicit TransformComponent(const glm::mat4& transform)
		: Transform(transform) {}
	~TransformComponent() = default;

	operator glm::mat4& () { return Transform; }
	operator const glm::mat4& () const { return Transform; }
};

struct TagComponent
{
	std::string Tag = std::string();

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	explicit TagComponent(std::string tag)
		: Tag(std::move(tag)) {}
	~TagComponent() = default;

	operator std::string& () { return Tag; }
	operator const std::string& () const { return Tag; }
};

struct MeshComponent
{
	unsigned int VAO = 0;
	int VertexCount = 0;

	MeshComponent() = default;
	MeshComponent(const MeshComponent&) = default;
	explicit MeshComponent(const unsigned int vao, const int vertexCount)
		: VAO(vao), VertexCount(vertexCount) {}
	MeshComponent(const std::vector<Vertex>& connectivityData, const std::vector<unsigned int>& indices);
	bool operator==(const MeshComponent& other) const { return VAO == other && VertexCount == other.VertexCount; }
	~MeshComponent() = default;

	operator unsigned int& () { return VAO; }
	operator const unsigned int& () const { return VAO; }
};

struct Tex2DComponent
{
	unsigned int ID = 0;

	Tex2DComponent() = default;
	Tex2DComponent(const Tex2DComponent&) = default;
	explicit Tex2DComponent(const unsigned int id)
		: ID(id) {}
	bool operator==(const Tex2DComponent& other) const { return ID == other; }
	~Tex2DComponent() = default;

	operator unsigned int& () { return ID; }
	operator const unsigned int& () const { return ID; }

	void Use() const
	{
		glActiveTexture(GL_TEXTURE0 + ID);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
};

struct TexCubeComponent
{
	unsigned int ID = 0;

	TexCubeComponent() = default;
	TexCubeComponent(const TexCubeComponent&) = default;
	explicit TexCubeComponent(const unsigned int id)
		: ID(id) {}
	bool operator==(const TexCubeComponent& other) const { return ID == other; }
	~TexCubeComponent() = default;

	operator unsigned int& () { return ID; }
	operator const unsigned int& () const { return ID; }

	void Use() const
	{
		glActiveTexture(GL_TEXTURE0 + ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}
};

struct Material2DComponent
{
	std::shared_ptr<Tex2DComponent> DiffuseMap;
	std::shared_ptr<Tex2DComponent> SpecularMap;
	std::shared_ptr<Tex2DComponent> EmissiveMap;
	float Shininess = 1.0f;

	Material2DComponent() = default;
	Material2DComponent(const Material2DComponent&) = default;
	explicit Material2DComponent(std::shared_ptr<Tex2DComponent> diffuseMap, std::shared_ptr<Tex2DComponent> specularMap, std::shared_ptr<Tex2DComponent> emissiveMap, const float shininess)
		: DiffuseMap(std::move(diffuseMap)), SpecularMap(std::move(specularMap)), EmissiveMap(std::move(emissiveMap)), Shininess(shininess) {}
	bool operator==(const Material2DComponent& other) const
	{
		return DiffuseMap == other.DiffuseMap &&
			SpecularMap == other.SpecularMap &&
			EmissiveMap == other.EmissiveMap &&
			Shininess == other.Shininess;
	}
	~Material2DComponent() = default;

	void SendToShader(Shader shader) const;
};

struct MaterialCubeComponent
{
	std::shared_ptr<TexCubeComponent> DiffuseMap;
	std::shared_ptr<TexCubeComponent> SpecularMap;
	std::shared_ptr<TexCubeComponent> EmissiveMap;
	float Shininess = 1.0f;

	MaterialCubeComponent() = default;
	MaterialCubeComponent(const MaterialCubeComponent&) = default;
	explicit MaterialCubeComponent(std::shared_ptr<TexCubeComponent> diffuseMap, std::shared_ptr<TexCubeComponent> specularMap, std::shared_ptr<TexCubeComponent> emissiveMap, const float shininess)
		: DiffuseMap(std::move(diffuseMap)), SpecularMap(std::move(specularMap)), EmissiveMap(std::move(emissiveMap)), Shininess(shininess) {}
	bool operator==(const MaterialCubeComponent& other) const
	{
		return DiffuseMap == other.DiffuseMap &&
			SpecularMap == other.SpecularMap &&
			EmissiveMap == other.EmissiveMap &&
			Shininess == other.Shininess;
	}
	~MaterialCubeComponent() = default;

	void SendToShader(Shader shader) const;
};

struct PointLightComponent
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float Distance = 0.0f;
	float KA = 0.0f, KD = 0.0f, KS = 0.0f;
	float Constant = 1.0f, Linear = 0.0f, Quadratic = 0.0f;
	inline static int Index = 0;

	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent&) = default;
	explicit PointLightComponent(const glm::vec3 position, const glm::vec3 color, const float distance, const float kA, const float kD, const float kS)
		: Position(position), Color(color), Distance(distance), KA(kA), KD(kD), KS(kS), Linear(4.48882f / distance), Quadratic(75.5817f / (distance * distance)) {}
	bool operator==(const PointLightComponent& other) const
	{
		return Position == other.Position &&
			Color == other.Color &&
			Distance == other.Distance &&
			KA == other.KA &&
			KD == other.KD &&
			KS == other.KS &&
			Constant == other.Constant &&
			Linear == other.Linear &&
			Quadratic == other.Quadratic;
	}
	~PointLightComponent() = default;

	void SendToShader(Shader shader) const;
};

struct DirectionalLightComponent
{
	glm::vec3 Direction = glm::vec3(0.0f);
	glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float KA = 0.0f, KD = 0.0f, KS = 0.0f;

	DirectionalLightComponent() = default;
	DirectionalLightComponent(const DirectionalLightComponent&) = default;
	bool operator==(const DirectionalLightComponent& other) const
	{
		return Direction == other.Direction &&
			Color == other.Color &&
			KA == other.KA &&
			KD == other.KD &&
			KS == other.KS;
	}
	~DirectionalLightComponent() = default;

	void SendToShader(Shader shader) const;
};

struct SpotLightComponent
{
	glm::vec3 Direction = glm::vec3(0.0f);
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float Distance = 0.0f;
	float KA = 0.0f, KD = 0.0f, KS = 0.0f;
	float Constant = 1.0f, Linear = 0.0f, Quadratic = 0.0f;
	float InnerCutOff = 0.0f, OuterCutOff = 0.0f;

	SpotLightComponent() = default;
	SpotLightComponent(const SpotLightComponent&) = default;
	explicit SpotLightComponent(const glm::vec3 position, const glm::vec3 color, const float distance, const float kA, const float kD, const float kS, const float theta)
		: Position(position), Color(color), Distance(distance), KA(kA), KD(kD), KS(kS), Linear(4.48882f / distance), Quadratic(75.5817f / (distance * distance)), InnerCutOff(CosDegrees(theta, 0.875f)), OuterCutOff(CosDegrees(theta, 1.125f)) {}
	bool operator==(const SpotLightComponent& other) const
	{
		return Position == other.Position &&
			Color == other.Color &&
			Distance == other.Distance &&
			KA == other.KA &&
			KD == other.KD &&
			KS == other.KS &&
			Constant == other.Constant &&
			Linear == other.Linear &&
			Quadratic == other.Quadratic;
	}
	~SpotLightComponent() = default;

	void SendToShader(Shader shader) const;

private:
	float CosDegrees(const float theta, const float fraction = 1.0f) const
	{
		return glm::cos(glm::radians(std::max(theta * fraction, 0.0f)));
	}
};
