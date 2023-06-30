#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "shader.h"
#include "texture.h"

struct Material
{
public:
    Material() = default;

    Material(const Texture& diffuseTexture, const Texture& specularTexture, const float shininess)
    {
        m_DiffuseTexture = diffuseTexture;
    	m_SpecularTexture = specularTexture;
        m_Shininess = shininess;
    }

    float GetShininess() const
    {
        return m_Shininess;
    }

    void SetDiffuseTexture(const Texture& diffuseTexture)
    {
        m_DiffuseTexture = diffuseTexture;
    }

    void SetSpecularColor(const Texture& specularTexture)
    {
        m_SpecularTexture = specularTexture;
    }

    void SetShininess(const float shininess)
    {
        m_Shininess = shininess;
    }

    void SendToShader(const Shader& shader) const
    {
        m_DiffuseTexture.UseCube(GL_TEXTURE0);
        m_SpecularTexture.UseCube(GL_TEXTURE1);

        shader.SetInt("material.diffuse", m_DiffuseTexture.m_ID);
        shader.SetInt("material.specular", m_SpecularTexture.m_ID);
        shader.SetFloat("material.shininess", m_Shininess);
    }

public:
    Texture m_DiffuseTexture;
    Texture m_SpecularTexture;
    float m_Shininess = 0.0f;
};

enum Name {
    EMERALD, JADE, OBSIDIAN, PEARL, RUBY,
    TURQUOISE, BRASS, BRONZE, CHROME, COPPER,
    GOLD, SILVER
};

std::unordered_map<int, Material> materials;

inline void InitMaterials()
{
    materials = {
    {EMERALD,   Material(Texture(256, 256, glm::vec3(0.0215f, 0.745f, 0.0215f)),
                Texture(256, 256, glm::vec3(0.633f, 0.727811f, 0.633f)), 0.6f)},
    {JADE,      Material(Texture(256, 256, glm::vec3(0.135f, 0.2225f, 0.1575f)),
                         Texture(256, 256, glm::vec3(0.316228f, 0.316228f, 0.316228f)), 0.1f)},
    {OBSIDIAN,  Material(Texture(256, 256, glm::vec3(0.05375f, 0.05f, 0.06625f)),
                         Texture(256, 256, glm::vec3(0.332741f, 0.328634f, 0.346435f)), 0.3f)},
    {PEARL,     Material(Texture(256, 256, glm::vec3(0.25f, 0.20725f, 0.20725f)),
                         Texture(256, 256, glm::vec3(0.296648f, 0.296648f, 0.296648f)), 0.088f)},
    {RUBY,      Material(Texture(256, 256, glm::vec3(0.1745f, 0.01175f, 0.01175f)),
                         Texture(256, 256, glm::vec3(0.727811f, 0.626959f, 0.626959f)), 0.6f)},
    {TURQUOISE, Material(Texture(256, 256, glm::vec3(0.1f, 0.18725f, 0.1745f)),
                         Texture(256, 256, glm::vec3(0.297254f, 0.30829f, 0.306678f)), 0.1f)},
    {BRASS,     Material(Texture(256, 256, glm::vec3(0.329412f, 0.223529f, 0.027451f)),
                         Texture(256, 256, glm::vec3(0.992157f, 0.941176f, 0.807843f)), 0.21794872f)},
    {BRONZE,    Material(Texture(256, 256, glm::vec3(0.2125f, 0.1275f, 0.054f)),
                         Texture(256, 256, glm::vec3(0.393548f, 0.271906f, 0.166721f)), 0.2f)},
    {CHROME,    Material(Texture(256, 256, glm::vec3(0.25f, 0.25f, 0.25f)),
                         Texture(256, 256, glm::vec3(0.774597f, 0.774597f, 0.774597f)), 0.6f)},
    {COPPER,    Material(Texture(256, 256, glm::vec3(0.19125f, 0.0735f, 0.0225f)),
                         Texture(256, 256, glm::vec3(0.25677f, 0.137622f, 0.086014f)), 0.1f)},
    {GOLD,      Material(Texture(256, 256, glm::vec3(0.24725f, 0.1995f, 0.0745f)),
                         Texture(256, 256, glm::vec3(0.628281f, 0.555802f, 0.366065f)), 0.4f)},
    {SILVER,    Material(Texture(256, 256, glm::vec3(0.19225f, 0.19225f, 0.19225f)),
                         Texture(256, 256, glm::vec3(0.508273f, 0.508273f, 0.508273f)), 0.4f)}
    };
}
