#pragma once

#include "ObjectTypes.h"

struct PlaneComponent : Object3D
{
	PlaneComponent();
	PlaneComponent(PlaneComponent& cubeComponent) = default;
};
