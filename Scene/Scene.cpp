#include "Scene.h"

#include "Components\TransformComponent.h"
#include "Components\TagComponent.h"
#include "Components\Renderable\CubeComponent.h"
#include "Components\Renderable\PlaneComponent.h"
#include "Components\Renderable\TriangleMeshComponent.h"

entt::entity Scene::CreateEntity(const std::string& name)
{
	const auto entity = m_Registry.create();
	AddComponent<TransformComponent>(entity);
	auto& tag = AddComponent<TagComponent>(entity);
	tag.Tag = name.empty() ? "Entity" : name;
	return entity;
}

void Scene::OnStart() const
{
	g_LitObjectShader->SetSceneData(m_SceneData);
	g_LitObjectShader->SetSceneData(m_SceneData);
	g_MirrorShader->SetSceneData(m_SceneData);
	g_RefractorShader->SetSceneData(m_SceneData);
	g_LineShader->SetSceneData(m_SceneData);
	g_SkyboxShader->SetSceneData(m_SceneData);
	g_ScreenShader->SetSceneData(m_SceneData);
}

void Scene::OnUpdate()
{
	const auto& renderer = m_Renderer.lock();
	if (!renderer) return;

	RenderSkybox();

	// Draw all Vertex Array Buffers
	for (const auto& entity : GetAllEntitiesWith<RenderableTag, MaterialComponent>())
	{
		UseMaterialShader(GetComponent<MaterialComponent>(entity));

		if (HasComponent<CubeComponent>(entity))
			renderer->RenderIndexed(GetComponent<CubeComponent>(entity).VAO);
		else if (HasComponent<PlaneComponent>(entity))
			renderer->RenderIndexed(GetComponent<PlaneComponent>(entity).VAO);
		else if (HasComponent<TriangleMeshComponent>(entity))
			renderer->RenderIndexed(GetComponent<TriangleMeshComponent>(entity).VAO);
	}
}

void Scene::RenderSkybox() const
{
	if (!m_SceneData.Skybox) return;

    // Do not set depth buffer
	glDepthMask(GL_FALSE);

    // Cube is seen from inside, so winding order is backwards
	glFrontFace(GL_CW);

    // Set active shader and texture(s)
	m_SceneData.Skybox->Material->m_Shader.lock()->Use();
    m_SceneData.Skybox->Material->m_Texture->Use();

    // Pass the skybox's mesh into the renderer
    const auto mesh = m_SceneData.Skybox->CubeMesh;
    auto renderer = m_Renderer.lock();
    if (renderer)
        renderer->RenderIndexed(mesh->VAO);

    // Reset flags
	glFrontFace(GL_CCW);

	glDepthMask(GL_TRUE);
}

void Scene::UseMaterialShader(const MaterialComponent& materialComponent)
{
	if (materialComponent.m_Shader.expired()) return;
	materialComponent.m_Shader.lock()->Use();

    SendMaterialDataToShader(materialComponent);
}

void Scene::SendMaterialDataToShader(const MaterialComponent& materialComponent)
{
    if (materialComponent.m_Shader.expired()) return;
    const auto& shader = materialComponent.m_Shader.lock();

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

    shader->SetInt("activeMaps", activeMaps);
    shader->SetFloat("shininess", materialComponent.m_Shininess);
}

