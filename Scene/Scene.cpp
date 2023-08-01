#include "Scene.h"

#include "Entity.h"
#include "Components\TransformComponent.h"
#include "Components\TagComponent.h"

Scene::Scene()
{

}

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = { m_Registry.create(), GetWeakPtr() };
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;
	return entity;
}


Scene::~Scene()
{
	
}

