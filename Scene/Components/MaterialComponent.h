#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Texture.h"

class Shader;

class AbstractMaterial
{
public:
    std::weak_ptr<Shader> m_Shader;
};

class MaterialComponent final : public AbstractMaterial
{
public:
    MaterialComponent() = default;

    MaterialComponent(const std::vector<std::shared_ptr<Tex2D>>& textures, float shininess);
    MaterialComponent(std::weak_ptr<Shader> shader, const std::vector<std::shared_ptr<Tex2D>>& textures, float shininess);

public:
    std::shared_ptr<Tex2D> m_BaseColorMap;
    std::shared_ptr<Tex2D> m_AlbedoMap;
    std::shared_ptr<Tex2D> m_MetallicMap;
    std::shared_ptr<Tex2D> m_RoughnessMap;
    std::shared_ptr<Tex2D> m_AmbientOcclusionMap;
    std::shared_ptr<Tex2D> m_NormalMap;
    std::shared_ptr<Tex2D> m_HeightMap;
    std::shared_ptr<Tex2D> m_OpacityMap;
    std::shared_ptr<Tex2D> m_EmissionMap;
    float m_Shininess = 1.0f;
    bool m_SetShininess = true;
};

class CubeMapMaterialComponent final : public AbstractMaterial
{
public:
    CubeMapMaterialComponent() = default;
    CubeMapMaterialComponent(std::shared_ptr<TexCube> texture)
        : m_Texture(std::move(texture)) {}

    CubeMapMaterialComponent(std::weak_ptr<Shader> shader, std::shared_ptr<TexCube> texture)
        : m_Texture(std::move(texture)) { m_Shader = std::move(shader); }

public:
    std::shared_ptr<TexCube> m_Texture;
};
