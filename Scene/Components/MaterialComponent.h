#pragma once

#include <memory>
#include <vector>

#include "TextureComponent.h"

class MaterialComponent
{
public:
    MaterialComponent() = default;

    MaterialComponent(std::vector<std::shared_ptr<Tex2DComponent>> textures, float shininess);
    MaterialComponent(const std::shared_ptr<Tex2DComponent>& texture, float shininess);
    void SetTextures(const std::vector<std::shared_ptr<Tex2DComponent>>& textures);
    std::vector<std::shared_ptr<Tex2DComponent>> GetTextures() const
        { return m_Textures; }
    std::shared_ptr<Tex2DComponent> GetTexture(const std::string& tag);

public:
    int m_DiffuseEnd = 0, m_SpecularEnd = 0, m_EmissiveEnd = 0;
    float m_Shininess = 1.0f;

private:
    void SetIndices();

private:
    std::vector<std::shared_ptr<Tex2DComponent>> m_Textures = std::vector<std::shared_ptr<Tex2DComponent>>();
};
