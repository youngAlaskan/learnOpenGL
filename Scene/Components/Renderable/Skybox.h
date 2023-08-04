#pragma once

#include <memory>

#include "CubeComponent.h"
#include "..\MaterialComponent.h"

struct Skybox
{
	std::shared_ptr<CubeComponent> CubeMesh = std::shared_ptr<CubeComponent>();
	std::shared_ptr<CubeMapMaterialComponent> Material;

	template<typename... Args>
	explicit Skybox(Args&&... args)
		: Material(std::forward<Args>(args)) {}
};
