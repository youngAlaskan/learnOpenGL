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
    : m_Shininess(shininess), m_Shader(std::move(shader))
{
    for (const auto& texture : textures)
    {
        switch (texture->m_Tag)
        {
        case "BaseColor":        m_BaseColorMap = texture;        break;
        case "Albedo":           m_AlbedoMap = texture;           break;
        case "Metallic":         m_MetallicMap = texture;         break;
        case "Roughness":        m_RoughnessMap = texture;        break;
        case "AmbientOcclusion": m_AmbientOcclusionMap = texture; break;
        case "Normal":           m_NormalMap = texture;           break;
        case "Height":           m_HeightMap = texture;           break;
        case "Opacity":          m_OpacityMap = texture;          break;
        case "Emission":         m_EmissionMap = texture;         break;
        default: std::cerr << "ERROR::MATERIAL: Texture with tag " << texture->m_Tag << " is not valid." << std::endl;
        }
    }
}
