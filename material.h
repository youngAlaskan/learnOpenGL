#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    Material() = default;

    Material(std::vector<std::shared_ptr<Tex2D>> textures, const float shininess)
        : m_Textures(std::move(textures)), m_Shininess(shininess)
    {
        SetIndices();
    }

    Material(const std::shared_ptr<Tex2D>& texture, const float shininess)
        : m_Shininess(shininess)
    {
        m_Textures.emplace_back(texture);
        SetIndices();
    }

    void SetTextures(const std::vector<std::shared_ptr<Tex2D>>& textures)
    {
        m_Textures = textures;
        SetIndices();
    }
    void SetShininess(const float shininess) { m_Shininess = shininess; }
    std::vector<std::shared_ptr<Tex2D>> GetTextures() const { return m_Textures; }
    int GetDiffuseEnd() const { return m_DiffuseEnd; }
    int GetSpecularEnd() const { return m_SpecularEnd; }
    int GetEmissiveEnd() const { return m_EmissiveEnd; }
    float GetShininess() const { return m_Shininess; }

    std::shared_ptr<Tex2D> GetTexture(const std::string& tag)
    {
        for (auto& texture : m_Textures)
            if (texture->m_Tag == tag) return texture;

        return nullptr;
    }

    void SendToShader(const Shader& shader) const
    {
        for (int i = 0; i < static_cast<int>(m_Textures.size()); i++)
            m_Textures[i]->Use(i);

        shader.SetInt("material.diffuseEnd", m_DiffuseEnd);
        shader.SetInt("material.specularEnd", m_SpecularEnd);
        shader.SetInt("material.emissiveEnd", m_EmissiveEnd);
        shader.SetFloat("material.shininess", m_Shininess);
    }

private:
    std::vector<std::shared_ptr<Tex2D>> m_Textures = std::vector<std::shared_ptr<Tex2D>>();
    int m_DiffuseEnd = 0, m_SpecularEnd = 0, m_EmissiveEnd = 0;
    float m_Shininess = 1.0f;

private:
    void SetIndices()
    {
        for (const auto& texture : m_Textures)
        {
            if (texture->m_Tag == std::string("diffuse"))
                m_DiffuseEnd++;
            else if (texture->m_Tag == std::string("specular"))
                m_SpecularEnd++;
            else if (texture->m_Tag == std::string("emissive"))
                m_EmissiveEnd++;
        }

        m_SpecularEnd += m_DiffuseEnd;
        m_EmissiveEnd += m_SpecularEnd;
    }
};
