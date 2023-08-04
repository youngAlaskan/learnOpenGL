#pragma once

#include "Light.h"
#include "Components\Renderable\Skybox.h"

struct SceneData
{
	// Lights
	std::shared_ptr<std::vector<PointLight>> PointLights;
	std::shared_ptr<DirectionalLight> Sun;
	std::shared_ptr<SpotLight> Flashlight;
	// Skybox
	std::shared_ptr<Skybox> Skybox;
	// Camera Matrices
	std::shared_ptr<glm::mat4> ViewMatrix;
	std::shared_ptr<glm::mat4> ProjectionMatrix;
};
