#include "Scene.h"

#include "..\Renderer\Shader.h"
#include "Components\TransformComponent.h"
#include "Components\TagComponent.h"
#include "Components\MaterialComponent.h"
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

void Scene::OnUpdate()
{
	const auto& renderer = m_Renderer.lock();
	if (!renderer) return;

	for (const auto& entity : GetAllEntitiesWith<RenderableTag, MaterialComponent>())
	{
		const auto& shader = GetComponent<MaterialComponent>(entity).m_Shader.lock();
		if (!shader) continue;

		shader->Use();

		if (HasComponent<CubeComponent>(entity))
			renderer->RenderIndexed(GetComponent<CubeComponent>(entity).VAO);
		else if (HasComponent<PlaneComponent>(entity))
			renderer->RenderIndexed(GetComponent<PlaneComponent>(entity).VAO);
		else if (HasComponent<TriangleMeshComponent>(entity))
			renderer->RenderIndexed(GetComponent<TriangleMeshComponent>(entity).VAO);
	}
}
