#pragma once

#include <memory>

#include "CubeComponent.h"
#include "..\MaterialComponent.h"

struct Skybox
{
	std::shared_ptr<CubeComponent> CubeMesh = std::shared_ptr<CubeComponent>();
	std::shared_ptr<CubeMapMaterialComponent> Material;

	Skybox(std::shared_ptr<TexCube> texture)
		: Material(std::make_shared<CubeMapMaterialComponent>(texture)) {}
	Skybox(std::weak_ptr<Shader> shader, std::shared_ptr<TexCube> texture)
		: Material(std::make_shared<CubeMapMaterialComponent>(shader, texture)) {}
};
