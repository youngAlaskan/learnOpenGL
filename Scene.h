#pragma once

#include <vector>

#include "Light.h"
#include "Camera.h"

struct Scene
{
	std::vector<std::shared_ptr<PointLight>> PointLights;
	std::shared_ptr<DirectionalLight> DirectionalLight;
	std::shared_ptr<SpotLight> SpotLight;
	std::shared_ptr<Camera> Camera;
};
