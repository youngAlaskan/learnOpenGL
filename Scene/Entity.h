#pragma once

#include <memory>
#include <utility>

#include "entt\include\entt.hpp"

class Scene;

class Entity
{
public:
	Entity() = default;
	Entity(const entt::entity& handle, std::weak_ptr<Scene> scene)
		: m_Handle(handle), m_Scene(std::move(scene)) {}
	Entity(const Entity&) = default;
	~Entity() = default;

	entt::entity GetHandle() const { return m_Handle; }
	void SetHandle(const entt::entity& handle) { m_Handle = handle; }
	void SetScene(const std::weak_ptr<Scene>& scene) { m_Scene = scene; }

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args);

	template<typename T>
	T& GetComponent();

	template<typename T>
	bool HasComponent() const;

	template<typename T>
	unsigned int RemoveComponent() const;

private:
	entt::entity m_Handle = entt::entity();
	std::weak_ptr<Scene> m_Scene;
};