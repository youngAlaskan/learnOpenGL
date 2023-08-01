#pragma once
#include <unordered_map>

#include "..\Scene\Scene.h"
#include "..\Scene\Components\Line.h"
#include "..\Scene\Components\Renderable\Renderable.h"
#include "..\Scene\Components\Renderable\CubeComponent.h"
#include "..\Scene\Components\Renderable\PlaneComponent.h"
#include "..\Scene\Components\Renderable\TriangleMeshComponent.h"
#include "..\Scene\Components\MaterialComponent.h"

#include "Shader.h"
#include "..\Scene\Entity.h"

class Renderer
{
public:
	Renderer()
	{
		SetShaders();
	}

	explicit Renderer(std::weak_ptr<Scene> scene)
		: m_Scene(std::move(scene))
	{
		SetShaders();
	}

	void Render()
	{
		glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_RenderAxis)
			RenderAxis();

		RenderComponents();
	}

	void Render(const bool renderAxis, const bool renderNormals)
	{
		glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_RenderAxis = renderAxis;
		m_RenderNormals = renderNormals;
		if (m_RenderAxis)
			RenderAxis();

		RenderComponents();
	}

	void RenderComponents()
	{
		if (m_Scene.expired())
			return;

		std::shared_ptr<Scene> activeScene = m_Scene.lock();

		// Set Common Shader info
		// PointLight, Directional Light, SpotLight, Camera
		auto pointLights = std::vector<PointLightComponent>();
		for (auto handle : activeScene->GetAllEntitiesWith<PointLightComponent>())
			pointLights.emplace_back(Entity(handle, m_Scene).GetComponent<PointLightComponent>());

		auto directionalLights = std::vector<DirectionalLightComponent>();
		for (auto handle : activeScene->GetAllEntitiesWith<DirectionalLightComponent>())
			directionalLights.emplace_back(Entity(handle, m_Scene).GetComponent<DirectionalLightComponent>());

		auto spotLights = std::vector<SpotLightComponent>();
		for (auto handle : activeScene->GetAllEntitiesWith<SpotLightComponent>())
			spotLights.emplace_back(Entity(handle, m_Scene).GetComponent<SpotLightComponent>());

		for (const auto shader : m_Shaders)
		{
			if (shader)
			{
				shader->SetInt("pointLightCount", static_cast<int>(pointLights.size()));
				shader->SetPointLights(pointLights);
				shader->SetDirectionalLights(directionalLights);
				shader->SetSpotLights(spotLights);
				shader->SetCamera(activeScene->GetSceneCamera());
			}
		}

		for (auto renderableEntity : activeScene->GetAllEntitiesWith<RenderableTag>())
		{
			auto entity = Entity(renderableEntity, activeScene);
			if (entity.HasComponent<CubeComponent>())
				RenderObject3D<CubeComponent>(entity);
			else if (entity.HasComponent<PlaneComponent>())
				RenderObject3D<PlaneComponent>(entity);
			else if (entity.HasComponent<TriangleMeshComponent>())
				RenderObject3D<TriangleMeshComponent>(entity);
			else if (entity.HasComponent<Line>())
				RenderLine(entity);
		}
	}

	void SetUniformBuffer(const std::shared_ptr<UniformBufferComponent>& uniformBuffer, const std::string& name) const
	{
		m_IsolatedShader.SetUniformBuffer(uniformBuffer, name);
		m_LitObjectShader.SetUniformBuffer(uniformBuffer, name);
		m_MirrorShader.SetUniformBuffer(uniformBuffer, name);
		m_RefractorShader.SetUniformBuffer(uniformBuffer, name);
		m_LineShader.SetUniformBuffer(uniformBuffer, name);
		m_SkyboxShader.SetUniformBuffer(uniformBuffer, name);
		m_ScreenShader.SetUniformBuffer(uniformBuffer, name);
	}

public:
	std::weak_ptr<Scene> m_Scene;
	bool m_RenderNormals = false, m_RenderAxis = false;

private:
	void RenderAxis() const
	{
		m_LineShader.Use();
		m_LineShader.SetTransform({});

		m_LineShader.SetVec4("color", m_XAxis->m_Color);

		glBindVertexArray(m_XAxis->VAO);
		glDrawArrays(GL_LINES, 0, 2);
		
		m_LineShader.SetVec4("color", m_YAxis->m_Color);

		glBindVertexArray(m_YAxis->VAO);
		glDrawArrays(GL_LINES, 0, 2);
		
		m_LineShader.SetVec4("color", m_ZAxis->m_Color);

		glBindVertexArray(m_ZAxis->VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	template <typename Component>
	void RenderObject3D(Entity& entity)
	{
		const auto materialComponent = entity.GetComponent<MaterialComponent>();
		const auto shader = materialComponent.m_Shader.lock();
		shader->Use();

		shader->SetMaterial(materialComponent);

		const auto mesh = entity.GetComponent<Component>();
		glBindVertexArray(mesh.VAO);
		if (!mesh.EBO)
			glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mesh.VertexCount));
		else
			glDrawElements(GL_TRIANGLES, static_cast<int>(mesh.IndexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void RenderNormals(Entity& entity) const
	{
		m_LineShader.Use();

		m_LineShader.SetVec4("color", glm::vec4(1.0, 0.0, 1.0, 1.0));

		const auto object = entity.GetComponent<Object3D>();
		glBindVertexArray(object.NVAO);
		glDrawArrays(GL_LINES, 0, static_cast<GLint>(object.VertexCount * 2));
		glBindVertexArray(0);
	}

	void RenderCubemap(Entity& entity) const
	{
		glDepthMask(GL_FALSE);

		glFrontFace(GL_CW);

		m_SkyboxShader.Use();
		entity.GetComponent<TexCube>().Use();

		const auto mesh = entity.GetComponent<Object3D>();
		glBindVertexArray(mesh.VAO);
		if (!mesh.EBO)
			glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mesh.VertexCount));
		else
			glDrawElements(GL_TRIANGLES, static_cast<int>(mesh.IndexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glFrontFace(GL_CCW);

		glDepthMask(GL_TRUE);
	}

	void RenderScreenMesh(Entity& entity) const
	{
		m_ScreenShader.Use();
		entity.GetComponent<TexColorBuffer>().Use();

		const auto planeComponent = entity.GetComponent<PlaneComponent>();
		glBindVertexArray(planeComponent.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<int>(planeComponent.VertexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void RenderLine(Entity& entity) const
	{
		m_LineShader.Use();

		const auto line = entity.GetComponent<Line>();
		m_LineShader.SetVec4("color", line.m_Color);

		glBindVertexArray(line.VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void SetShaders()
	{
		m_IsolatedShader = Shader("positionNormalTex.vert", "uniformColor.frag");
		m_Shaders.emplace_back(&m_IsolatedShader);
		m_LitObjectShader = Shader("positionNormalTex.vert", "objectLitByVariousLights.frag");
		m_Shaders.emplace_back(&m_LitObjectShader);
		m_MirrorShader = Shader("positionNormalTex.vert", "skyboxMirror.frag");
		m_Shaders.emplace_back(&m_MirrorShader);
		m_RefractorShader = Shader("positionNormalTex.vert", "skyboxRefractor.frag");
		m_Shaders.emplace_back(&m_RefractorShader);
		m_LineShader = Shader("position.vert", "uniformColor.frag");
		m_Shaders.emplace_back(&m_LineShader);
		m_SkyboxShader = Shader("skybox.vert", "skybox.frag");
		m_Shaders.emplace_back(&m_SkyboxShader);
		m_ScreenShader = Shader("screen.vert", "texture2D.frag");
		m_Shaders.emplace_back(&m_ScreenShader);

		m_LitObjectShader.Use();
		for (int i = 0; i < 16; i++)
			m_LitObjectShader.SetInt("textures[" + std::to_string(i) + "]", i);

		m_MirrorShader.Use();
		m_MirrorShader.SetInt("skybox", 0);

		m_RefractorShader.Use();
		m_RefractorShader.SetInt("skybox", 0);

		m_SkyboxShader.Use();
		m_SkyboxShader.SetInt("skybox", 0);
	}

private:
	Shader m_IsolatedShader;
	Shader m_LitObjectShader;
	Shader m_MirrorShader;
	Shader m_RefractorShader;
	Shader m_LineShader;
	Shader m_SkyboxShader;
	Shader m_ScreenShader;
	std::vector<Shader*> m_Shaders;

	std::shared_ptr<Line> m_XAxis = std::make_shared<Line>(glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	std::shared_ptr<Line> m_YAxis = std::make_shared<Line>(glm::vec3(0.0f), glm::vec3(0.0f, 10.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	std::shared_ptr<Line> m_ZAxis = std::make_shared<Line>(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 10.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
};
