#include "Material.h"

void Material::SendToShader(const Shader& shader) const
{
    m_DiffuseMap->Use();
    m_SpecularMap->Use();
    m_EmissiveMap->Use();

    shader.SetInt("material.diffuse", static_cast<int>(m_DiffuseMap->m_ID));
    shader.SetInt("material.specular", static_cast<int>(m_SpecularMap->m_ID));
    shader.SetInt("material.emissive", static_cast<int>(m_EmissiveMap->m_ID));
    shader.SetFloat("material.shininess", m_Shininess);
}
