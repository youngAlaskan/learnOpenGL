#pragma once

#include <memory>
#include <vector>

#include "Texture.h"

class Shader;

class MaterialComponent
{
public:
    MaterialComponent() = default;

    MaterialComponent(std::vector<std::shared_ptr<Tex2D>> textures, float shininess);
    MaterialComponent(const std::shared_ptr<Tex2D>& texture, float shininess);
    void SetTextures(const std::vector<std::shared_ptr<Tex2D>>& textures);
    std::vector<std::shared_ptr<Tex2D>> GetTextures() const
        { return m_Textures; }
    std::shared_ptr<Tex2D> GetTexture(const std::string& tag);

public:
    int m_DiffuseEnd = 0, m_SpecularEnd = 0, m_EmissiveEnd = 0;
    float m_Shininess = 1.0f;
    std::weak_ptr<Shader> m_Shader;

private:
    void SetIndices();

private:
    std::vector<std::shared_ptr<Tex2D>> m_Textures = std::vector<std::shared_ptr<Tex2D>>();
};
