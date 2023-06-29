#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "shader.h"

struct Material
{
public:
    Material() = default;

    Material(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess)
    {
        m_AmbientColor = ambient;
        m_DiffuseColor = diffuse;
        m_SpecularColor = specular;
        m_Shininess = shininess;
    }

    glm::vec3 GetAmbientColor() const
    {
        return m_AmbientColor;
    }

    glm::vec3 GetDiffuseColor() const
    {
        return m_DiffuseColor;
    }

    glm::vec3 GetSpecularColor() const
    {
        return m_SpecularColor;
    }

    float GetShininess() const
    {
        return m_Shininess;
    }

    void SetAmbientColor(const glm::vec3 color)
    {
        m_AmbientColor = color;
    }

    void SetDiffuseColor(const glm::vec3 color)
    {
        m_DiffuseColor = color;
    }

    void SetSpecularColor(const glm::vec3 color)
    {
        m_SpecularColor = color;
    }

    void SetShininess(const float shininess)
    {
        m_Shininess = shininess;
    }

    void SendToShader(const Shader& shader, const glm::vec3 lightColor) const
    {
        shader.SetVec3("material.ambientColor", m_AmbientColor * lightColor);
        shader.SetVec3("material.diffuseColor", m_DiffuseColor * lightColor);
        shader.SetVec3("material.specularColor", m_SpecularColor * lightColor);
        shader.SetFloat("material.shininess", m_Shininess * 128.0f);
    }

private:
    glm::vec3 m_AmbientColor = glm::vec3(0.0f);
    glm::vec3 m_DiffuseColor = glm::vec3(0.0f);
    glm::vec3 m_SpecularColor = glm::vec3(0.0f);
    float m_Shininess = 0.0f;
};

enum Name {
    EMERALD, JADE, OBSIDIAN, PEARL, RUBY,
    TURQUOISE, BRASS, BRONZE, CHROME, COPPER,
    GOLD, SILVER
};

std::unordered_map<int, Material> materials = {
    {EMERALD,   Material(glm::vec3(0.0215f, 0.745f, 0.0215f),
                         glm::vec3(0.07568f, 0.61424f, 0.07568f),
                         glm::vec3(0.633f, 0.727811f, 0.633f), 0.6f)},
    {JADE,      Material(glm::vec3(0.135f, 0.2225f, 0.1575f),
                         glm::vec3(0.54f, 0.89f, 0.63f),
                         glm::vec3(0.316228f, 0.316228f, 0.316228f), 0.1f)},
    {OBSIDIAN,  Material(glm::vec3(0.05375f, 0.05f, 0.06625f),
                         glm::vec3(0.18275f, 0.17f, 0.22525f),
                         glm::vec3(0.332741f, 0.328634f, 0.346435f), 0.3f)},
    {PEARL,     Material(glm::vec3(0.25f, 0.20725f, 0.20725f),
                         glm::vec3(1.0f, 0.829f, 0.829f),
                         glm::vec3(0.296648f, 0.296648f, 0.296648f), 0.088f)},
    {RUBY,      Material(glm::vec3(0.1745f, 0.01175f, 0.01175f),
                         glm::vec3(0.61424f, 0.04136f, 0.04136f),
                         glm::vec3(0.727811f, 0.626959f, 0.626959f), 0.6f)},
    {TURQUOISE, Material(glm::vec3(0.1f, 0.18725f, 0.1745f),
                         glm::vec3(0.396f, 0.74151f, 0.69102f),
                         glm::vec3(0.297254f, 0.30829f, 0.306678f), 0.1f)},
    {BRASS,     Material(glm::vec3(0.329412f, 0.223529f, 0.027451f),
                         glm::vec3(0.780392f, 0.568627f, 0.113725f),
                         glm::vec3(0.992157f, 0.941176f, 0.807843f), 0.21794872f)},
    {BRONZE,    Material(glm::vec3(0.2125f, 0.1275f, 0.054f),
                         glm::vec3(0.714f, 0.4284f, 0.18144f),
                         glm::vec3(0.393548f, 0.271906f, 0.166721f), 0.2f)},
    {CHROME,    Material(glm::vec3(0.25f, 0.25f, 0.25f),
                         glm::vec3(0.4f, 0.4f, 0.4f),
                         glm::vec3(0.774597f, 0.774597f, 0.774597f), 0.6f)},
    {COPPER,    Material(glm::vec3(0.19125f, 0.0735f, 0.0225f),
                         glm::vec3(0.7308f, 0.27048f, 0.0828f),
                         glm::vec3(0.25677f, 0.137622f, 0.086014f), 0.1f)},
    {GOLD,      Material(glm::vec3(0.24725f, 0.1995f, 0.0745f),
                         glm::vec3(0.75164f, 0.60648f, 0.22648f),
                         glm::vec3(0.628281f, 0.555802f, 0.366065f), 0.4f)},
    {SILVER,    Material(glm::vec3(0.19225f, 0.19225f, 0.19225f),
                         glm::vec3(0.50754f, 0.50754f, 0.50754f),
                         glm::vec3(0.508273f, 0.508273f, 0.508273f), 0.4f)}
};
