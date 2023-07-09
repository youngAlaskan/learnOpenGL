#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    Material() = default;

    Material(std::vector<std::shared_ptr<Texture>> textures, const float shininess)
        : m_Textures(std::move(textures)), m_Shininess(shininess) {}

    void SetTextures(const std::vector<std::shared_ptr<Texture>>& textures) { m_Textures = textures; }
    void SetShininess(const float shininess) { m_Shininess = shininess; }
    std::vector<std::shared_ptr<Texture>> GetTextures() const { return m_Textures; }
    float GetShininess() const { return m_Shininess; }

    std::shared_ptr<Texture> GetTexture(const std::string& tag)
    {
        for (auto& texture : m_Textures)
            if (texture->m_Tag == tag) return texture;

        return nullptr;
    }

    void SendToShader(const Shader& shader) const
    {
        for (auto& texture : m_Textures) {
            texture->Use();
            shader.SetInt("material." + texture->m_Tag, static_cast<int>(texture->m_ID));
        }

        shader.SetFloat("material.shininess", m_Shininess);
    }

public:
    std::vector<std::shared_ptr<Texture>> m_Textures;
    float m_Shininess = 1.0f;
};
