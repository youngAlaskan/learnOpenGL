#include "Scene.h"

#include "..\utils.h"
#include "Components\TransformComponent.h"
#include "Components\TagComponent.h"
#include "Components\Renderable\CubeComponent.h"
#include "Components\Renderable\PlaneComponent.h"
#include "Components\Renderable\TriangleMeshComponent.h"

// Create an entity in the scene registry and give it a...
// - TransformComponent
// - TagComponenent
// return the entt::entity reference
entt::entity Scene::CreateEntity(const std::string& name)
{
	const auto entity = m_Registry.create();
	AddComponent<TransformComponent>(entity);
	auto& tag = AddComponent<TagComponent>(entity);
	tag.Tag = name.empty() ? "Entity" : name;
	return entity;
}

// Function to be executed once upon program start
void Scene::OnStart() const
{
    assert(g_LitObjectShader);
    assert(g_LitObjectShader);
    assert(g_MirrorShader);
    assert(g_RefractorShader);
    assert(g_LineShader);
    assert(g_SkyboxShader);
    assert(g_ScreenShader);

	g_LitObjectShader->SetSceneData(m_SceneData);
	g_LitObjectShader->SetSceneData(m_SceneData);
	g_MirrorShader->SetSceneData(m_SceneData);
	g_RefractorShader->SetSceneData(m_SceneData);
	g_LineShader->SetSceneData(m_SceneData);
	g_SkyboxShader->SetSceneData(m_SceneData);
	g_ScreenShader->SetSceneData(m_SceneData);
}

// Function to be executed on every update iteration
void Scene::OnUpdate()
{
	auto renderer = m_Renderer.lock();
    assert(renderer);

    assert(GetAllEntitiesWith<SkyboxTag>().size() < 2);

    for (const auto& entity : GetAllEntitiesWith<SkyboxTag>()) {
        RenderSkybox(GetComponent<CubeComponent>(entity), GetComponent<CubeMapMaterialComponent>(entity));
    }

	// Draw all Vertex Array Buffers
	for (const auto& entity : GetAllEntitiesWith<RenderableTag, MaterialComponent>())
	{
		UseMaterialShader(GetComponent<MaterialComponent>(entity));

        // Render based on object type
		if (HasComponent<CubeComponent>(entity))
			renderer->RenderIndexed(GetComponent<CubeComponent>(entity).VAO);
		else if (HasComponent<PlaneComponent>(entity))
			renderer->RenderIndexed(GetComponent<PlaneComponent>(entity).VAO);
		else if (HasComponent<TriangleMeshComponent>(entity))
			renderer->RenderIndexed(GetComponent<TriangleMeshComponent>(entity).VAO);
	}
}

// Render the currently set skybox in m_SceneData
void Scene::RenderSkybox(const CubeComponent& mesh, const CubeMapMaterialComponent& material) const
{
    assert(material.m_Shader.lock());
    assert(material.m_Texture);

	// Do not set depth buffer
	glDepthMask(GL_FALSE);

    // Cube is seen from inside, so winding order is backwards
	glFrontFace(GL_CW);

    // Set active shader and texture(s)
	material.m_Shader.lock()->Use();
    material.m_Texture->Use();

    // Pass the skybox's mesh into the renderer
    auto renderer = m_Renderer.lock();
    if (renderer)
        renderer->RenderIndexed(mesh.VAO);

    // Reset flags
	glFrontFace(GL_CCW);

	glDepthMask(GL_TRUE);
}

// Set the active shader to be material's shader and populate shader data
void Scene::UseMaterialShader(const MaterialComponent& materialComponent)
{
    assert(materialComponent.m_Shader.lock());
	materialComponent.m_Shader.lock()->Use();

    SendMaterialDataToShader(materialComponent);
}

// Populate the material struct in the shader file as well as the texture array
void Scene::SendMaterialDataToShader(const MaterialComponent& materialComponent)
{
    assert(materialComponent.m_Shader.lock());
    auto shader = materialComponent.m_Shader.lock();

    int activeMaps = 0; // Bit Mask cooresponding to different texture types
    int index = 0; // Refers to the offset for the active texture flag

    if (materialComponent.m_BaseColorMap)
    {
        materialComponent.m_BaseColorMap->Use(index++);
        shader->SetInt("textures[0]", static_cast<int>(materialComponent.m_BaseColorMap->m_ID));
        activeMaps |= 0b1;
    }
    if (materialComponent.m_AlbedoMap)
    {
        materialComponent.m_AlbedoMap->Use(index++);
        shader->SetInt("textures[1]", static_cast<int>(materialComponent.m_AlbedoMap->m_ID));
        activeMaps |= 0b10;
    }
    if (materialComponent.m_MetallicMap)
    {
        materialComponent.m_MetallicMap->Use(index++);
        shader->SetInt("textures[2]", static_cast<int>(materialComponent.m_MetallicMap->m_ID));
        activeMaps |= 0b100;
    }
    if (materialComponent.m_RoughnessMap)
    {
        materialComponent.m_RoughnessMap->Use(index++);
        shader->SetInt("textures[3]", static_cast<int>(materialComponent.m_RoughnessMap->m_ID));
        activeMaps |= 0b1000;
    }
    if (materialComponent.m_AmbientOcclusionMap)
    {
        materialComponent.m_AmbientOcclusionMap->Use(index++);
        shader->SetInt("textures[4]", static_cast<int>(materialComponent.m_AmbientOcclusionMap->m_ID));
        activeMaps |= 0b10000;
    }
    if (materialComponent.m_NormalMap)
    {
        materialComponent.m_NormalMap->Use(index++);
        shader->SetInt("textures[5]", static_cast<int>(materialComponent.m_NormalMap->m_ID));
        activeMaps |= 0b100000;
    }
    if (materialComponent.m_HeightMap)
    {
        materialComponent.m_HeightMap->Use(index++);
        shader->SetInt("textures[6]", static_cast<int>(materialComponent.m_HeightMap->m_ID));
        activeMaps |= 0b1000000;
    }
    if (materialComponent.m_EmissionMap)
    {
        materialComponent.m_EmissionMap->Use(index);
        shader->SetInt("textures[7]", static_cast<int>(materialComponent.m_EmissionMap->m_ID));
        activeMaps |= 0b10000000;
    }

    shader->SetInt("material.activeMaps", activeMaps);
    if (materialComponent.m_SetShininess)
        shader->SetFloat("material.shininess", materialComponent.m_Shininess);
}

