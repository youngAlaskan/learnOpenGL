#include "MaterialComponent.h"

#include <iostream>

MaterialComponent::MaterialComponent(const std::vector<std::shared_ptr<Tex2D>>& textures, const float shininess)
    : m_Shininess(shininess)
{
    for (const auto& texture : textures)
    {
        if (texture->m_Tag == "BaseColor")        m_BaseColorMap = texture;
        if (texture->m_Tag == "Albedo")           m_AlbedoMap = texture;
        if (texture->m_Tag == "Metallic")         m_MetallicMap = texture;
        if (texture->m_Tag == "Roughness")        m_RoughnessMap = texture;
        if (texture->m_Tag == "AmbientOcclusion") m_AmbientOcclusionMap = texture;
        if (texture->m_Tag == "Normal")           m_NormalMap = texture;
        if (texture->m_Tag == "Height")           m_HeightMap = texture;
        if (texture->m_Tag == "Opacity")          m_OpacityMap = texture;
        if (texture->m_Tag == "Emission")         m_EmissionMap = texture;
    }
}

MaterialComponent::MaterialComponent(std::weak_ptr<Shader> shader, const std::vector<std::shared_ptr<Tex2D>>& textures, const float shininess)
    : m_Shininess(shininess)
{
    m_Shader = std::move(shader);

    for (const auto& texture : textures)
    {
        if (texture->m_Tag == "BaseColor")        m_BaseColorMap = texture;
        if (texture->m_Tag == "Albedo")           m_AlbedoMap = texture;
        if (texture->m_Tag == "Metallic")         m_MetallicMap = texture;
        if (texture->m_Tag == "Roughness")        m_RoughnessMap = texture;
        if (texture->m_Tag == "AmbientOcclusion") m_AmbientOcclusionMap = texture;
        if (texture->m_Tag == "Normal")           m_NormalMap = texture;
        if (texture->m_Tag == "Height")           m_HeightMap = texture;
        if (texture->m_Tag == "Opacity")          m_OpacityMap = texture;
        if (texture->m_Tag == "Emission")         m_EmissionMap = texture;
    }
}
