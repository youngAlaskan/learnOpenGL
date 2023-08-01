#include "MaterialComponent.h"

MaterialComponent::MaterialComponent(std::vector<std::shared_ptr<Tex2DComponent>> textures, const float shininess)
    : m_Shininess(shininess), m_Textures(std::move(textures))
{
    SetIndices();
}

MaterialComponent::MaterialComponent(const std::shared_ptr<Tex2DComponent>& texture, const float shininess)
    : m_Shininess(shininess)
{
    m_Textures.emplace_back(texture);
    SetIndices();
}

void MaterialComponent::SetTextures(const std::vector<std::shared_ptr<Tex2DComponent>>& textures)
{
    m_Textures = textures;
    SetIndices();
}

std::shared_ptr<Tex2DComponent> MaterialComponent::GetTexture(const std::string& tag)
{
    for (auto& texture : m_Textures)
        if (texture->m_Tag == tag) return texture;

    return nullptr;
}

void MaterialComponent::SetIndices()
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
