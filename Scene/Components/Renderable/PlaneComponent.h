#pragma once

#include "Renderable.h"

struct PlaneComponent : Object3D
{
	PlaneComponent();
	PlaneComponent(PlaneComponent& cubeComponent) = default;
};
