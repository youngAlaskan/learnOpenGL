#pragma once

#include "Scene/Scene.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer()
	{
		m_BasicShader     = Shader("positionColorNormalTex.vert", "variableColor.frag");
		m_LitObjectShader = Shader("positionColorNormalTex.vert", "objectLitByVariousLights.frag");
		m_LightShader     = Shader("position.vert", "uniformColor.frag");
	}

	void SetActiveScene(const std::shared_ptr<Scene>& scene) { m_ActiveScene = scene; }
	std::shared_ptr<Scene> GetActiveScene() const {return m_ActiveScene;}

	void DrawMesh(Entity entity) const;

	~Renderer() = default;

private:
	Shader m_BasicShader = Shader(), m_LitObjectShader = Shader(), m_LightShader = Shader();
	std::shared_ptr<Scene> m_ActiveScene;
};