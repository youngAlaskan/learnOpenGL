#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    Material() = default;
    Material(std::shared_ptr<Texture> diffuseMap, std::shared_ptr<Texture> specularMap, std::shared_ptr<Texture> emissiveMap, const float shininess)
        : m_DiffuseMap(std::move(diffuseMap)), m_SpecularMap(std::move(specularMap)), m_EmissiveMap(std::move(emissiveMap)), m_Shininess(shininess) {}
    float GetShininess() const { return m_Shininess; }
    void SetDiffuseTexture(const std::shared_ptr<Texture>& diffuseMap) { m_DiffuseMap = diffuseMap; }
    void SetSpecularTexture(const std::shared_ptr<Texture>& specularMap) { m_SpecularMap = specularMap; }
    void SetEmissiveTexture(const std::shared_ptr<Texture>& emissiveMap) { m_EmissiveMap = emissiveMap; }
    void SetShininess(const float shininess) { m_Shininess = shininess; }
    void SendToShader(const Shader& shader) const;

public:
    std::shared_ptr<Texture> m_DiffuseMap;
    std::shared_ptr<Texture> m_SpecularMap;
    std::shared_ptr<Texture> m_EmissiveMap;
    float m_Shininess = 1.0f;
};
