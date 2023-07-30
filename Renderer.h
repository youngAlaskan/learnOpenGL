#pragma once
#include <vector>
#include <unordered_map>

#include "Line.h"
#include "TM.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer()
	{
		SetShaders();
	}

	explicit Renderer(std::shared_ptr<Scene> scene)
		: m_Scene(std::move(scene))
	{
		SetShaders();
	}

	~Renderer() = default;

	void Render()
	{
		for (const auto& drawable : m_Meshes)
		{
			switch (drawable->m_Type)
			{
			case DrawableType::TRIANGLE_MESH:
				RenderTriangleMesh(std::static_pointer_cast<TriangleMesh>(drawable));
				if (m_RenderNormals)
					RenderNormals(std::static_pointer_cast<TriangleMesh>(drawable));
				break;
			case DrawableType::CUBE_MAP:
				RenderCubemap(std::static_pointer_cast<CubemapMesh>(drawable));
				break;
			case DrawableType::SCREEN:
				RenderScreenMesh(std::static_pointer_cast<ScreenMesh>(drawable));
				break;
			case DrawableType::LINE:
				RenderLine(std::static_pointer_cast<Line>(drawable));
				break;
			}
		}
	}

public:
	std::shared_ptr<Scene> m_Scene;
	std::vector<std::shared_ptr<Drawable>> m_Meshes;
	bool m_RenderNormals = false;

private:
	void RenderTriangleMesh(const std::shared_ptr<TriangleMesh>& mesh) const
	{
		const auto& shader = m_Shaders.at(mesh->m_DrawingMode);
		shader.Use();
		shader.SetTransform(mesh->m_Transform, true);

		switch (mesh->m_DrawingMode)  // NOLINT(clang-diagnostic-switch-enum)
		{
		case DrawingMode::ISOLATED:
			shader.SetVec4("color", glm::vec4(1.0));
			break;
		case DrawingMode::LIT_OBJECT:
			if (mesh->m_Material)
				shader.SetMaterial(mesh->m_Material);

			shader.SetInt("pointLightCount", PointLight::GetCount());
			shader.SetPointLights(m_Scene->PointLights);
			shader.SetDirectionalLight(m_Scene->DirectionalLight);
			shader.SetSpotLight(m_Scene->SpotLight);

			shader.SetCamera(m_Scene->Camera);
			break;
		case DrawingMode::REFRACTOR:
			shader.SetFloat("refractiveIndex", mesh->m_RefractiveIndex);
		case DrawingMode::MIRROR:  // NOLINT(clang-diagnostic-implicit-fallthrough)
			if (m_Scene->Skybox)
				m_Scene->Skybox->Use();

			shader.SetCamera(m_Scene->Camera);
			break;
		default:
			std::cerr << "ERROR::RENDERER::RENDER::RENDER_TRIANGLE_MESH: Draw type is not valid for this mesh type!" << std::endl;
			return;
		}

		glBindVertexArray(mesh->m_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->m_VertexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void RenderNormals(const std::shared_ptr<TriangleMesh>& mesh) const
	{
		const auto& shader = m_Shaders.at(DrawingMode::LINES);
		shader.Use();
		shader.SetTransform(mesh->m_Transform);

		shader.SetVec4("color", glm::vec4(1.0, 0.0, 1.0, 1.0));

		glBindVertexArray(mesh->m_NVAO);
		glDrawArrays(GL_LINES, 0, static_cast<GLint>(mesh->m_VertexCount * 2));
		glBindVertexArray(0);
	}

	void RenderCubemap(const std::shared_ptr<CubemapMesh>& mesh) const
	{
		glDepthFunc(GL_LEQUAL);

		glFrontFace(GL_CW);

		m_Shaders.at(DrawingMode::SKYBOX).Use();
		mesh->m_Texture->Use();
		m_Shaders.at(DrawingMode::SKYBOX).SetMat4("view", mesh->m_Transform.View);
		m_Shaders.at(DrawingMode::SKYBOX).SetMat4("proj", mesh->m_Transform.Projection);

		glBindVertexArray(mesh->m_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->m_VertexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glFrontFace(GL_CCW);

		glDepthFunc(GL_LESS);
	}

	void RenderScreenMesh(const std::shared_ptr<ScreenMesh>& mesh) const
	{
		m_Shaders.at(DrawingMode::SCREEN).Use();
		mesh->m_Texture->Use();

		glBindVertexArray(mesh->m_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->m_VertexCount), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void RenderLine(const std::shared_ptr<Line>& line) const
	{
		const auto& shader = m_Shaders.at(DrawingMode::LINES);
		shader.Use();
		shader.SetTransform(line->m_Transform);

		shader.SetVec4("color", line->m_Color);

		glBindVertexArray(line->m_VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void SetShaders()
	{
		m_Shaders = std::unordered_map<DrawingMode, Shader>
		{
			{ DrawingMode::ISOLATED,   Shader("positionNormalTex.vert", "uniformColor.frag") },
			{ DrawingMode::LIT_OBJECT, Shader("positionNormalTex.vert", "objectLitByVariousLights.frag") },
			{ DrawingMode::MIRROR,     Shader("positionNormalTex.vert", "skyboxMirror.frag") },
			{ DrawingMode::REFRACTOR,  Shader("positionNormalTex.vert", "skyboxRefractor.frag") },
			{ DrawingMode::LINES,      Shader("position.vert",          "uniformColor.frag") },
			{ DrawingMode::SKYBOX,     Shader("skybox.vert",            "skybox.frag") },
			{ DrawingMode::SCREEN,     Shader("screen.vert",            "texture2D.frag") }
		};

		m_Shaders[DrawingMode::LIT_OBJECT].Use();
		for (int i = 0; i < 16; i++)
			m_Shaders[DrawingMode::LIT_OBJECT].SetInt("textures[" + std::to_string(i) + "]", i);

		m_Shaders[DrawingMode::MIRROR].Use();
		m_Shaders[DrawingMode::MIRROR].SetInt("skybox", 0);

		m_Shaders[DrawingMode::REFRACTOR].Use();
		m_Shaders[DrawingMode::REFRACTOR].SetInt("skybox", 0);

		m_Shaders[DrawingMode::SKYBOX].Use();
		m_Shaders[DrawingMode::SKYBOX].SetInt("skybox", 0);
	}

private:
	std::unordered_map<DrawingMode, Shader> m_Shaders;
};
