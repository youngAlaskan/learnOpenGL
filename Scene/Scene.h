#pragma once

#include "entt\include\entt.hpp"
#include "Camera.h"
#include "Model.h"

class Entity;

class Scene
{
public:
	Scene();
	Scene(const int viewportWidth, const int viewportHeight)
		: m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight) {}

	entt::registry& GetRegistry() { return m_Registry; }
	Entity CreateEntity(const std::string& name = std::string());

	std::shared_ptr<Camera> GetSceneCamera() { return m_SceneCamera; }

	template<typename... Components>
	auto GetAllEntitiesWith() { return m_Registry.view<Components...>(); }

	std::weak_ptr<Scene> GetWeakPtr() { return { std::shared_ptr<Scene>(this) }; }

	~Scene();

private:
	entt::registry m_Registry = entt::registry();
	std::shared_ptr<Camera> m_SceneCamera = std::make_shared<Camera>(glm::vec3(1.0f, 1.0f, 3.0f));
	int m_ViewportWidth = 0, m_ViewportHeight = 0;

	friend class Entity;
};
