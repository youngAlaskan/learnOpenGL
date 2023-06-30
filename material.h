#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "shader.h"
#include "texture.h"

class Material
{
public:
    Material() = default;

    Material(Texture* diffuseMap, Texture* specularMap, Texture* emissiveMap, const float shininess)
    {
        m_DiffuseMap = diffuseMap;
        m_SpecularMap = specularMap;
        m_EmissiveMap = emissiveMap;
        m_Shininess = shininess;
    }

    float GetShininess() const
    {
        return m_Shininess;
    }

    void SetDiffuseTexture(Texture* diffuseMap)
    {
        m_DiffuseMap = diffuseMap;
    }

    void SetSpecularColor(Texture* specularMap)
    {
        m_SpecularMap = specularMap;
    }

    void SetShininess(const float shininess)
    {
        m_Shininess = shininess;
    }

    void SendToShader(const Shader& shader) const
    {
        m_DiffuseMap->Use();
        m_SpecularMap->Use();
        m_EmissiveMap->Use();

        shader.SetInt("material.diffuse", static_cast<int>(m_DiffuseMap->m_ID));
        shader.SetInt("material.specular", static_cast<int>(m_SpecularMap->m_ID));
        shader.SetInt("material.emissive", static_cast<int>(m_EmissiveMap->m_ID));
        shader.SetFloat("material.shininess", m_Shininess);
    }

public:
    Texture* m_DiffuseMap;
    Texture* m_SpecularMap;
    Texture* m_EmissiveMap;
    float m_Shininess = 1.0f;
};
