#pragma once

#include "Scene.h"

#include "entt/include/entt.hpp"

class Entity
{
public:
	Entity() = default;
	Entity(const entt::entity& handle, Scene* scene);
	Entity(const Entity& other) = default;

	entt::entity GetHandle() const
	{
		return m_Handle;
	}

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		return m_Scene->m_Registry.get<T>(m_Handle);
	}

	template<typename T>
	bool HasComponent() const
	{
		return m_Scene->m_Registry.all_of<T>(m_Handle);
	}

	template<typename T>
	void RemoveComponent() const
	{
		m_Scene->m_Registry.remove<T>(m_Handle);
	}

	~Entity() = default;

private:
	entt::entity m_Handle = entt::entity();
	Scene* m_Scene;
};