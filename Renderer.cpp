#include "Renderer.h"

#include <glad/glad.h>
#include "Scene/Components.h"
#include "Scene/Entity.h"

void Renderer::DrawMesh(Entity entity) const
{
	m_LitObjectShader.Use();
	m_LitObjectShader.SetMat4("Transform", entity.GetComponent<TransformComponent>());

	m_LitObjectShader.SetMat4("modelInv", glm::inverse(static_cast<glm::mat4>(entity.GetComponent<TransformComponent>())));

	if (entity.HasComponent<MaterialCubeComponent>())
		entity.GetComponent<MaterialCubeComponent>().SendToShader(m_LitObjectShader);

	for (auto& pointLightEntity : m_ActiveScene->GetAllEntitiesWith<PointLightComponent>())
	{
		((Entity&)pointLightEntity).GetComponent<PointLightComponent>().SendToShader(m_LitObjectShader);
	}

	for (auto& directionalLightEntity : m_ActiveScene->GetAllEntitiesWith<DirectionalLightComponent>())
		((Entity&)directionalLightEntity).GetComponent<DirectionalLightComponent>().SendToShader(m_LitObjectShader);

	const SpotLight spotLight;
	spotLight.SendToShader(m_LitObjectShader);

	m_ActiveScene->GetSceneCamera()->SendToShader(m_LitObjectShader);

	glBindVertexArray(entity.GetComponent<MeshComponent>());
	glDrawElements(GL_TRIANGLES, entity.GetComponent<MeshComponent>().VertexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
