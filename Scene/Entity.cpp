#include "Entity.h"

Entity::Entity(const entt::entity& handle, Scene* scene)
{
	m_Handle = handle;
	m_Scene = scene;
}
