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
	GLuint SkyboxTexture = 0;
	// Camera Matrices
	std::shared_ptr<glm::mat4> ViewMatrix;
	std::shared_ptr<glm::mat4> ProjectionMatrix;
};
