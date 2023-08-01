#pragma once

#include "ObjectTypes.h"

struct CubeComponent final : Object3D
{
	CubeComponent();
	CubeComponent(CubeComponent& cubeComponent) = default;
};