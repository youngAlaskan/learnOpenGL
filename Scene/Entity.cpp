#include "Entity.h"

#include "Scene.h"

template<typename T, typename... Args>
T& Entity::AddComponent(Args&&... args)
{
	return m_Scene.lock()->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
}

template<typename T>
T& Entity::GetComponent()
{
	return m_Scene.lock()->m_Registry.get<T>(m_Handle);
}

template<typename T>
bool Entity::HasComponent() const
{
	return m_Scene.lock()->m_Registry.all_of<T>(m_Handle);
}

template<typename T>
unsigned int Entity::RemoveComponent() const
{
	return m_Scene.lock()->m_Registry.remove<T>(m_Handle);
}
