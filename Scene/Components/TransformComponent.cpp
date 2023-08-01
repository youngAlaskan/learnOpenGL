#include "TransformComponent.h"

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

glm::mat4 TransformComponent::GetTransform() const
{
	const glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

	return glm::translate(glm::mat4(1.0f), Translation)
		* rotation
		* glm::scale(glm::mat4(1.0f), Scale);
}
