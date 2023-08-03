#pragma once

#include "entt\include\entt.hpp"
#include "Camera.h"

#include "..\Renderer\Renderer.h"
#include "Light.h"

class Scene
{
public:
	Scene() = default;
	Scene(const int viewportWidth, const int viewportHeight)
		: m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight) {}

	entt::registry& GetRegistry() { return m_Registry; }
	entt::entity CreateEntity(const std::string& name = std::string());

	std::shared_ptr<Camera> GetSceneCamera() { return m_SceneCamera; }

	template<typename... Components>
	auto GetAllEntitiesWith() { return m_Registry.view<Components...>(); }

	template<typename Component, typename... Args>
	Component& AddComponent(const entt::entity entity, Args&&... args) { return m_Registry.emplace<Component>(entity, std::forward<Args>(args)...); }

	template<typename Component>
	Component& GetComponent(const entt::entity entity) { return m_Registry.get<Component>(entity); }

	template<typename Component>
	bool HasComponent(const entt::entity entity) const { return m_Registry.all_of<Component>(entity); }

	template<typename Component>
	unsigned int RemoveComponent(const entt::entity entity) const { return m_Registry.remove<Component>(entity); }

	std::weak_ptr<Scene> GetWeakPtr() { return { std::shared_ptr<Scene>(this) }; }

	void OnUpdate();

	~Scene() = default;

private:
	entt::registry m_Registry = entt::registry();
	std::shared_ptr<Camera> m_SceneCamera = std::make_shared<Camera>(glm::vec3(1.0f, 1.0f, 3.0f));
	int m_ViewportWidth = 0, m_ViewportHeight = 0;

	std::vector<PointLight> m_PointLights = std::vector<PointLight>();
	DirectionalLight m_DirectionalLight;
	SpotLight m_SpotLight;

	entt::entity m_Skybox;

	std::weak_ptr<Renderer> m_Renderer;
};
