#pragma once

#include "entt/include/entt.hpp"

class Scene
{
public:
	Scene() = default;
	~Scene() { m_Registry.clear(); }

public:
	entt::registry m_Registry;

};
